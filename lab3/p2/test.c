#include <stdio.h>
#include "n4_queue.h"

void print(void *smt)
{
    printf("%s\n", (char *) smt);
}

void main()
{
    n4_queue myq;

    n4_init(&myq);

    n4_add(&myq, print, "Function 1");
    n4_add(&myq, print, "Function 2");

    printf("Main thread: msg 1\n");

    n4_work(&myq);

    n4_add(&myq, print, "Function 3");

    printf("Main thread: msg 2\n");

    n4_work(&myq);

    n4_exit(&myq);

    printf("Main thread: msg 3\n");
}