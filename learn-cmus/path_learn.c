#include <stdio.h>
#include "path.h"

char *program_name = NULL;

int main(void)
{
    char src[1024] = "test/ /// /  aaa/.ttt/.sss";

    printf("%s\n", path_absolute(src));
    printf("%s\n", path_basename(path_absolute(src)));
    printf("%s\n", get_extension(path_absolute(src)) == NULL ? "NULL" : get_extension(path_absolute(src)));

    //printf("%s\n", (char*)NULL);

    return 0;
}
