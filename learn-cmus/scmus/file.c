#include "file.h"
#include "xmalloc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

ssize_t read_all(int fd, void *buf, size_t count)
{
	char *buffer = buf;
	ssize_t pos = 0;

	do {
		ssize_t rc;

		rc = read(fd, buffer + pos, count - pos);
		if (rc == -1) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			return -1;
		}
		if (rc == 0) {
			/* eof */
			break;
		}
		pos += rc;
	} while (count - pos > 0);
	return pos;
}

ssize_t write_all(int fd, const void *buf, size_t count)
{
	const char *buffer = buf;
	int count_save = count;

	do {
		int rc;

		rc = write(fd, buffer, count);
		if (rc == -1) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			return -1;
		}
		buffer += rc;
		count -= rc;
	} while (count > 0);
	return count_save;
}

char *mmap_file(const char *filename, int *size)
{
	struct stat st;
	char *buf;
	int fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		goto err;

	if (fstat(fd, &st) == -1)
		goto close_err;

	/* can't mmap empty files */
	buf = NULL;
	if (st.st_size) {
		buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (buf == MAP_FAILED)
			goto close_err;
	}

	close(fd);
	*size = st.st_size;
	return buf;

close_err:
	close(fd);
err:
	*size = -1;
	return NULL;
}

void buffer_for_each_line(const char *buf, int size,
		int (*cb)(void *data, const char *line),
		void *data)
{
	char *line = NULL;
	int line_size = 0, pos = 0;

	while (pos < size) {
		int end, len;

		end = pos;
		while (end < size && buf[end] != '\n')
			end++;

		len = end - pos;
		if (end > pos && buf[end - 1] == '\r')
			len--;

		if (len >= line_size) {
			line_size = len + 1;
			line = xrenew(char, line, line_size);
		}
		memcpy(line, buf + pos, len);
		line[len] = 0;
		pos = end + 1;

		if (cb(data, line))
			break;
	}
	free(line);
}

void buffer_for_each_line_reverse(const char *buf, int size,
		int (*cb)(void *data, const char *line),
		void *data)
{
	char *line = NULL;
	int line_size = 0, end = size - 1;

	while (end >= 0) {
		int pos, len;

		if (end > 1 && buf[end] == '\n' && buf[end - 1] == '\r')
			end--;

		pos = end;
		while (pos > 0 && buf[pos - 1] != '\n')
			pos--;

		len = end - pos;
		if (len >= line_size) {
			line_size = len + 1;
			line = xrenew(char, line, line_size);
		}
		memcpy(line, buf + pos, len);
		line[len] = 0;
		end = pos - 1;

		if (cb(data, line))
			break;
	}
	free(line);
}

int file_for_each_line(const char *filename,
		int (*cb)(void *data, const char *line),
		void *data)
{
	char *buf;
	int size;

	buf = mmap_file(filename, &size);
	if (size == -1)
		return -1;

	if (buf) {
		buffer_for_each_line(buf, size, cb, data);
		munmap(buf, size);
	}
	return 0;
}
