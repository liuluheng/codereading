#include <stdio.h>
#include "file.h"

char *program_name;

int print(void *data, const char *line)
{
    printf("%s\n", line);
    return 0;
}

int main(int argc, char *argv[])
{
    program_name = argv[0];

    file_for_each_line("file.h", print, NULL);

    print(NULL, argv[0]);

    return 0;
}
