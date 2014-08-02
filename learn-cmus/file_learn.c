#include <stdio.h>
#include "file.h"

char *program_name = NULL;

int print(void *data, const char *line)
{
    printf("%s%s\n", data, line);
    return 0;
}

int main(int argc, char *argv[])
{
    program_name = argv[0];

    char *p = "prompt> ";

    file_for_each_line("file.h", print, p);

    print(p, argv[0]);

    return 0;
}
