#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "utility.h"
#include "queue.h"

#define NR_OBJECTS    2

struct object {
	int data;
};

static struct queue *qobj;
static pthread_mutex_t lock;
static pthread_cond_t can_create;
static pthread_cond_t can_destory;

static inline struct object *new_object(int data)
{
	struct object *obj = MALLOC(sizeof(*obj));
	obj->data = data;
	return obj;
}

static inline void delete_object(struct object *obj)
{
	FREE(obj);
}

static void *create_objects(void *unused)
{
	int data = 0;
	AWALYS_LOOPS() {
		GODIE_ON(pthread_mutex_lock(&lock), "lock failed");
		while (q_is_full(qobj))
			GODIE_ON(pthread_cond_wait(&can_create, &lock), "wait cond failed");
		GODIE_ON(q_enqueue(qobj, (unsigned int) new_object(data)), "enqueue data %d failed", data);
		P("enqueue data %d ok\n", data);
		data++;
		GODIE_ON(pthread_mutex_unlock(&lock), "unlock failed");
		GODIE_ON(pthread_cond_signal(&can_destory), "signal failed");
	}
}

static void *destory_objects(void *unused)
{
	unsigned int data;
	AWALYS_LOOPS() {
		GODIE_ON(pthread_mutex_lock(&lock), "lock failed");
		while (q_is_empty(qobj))
			GODIE_ON(pthread_cond_wait(&can_destory, &lock), "wait cond failed");
		GODIE_ON(q_dequeue(qobj, &data), "enqueue data failed");
		P("dequeue data %d ok\n", ((struct object *) data)->data);
		delete_object((struct object *) data);
		GODIE_ON(pthread_mutex_unlock(&lock), "unlock failed");
		GODIE_ON(pthread_cond_signal(&can_create), "signal failed");
	}
}

int main(int argc, char *argv[])
{
	int qsize = NR_OBJECTS;
	pthread_t create_objects_tid;
	pthread_t destory_objects_tid;

	if (argv[1]) {
		qsize = strtol(argv[1], NULL, 10);
		if (qsize <= 0)
			qsize = NR_OBJECTS;
	}

	GODIE_ON(q_create(&qobj, qsize) != 0, "create queue failed");

	GODIE_ON(pthread_mutex_init(&lock, NULL) != 0, "init 'lock' failed");
	GODIE_ON(pthread_cond_init(&can_create, NULL) != 0, "init 'can_create' failed");
	GODIE_ON(pthread_cond_init(&can_destory, NULL) != 0, "init 'can_destory' failed");

	create_objects_tid = NEW_THREAD(create_objects, NULL);
	destory_objects_tid = NEW_THREAD(destory_objects, NULL);

	GODIE_ON(pthread_join(create_objects_tid, NULL) != 0, "join 'create_objects' thread failed");
	GODIE_ON(pthread_join(destory_objects_tid, NULL) != 0, "join 'destory_objects' thread failed");

	GODIE_ON(pthread_mutex_destroy(&lock) != 0, "destory 'lock' failed");

	q_destory(qobj);

	return 0;
}

/* gcc thread_cond.c queue.c -pthread -o thread_cond */
