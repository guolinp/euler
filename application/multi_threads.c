#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "utility.h"

static int x = 0;
static int thread_index[] = { 0, 1, 2, 3 };

static void *run(void *data)
{
	int index = *(int *) data;

	AWALYS_LOOPS() {
		x = index;
		/* !!! */
		if (x != index)
			P("thread index %d, x=%d\n", index, x);
	}
}

int main(int argc, char *argv[])
{
	int i;

	for (i = 0; i < ARRAY_SIZE(thread_index); i++)
		NEW_THREAD(run, &thread_index[i]);

	AWALYS_LOOPS();

	return 0;
}

/* gcc multi_threads.c -pthread -o multi_threads */
