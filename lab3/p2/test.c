#include <stdio.h>
#include "n4_queue.h"

void print(void *smt)
{
    printf("%s\n", (char *) smt);
}

void main()
{
    n4_queue myq, queue2;

    n4_init(&myq);

    n4_add(&myq, print, "Thread 1: Function 1");
    n4_add(&myq, print, "Thread 1: Function 2");

    printf("Main thread: msg 1\n");

    n4_work(&myq);

    n4_add(&myq, print, "Thread 1: Function 3");

    n4_init(&queue2);
    n4_add(&queue2, print, "Thread 2: Function 1");
    n4_add(&queue2, print, "Thread 2: Function 2");

    printf("Main thread: msg 2\n");

    n4_work(&myq);
    n4_work(&queue2);

    n4_exit(&myq);
    n4_exit(&queue2);

    printf("Main thread: msg 3\n");
}
