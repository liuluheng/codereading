#include "debug.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static char error_buf[512];

void error_msg(const char *format, ...)
{
    va_list ap;

    strcpy(error_buf, "Error: ");
    va_start(ap, format);
    vsnprintf(error_buf + 7, sizeof(error_buf) - 7, format, ap);
    va_end(ap);

    /*
    d_print("%s\n", error_buf);
    if (client_fd != -1) {
        write_all(client_fd, error_buf, strlen(error_buf));
        write_all(client_fd, "\n", 1);
    }

    msg_is_error = 1;
    error_count++;

    if (ui_initialized) {
        error_time = time(NULL);
        update_commandline();
    } else {
        warn("%s\n", error_buf);
        error_buf[0] = 0;
    }
    */
}
