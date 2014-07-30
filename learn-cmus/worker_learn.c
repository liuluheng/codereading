#include "worker.h"
#include "debug.h"

#include <stdio.h>
#include <unistd.h>

void print(void *i)
{
    printf("job_cb, %s\n", (char *)i);
}

void fre(void *i)
{
    printf("free_cb, %s\n", (char *)i);
}


int main(void)
{
    debug_init();
    worker_init();

    char *str = "hello";
    worker_add_job(1, print, fre, str);
    worker_add_job(1, print, fre, str);


    sleep(4);
    worker_exit();


    return 0;
}
