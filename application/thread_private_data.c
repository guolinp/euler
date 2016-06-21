#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "utility.h"

struct task_notepad {
	pthread_t tid;
	int data;
};

static pthread_key_t key;
static int thread_index[] = { 0, 1, 2, 3 };

static void dump_task_notepad(int index, struct task_notepad *notes)
{
	P("thread %d, notepad: address %p, tid 0x%x, data %d\n", index, notes, (int) notes->tid, notes->data);
}

static void *run(void *data)
{
	int index = *(int *) data;
	struct task_notepad *notes = MALLOC(sizeof(*notes));

	notes->tid = pthread_self();
	notes->data = 0;
	dump_task_notepad(index, notes);

	GODIE_ON(pthread_setspecific(key, notes) != 0, "set thread private data failed");

	AWALYS_LOOPS() {
		notes = pthread_getspecific(key);
		notes->data = index;
		dump_task_notepad(index, notes);
		sleep(1);
	}
}

int main(int argc, char *argv[])
{
	int i;

	GODIE_ON(pthread_key_create(&key, NULL) != 0, "Init thread key failed");

	for (i = 0; i < ARRAY_SIZE(thread_index); i++)
		NEW_THREAD(run, &thread_index[i]);

	AWALYS_LOOPS();

	return 0;
}

/* gcc thread_private_data.c -pthread -o thread_private_data */
