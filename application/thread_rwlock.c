#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "utility.h"
#include "queue.h"

#define NR_OBJECTS    1

struct object {
	int data;
};

struct thread_settings {
	pthread_t tid;
	const char *name;
	void *(*fun) (void *);
};

static struct queue *qobj;
static pthread_rwlock_t rwlock;

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

static void *write_objects(void *settings)
{
	struct thread_settings *s = (struct thread_settings *) settings;
	int data = 0;
	AWALYS_LOOPS() {
		GODIE_ON(pthread_rwlock_wrlock(&rwlock) != 0, "wrlock failed");
		if (!q_is_full(qobj)) {
			GODIE_ON(q_enqueue(qobj, (unsigned int) new_object(data)), "enqueue data %d failed", data);
			P("%s: enqueue data %d ok\n", s->name, data);
			data++;
		}
		GODIE_ON(pthread_rwlock_unlock(&rwlock) != 0, "rwunlock failed");
		sleep(1);
	}
}

static void *read_objects(void *settings)
{
	struct thread_settings *s = (struct thread_settings *) settings;
	unsigned int data;
	AWALYS_LOOPS() {
		GODIE_ON(pthread_rwlock_rdlock(&rwlock) != 0, "rdlock failed");
		if (!q_is_empty(qobj)) {
			GODIE_ON(q_dequeue(qobj, &data) != 0, "dequeue data failed");
			P("%s: dequeue data %d ok\n", s->name, ((struct object *) data)->data);
			delete_object((struct object *) data);
		}
		GODIE_ON(pthread_rwlock_unlock(&rwlock) != 0, "rwunlock failed");
		usleep(10000);
	}
}

static struct thread_settings settings[] = {
	{0, "reader_0", read_objects},
	{0, "reader_1", read_objects},
	{0, "writer_0", write_objects},
	{0, "writer_1", write_objects},
};

int main(int argc, char *argv[])
{
	int qsize = NR_OBJECTS;
	int i;

	if (argv[1]) {
		qsize = strtol(argv[1], NULL, 10);
		if (qsize <= 0)
			qsize = NR_OBJECTS;
	}

	GODIE_ON(q_create(&qobj, qsize) != 0, "create queue failed");

	GODIE_ON(pthread_rwlock_init(&rwlock, NULL) != 0, "init 'rwlock' failed");

	for (i = 0; i < ARRAY_SIZE(settings); i++)
		settings[i].tid = NEW_THREAD(settings[i].fun, &settings[i]);

	for (i = 0; i < ARRAY_SIZE(settings); i++)
		GODIE_ON(pthread_join(settings[i].tid, NULL) != 0, "join %s thread failed", settings[i].name);

	GODIE_ON(pthread_rwlock_destroy(&rwlock) != 0, "destory 'rwlock' failed");

	q_destory(qobj);

	return 0;
}

/* gcc thread_rwlock.c queue.c -pthread -o thread_rwlock */
