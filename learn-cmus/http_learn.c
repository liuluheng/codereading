#include <stdio.h>
#include <unistd.h>

#include "xmalloc.h"
#include "http.h"
#include "debug.h"
#include "ip.h"
#include "keyval.h"

/* timeouts (ms) */
static int http_connection_timeout = 5e3;
static int http_read_timeout = 5e3;

static void keyvals_add_basic_auth(struct growing_keyvals *c,
				   const char *user,
				   const char *pass,
				   const char *header)
{
	char buf[256];
	char *encoded;

	snprintf(buf, sizeof(buf), "%s:%s", user, pass);
	encoded = base64_encode(buf);
	if (encoded == NULL) {
		d_print("couldn't base64 encode '%s'\n", buf);
	} else {
		snprintf(buf, sizeof(buf), "Basic %s", encoded);
		free(encoded);
		keyvals_add(c, header, xstrdup(buf));
	}
}

static int do_http_get(struct http_get *hg, 
                       const char *uri, int redirections)
{
	GROWING_KEYVALS(h);
	int i, rc;
	const char *val;
	char *redirloc;

	d_print("%s\n", uri);

	hg->headers = NULL;
	hg->reason = NULL;
	hg->code = -1;
	hg->fd = -1;
	if (http_parse_uri(uri, &hg->uri))
		return -IP_ERROR_INVALID_URI;

	if (http_open(hg, http_connection_timeout))
		return -IP_ERROR_ERRNO;

	keyvals_add(&h, "Host", xstrdup(hg->uri.host));
	if (hg->proxy && hg->proxy->user && hg->proxy->pass)
		keyvals_add_basic_auth(&h, hg->proxy->user, hg->proxy->pass, "Proxy-Authorization");
	keyvals_add(&h, "User-Agent", xstrdup("cmus/" VERSION));
	keyvals_add(&h, "Icy-MetaData", xstrdup("1"));
	if (hg->uri.user && hg->uri.pass)
		keyvals_add_basic_auth(&h, hg->uri.user, hg->uri.pass, "Authorization");
	keyvals_terminate(&h);

	rc = http_get(hg, h.keyvals, http_read_timeout);
	keyvals_free(h.keyvals);
	switch (rc) {
	case -1:
		return -IP_ERROR_ERRNO;
	case -2:
		return -IP_ERROR_HTTP_RESPONSE;
	}

	d_print("HTTP response: %d %s\n", hg->code, hg->reason);
	for (i = 0; hg->headers[i].key != NULL; i++)
		d_print("  %s: %s\n", hg->headers[i].key, hg->headers[i].val);

	switch (hg->code) {
	case 200: /* OK */
		return 0;
	/*
	 * 3xx Codes (Redirections)
	 *     unhandled: 300 Multiple Choices
	 */
	case 301: /* Moved Permanently */
	case 302: /* Found */
	case 303: /* See Other */
	case 307: /* Temporary Redirect */
		val = keyvals_get_val(hg->headers, "location");
		if (!val)
			return -IP_ERROR_HTTP_RESPONSE;

		redirections++;
		if (redirections > 2)
			return -IP_ERROR_HTTP_REDIRECT_LIMIT;

		redirloc = xstrdup(val);
		http_get_free(hg);
		close(hg->fd);

		rc = do_http_get(hg, redirloc, redirections);

		free(redirloc);
		return rc;
	default:
		return -IP_ERROR_HTTP_STATUS;
	}
}

int main(void)
{
    debug_init();
    struct http_get *hg = xnew(struct http_get, 1);

    //const char *uri = "http://www.baidu.com/index.php?tn=10018801_hao";
    const char *uri = "http://www.taobao.com/";

    //http_parse_uri(uri, &hg->uri);

    //printf("host:%s, port:%d, path:%s\n", 
    //        hg->uri.host, hg->uri.port, hg->uri.path);

    do_http_get(hg, uri, 0);

    http_get_free(hg);
    return 0;
}

