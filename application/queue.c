#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include "utility.h"

struct queue {
	unsigned int size;
	unsigned int *data;
	unsigned int reader;
	unsigned int writer;
	pthread_mutex_t lock;
};

int q_create(struct queue **q, unsigned int size)
{
	struct queue *new;

	GODIE_ON(q == NULL, "invalid arguments, q");
	GODIE_ON(size == 0, "invalid arguments, size");

	new = MALLOC(sizeof(*new) + sizeof(unsigned int) * (size + 1));
	GODIE_ON(pthread_mutex_init(&new->lock, NULL), "init queue lock failed");
	new->size = size + 1;
	new->data = (unsigned int *) &new[1];
	new->reader = 0;
	new->writer = 0;
	*q = new;
	return 0;
}

void q_destory(struct queue *q)
{
	GODIE_ON(q == NULL, "invalid arguments, q");
	GODIE_ON(pthread_mutex_destroy(&q->lock) != 0, "destroy lock failed");
	FREE(q);
}

static inline int __q_is_full(struct queue *q)
{
	int full = ((q->writer + 1) % q->size) == q->reader;
	return full;
}

static inline int __q_is_empty(struct queue *q)
{
	int empty = q->reader == q->writer;
	return empty;
}

int q_is_full(struct queue *q)
{
	int full;
	GODIE_ON(q == NULL, "invalid arguments, q");
	GODIE_ON(pthread_mutex_lock(&q->lock) != 0, "lock failed");
	full = __q_is_full(q);
	GODIE_ON(pthread_mutex_unlock(&q->lock) != 0, "unlock failed");
	return full;
}

int q_is_empty(struct queue *q)
{
	int empty;
	GODIE_ON(q == NULL, "invalid arguments, q");
	GODIE_ON(pthread_mutex_lock(&q->lock) != 0, "lock failed");
	empty = __q_is_empty(q);
	GODIE_ON(pthread_mutex_unlock(&q->lock) != 0, "unlock failed");
	return empty;
}

int q_enqueue(struct queue *q, unsigned int data)
{
	unsigned int next;

	GODIE_ON(q == NULL, "invalid arguments, q");
	GODIE_ON(pthread_mutex_lock(&q->lock) != 0, "lock failed");
	if (__q_is_full(q)) {
		GODIE_ON(pthread_mutex_unlock(&q->lock) != 0, "unlock failed");
		return -EAGAIN;
	}

	next = (q->writer + 1) % q->size;
	q->data[next] = data;
	q->writer = next;
	GODIE_ON(pthread_mutex_unlock(&q->lock) != 0, "unlock failed");

	return 0;
}

int q_dequeue(struct queue *q, unsigned int *data)
{
	unsigned int next;

	GODIE_ON(q == NULL, "invalid arguments, q");
	GODIE_ON(data == NULL, "invalid arguments, data");

	GODIE_ON(pthread_mutex_lock(&q->lock) != 0, "lock failed");
	if (__q_is_empty(q)) {
		GODIE_ON(pthread_mutex_unlock(&q->lock) != 0, "unlock failed");
		return -ENODEV;
	}

	next = (q->reader + 1) % q->size;
	*data = q->data[next];
	q->reader = next;
	GODIE_ON(pthread_mutex_unlock(&q->lock) != 0, "unlock failed");

	return 0;
}



#if 0
void queue_test(void)
{
	struct queue *q;
	int ret;
	unsigned int i, data;

	ret = q_create(&q, 512);
	P("create queue, ret=%d\n", ret);

	for (i = 0; i < 522; i++) {
		P("i=%d, add data(%d) into queue", i, i);
		if (q_enqueue(q, i))
			P("i=%d, add data(%d) into queue failed\n", i, i);
		else
			P("i=%d, add data(%d) into queue ok\n", i, i);
	}

	for (i = 0; i < 522; i++) {
		if (q_dequeue(q, &data))
			P("i=%d, read data from queue failed\n", i);
		else
			P("i=%d, read data(%d) from queue ok\n", i, data);
	}
}

int main(int argc, char *argv[])
{
	queue_test();
	return 0;
}

/* gcc queue.c -o queue */
#endif
