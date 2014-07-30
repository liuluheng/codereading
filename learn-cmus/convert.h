#ifndef CONVERT_H
#define CONVERT_H

#include <sys/types.h> /* ssize_t */

/* Returns length of *outbuf in bytes (without closing '\0'), -1 on error. */
ssize_t convert(const char *inbuf, ssize_t inbuf_size,
		char **outbuf, ssize_t outbuf_estimate,
		const char *tocode, const char *fromcode);

int utf8_encode(const char *inbuf, const char *encoding, char **outbuf);

char *to_utf8(const char *str, const char *enc);

#endif
