#include "n4_queue.h"

static void *work_thread(void *data)
{
	n4_queue *queue;

	queue = data;

	do {
		n4_funct *ptr;

		sem_wait(&(queue->semaphore));

		ptr = queue->list;

		while (ptr) {
			(*(ptr->function))(ptr->parameter);
			ptr = ptr->next;
		}
	} while (1);
}

int n4_init(n4_queue *head)
{
	head->list = NULL;
	sem_init(&(head->semaphore), 0, 0);

	return pthread_create(&(head->thread_id), NULL, &work_thread, head);
}

int n4_add(n4_queue *head, void (*fun)(void *), void *param)
{
	n4_funct *ptr1, *ptr2;

	ptr1 = malloc(sizeof(n4_funct));
	if (!ptr1)
		return -1;

	ptr1->next = NULL;
	ptr1->function  = fun;
	ptr1->parameter = param;

	ptr2 = head->list;
	if (!ptr2)
		head->list = ptr1;
	else {
		while (ptr2->next)
			ptr2 = ptr2->next;

		ptr2->next = ptr1;
	}

	return 0;
}

void n4_exit(n4_queue *head)
{
	n4_funct *ptr1, *ptr2;

	pthread_cancel(head->thread_id);
	pthread_join(head->thread_id, NULL);

	ptr1 = head->list;
	if (ptr1)
		while (ptr1->next) {
			ptr2 = ptr1->next;
			free(ptr1);
			ptr1 = ptr2;
		}
}