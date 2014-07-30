#ifndef _FILE_H
#define _FILE_H

#include <stddef.h> /* size_t */
#include <sys/types.h> /* ssize_t */

ssize_t read_all(int fd, void *buf, size_t count);
ssize_t write_all(int fd, const void *buf, size_t count);

/* @filename  file to mmap for reading
 * @size      returned size of the file or -1 if failed
 *
 * returns buffer or NULL if empty file or failed
 */
char *mmap_file(const char *filename, int *size);

void buffer_for_each_line(const char *buf, int size,
		int (*cb)(void *data, const char *line),
		void *data);
void buffer_for_each_line_reverse(const char *buf, int size,
		int (*cb)(void *data, const char *line),
		void *data);
int file_for_each_line(const char *filename,
		int (*cb)(void *data, const char *line),
		void *data);

#endif
