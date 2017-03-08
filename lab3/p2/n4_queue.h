#ifndef _N4_QUEUE_H
#define _N4_QUEUE_H

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct __n4_funct
{
    struct __n4_funct *next;
    void (*function)(void *);
    void *parameter;
} n4_funct;

typedef struct __n4_queue
{
    n4_funct  *list;
    pthread_t thread_id;
    sem_t     semaphore;
} n4_queue;

#define n4_work(head) sem_post(&((head)->semaphore));

int n4_init(n4_queue *head);
int n4_add(n4_queue *head, void (*fun)(void *), void *param);
void n4_exit(n4_queue *head);

#endif