#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>


struct http_uri {
    char *uri;
    char *user;
    char *pass;
    char *host;
    char *path;
    int port;
};

struct http_get {
    struct http_uri uri;
    struct http_uri *proxy;
    int fd;
    char *reason;
    int code;
};

void http_free_uri(struct http_uri *u);

int http_parse_uri(const char *uri, struct http_uri *u)
{
	const char *str, *colon, *at, *slash, *host_start;

	/* initialize all fields */
	u->uri  = strdup(uri);
	u->user = NULL;
	u->pass = NULL;
	u->host = NULL;
	u->path = NULL;
	u->port = 80;

	if (strncmp(uri, "http://", 7))
		return -1;
	str = uri + 7;
	host_start = str;

	/* [/path] */
	slash = strchr(str, '/');
	if (slash) {
		u->path = strdup(slash);
	} else {
		u->path = strdup("/");
	}

	/* [user[:pass]@] */
	at = strchr(str, '@');
	if (at) {
		/* user[:pass]@ */
		host_start = at + 1;
		colon = strchr(str, ':');
		if (colon == NULL || colon > at) {
			/* user */
			u->user = strndup(str, at - str);
		} else {
			/* user:pass */
			u->user = strndup(str, colon - str);
			u->pass = strndup(colon + 1, at - (colon + 1));
		}
	}

	/* host[:port] */
	colon = strchr(host_start, ':');
	if (colon) {
		/* host:port */
		const char *start;
		int port;

		u->host = strndup(host_start, colon - host_start);
		colon++;
		start = colon;

		port = 0;
		while (*colon >= '0' && *colon <= '9') {
			port *= 10;
			port += *colon - '0';
			colon++;
		}
		u->port = port;

		if (colon == start || (*colon != 0 && *colon != '/')) {
			http_free_uri(u);
			return -1;
		}
	} else {
		/* host */
		if (slash) {
			u->host = strndup(host_start, slash - host_start);
		} else {
			u->host = strdup(host_start);
		}
	}
	return 0;
}

void http_free_uri(struct http_uri *u)
{
	free(u->uri);
	free(u->user);
	free(u->pass);
	free(u->host);
	free(u->path);

	u->uri  = NULL;
	u->user = NULL;
	u->pass = NULL;
	u->host = NULL;
	u->path = NULL;
}


int http_open(struct http_get *hg, int timeout_ms)
{
	const struct addrinfo hints = {
		.ai_socktype = SOCK_STREAM
		//.ai_socktype = SOCK_DGRAM
	};
	struct addrinfo *result;
	union {
		struct sockaddr sa;
		struct sockaddr_storage sas;
	} addr;
	size_t addrlen;
	struct timeval tv;
	int save, flags, rc;
	char port[16];

	char *proxy = (char *)getenv("http_proxy");
	if (proxy) {
        hg->proxy = calloc(1, sizeof(struct http_uri));
        printf("Failed to parse HTTP proxy URI '%s'\n", proxy);
        return -1;
	} else {
		hg->proxy = NULL;
	}

	snprintf(port, sizeof(port), "%d", hg->proxy ? hg->proxy->port : hg->uri.port);
	rc = getaddrinfo(hg->proxy ? hg->proxy->host : hg->uri.host, port, &hints, &result);
	if (rc != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(rc));
		return -1;
	}
	memcpy(&addr.sa, result->ai_addr, result->ai_addrlen);
	addrlen = result->ai_addrlen;
	freeaddrinfo(result);

	hg->fd = socket(addr.sa.sa_family, SOCK_STREAM, 0);
	//hg->fd = socket(addr.sa.sa_family, SOCK_DGRAM, 0);
	if (hg->fd == -1)
		return -1;

	flags = fcntl(hg->fd, F_GETFL);
	if (fcntl(hg->fd, F_SETFL, O_NONBLOCK) == -1)
		goto close_exit;

	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;
	while (1) {
		fd_set wfds;

		printf("connecting. timeout=%lld s %lld us\n", (long long)tv.tv_sec, (long long)tv.tv_usec);
		if (connect(hg->fd, &addr.sa, addrlen) == 0) {
            printf("haha, connected\n");
			break;
        }
		if (errno == EISCONN)
			break;
		if (errno != EAGAIN && errno != EINPROGRESS)
			goto close_exit;

		FD_ZERO(&wfds);
		FD_SET(hg->fd, &wfds);
		while (1) {
            printf("cannt connect, ok i select first\n");
			rc = select(hg->fd + 1, NULL, &wfds, NULL, &tv);
			if (rc == -1) {
				if (errno != EINTR)
					goto close_exit;
				/* signalled */
				continue;
			}
			if (rc == 1) {
				/* socket ready */
                printf("haha, ready to connect\n");
				break;
			}
			if (tv.tv_sec == 0 && tv.tv_usec == 0) {
				errno = ETIMEDOUT;
				goto close_exit;
			}
		}
	}

	/* restore old flags */
	if (fcntl(hg->fd, F_SETFL, flags) == -1)
		goto close_exit;

	return 0;
close_exit:
	save = errno;
	close(hg->fd);
	errno = save;
	return -1;
}

static int read_timeout(int fd, int timeout_ms)
{
	struct timeval tv;

	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;
	while (1) {
		fd_set rfds;
		int rc;

		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		rc = select(fd + 1, &rfds, NULL, NULL, &tv);
		if (rc == -1) {
			if (errno != EINTR)
				return -1;
			/* signalled */
			continue;
		}
		if (rc == 1)
			return 0;
		if (tv.tv_sec == 0 && tv.tv_usec == 0) {
			errno = ETIMEDOUT;
			return -1;
		}
	}
}

/* reads response, ignores fscking carriage returns */
//static int http_read_response(int fd, struct gbuf *buf, int timeout_ms)
static int http_read_response(int fd, int timeout_ms)
{
	char prev = 0;

	if (read_timeout(fd, timeout_ms))
		return -1;
	while (1) {
		int rc;
		char ch;

		rc = read(fd, &ch, 1);
		if (rc == -1) {
			return -1;
		}
		if (rc == 0) {
			return -2;
		}
		if (ch == '\r')
			continue;
		if (ch == '\n' && prev == '\n')
			return 0;
		putchar(ch);
		prev = ch;
	}
}

static int http_write(int fd, const char *buf, int count, int timeout_ms)
{
	struct timeval tv;
	int pos = 0;

	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;
	while (1) {
		fd_set wfds;
		int rc;

		printf("timeout=%lld s %lld us\n", (long long)tv.tv_sec, (long long)tv.tv_usec);

		FD_ZERO(&wfds);
		FD_SET(fd, &wfds);
		rc = select(fd + 1, NULL, &wfds, NULL, &tv);
		if (rc == -1) {
			if (errno != EINTR)
				return -1;
			/* signalled */
			continue;
		}
		if (rc == 1) {
            printf("writing...\n");
			rc = write(fd, buf + pos, count - pos);
			if (rc == -1) {
				if (errno == EINTR || errno == EAGAIN)
					continue;
				return -1;
			}
			pos += rc;
			if (pos == count) {
                printf("return\n");
				return 0;
            }
		} else if (tv.tv_sec == 0 && tv.tv_usec == 0) {
			errno = ETIMEDOUT;
			return -1;
		}
	}
}

void http_get_free(struct http_get *hg)
{
    http_free_uri(&hg->uri);
    if (hg->proxy) {
        http_free_uri(hg->proxy);
        free(hg->proxy);
    }

    free(hg->reason);
}

int main(void)
{
    struct http_get *hg = (struct http_get *)calloc(1, sizeof(struct http_get));
    //hg->uri.host = "localhost";
    //hg->uri.port = 5000;
    
    //hg->uri.host = "www.baidu.com";
    //hg->uri.port = 80;

    //const char *uri = "http://www.baidu.com/index.php?tn=10018801_hao";
    const char *uri = "http://www.taobao.com/";

    http_parse_uri(uri, &hg->uri);

    printf("%s, port %d, path %s\n", hg->uri.host, hg->uri.port, hg->uri.path);

    http_open(hg, 5000);

    char buf[] = "GET HTTP/1.0\r\n\r\n";

    http_write(hg->fd, buf, sizeof(buf), 5000);
    http_read_response(hg->fd, 5000);

    http_get_free(hg);
    return 0;
}
