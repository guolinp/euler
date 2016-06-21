#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <pthread.h>
#include "utility.h"

struct thread_settings {
	pthread_t tid;
	const char *name;
	void (*fun) (void *);
};

static void fun0(void *d)
{
	AWALYS_LOOPS() {
		P("this is function %s\n", __func__);
		sleep(4);
	}
}

static void fun1(void *d)
{
	AWALYS_LOOPS() {
		P("this is function %s\n", __func__);
		sleep(8);
	}
}

static void fun2(void *d)
{
	AWALYS_LOOPS() {
		P("this is function %s\n", __func__);
		sleep(16);
	}
}

static void fun3(void *d)
{
	AWALYS_LOOPS() {
		P("this is function %s\n", __func__);
		sleep(24);
	}
}

static void *run(void *settings)
{
	struct thread_settings *s = (struct thread_settings *) settings;
	GODIE_ON(prctl(PR_SET_NAME, s->name), "set thread name failed");
	s->fun(settings);
}

static struct thread_settings settings[] = {
	{0, "T0", fun0},
	{0, "T1", fun1},
	{0, "T2", fun2},
	{0, "T3", fun3},
};

int main(int argc, char *argv[])
{
	int i;

	for (i = 0; i < ARRAY_SIZE(settings); i++)
		settings[i].tid = NEW_THREAD(run, &settings[i]);

	for (i = 0; i < ARRAY_SIZE(settings); i++)
		GODIE_ON(pthread_join(settings[i].tid, NULL) != 0, "join %s thread failed", settings[i].name);

	return 0;
}

/* gcc thread_name.c -pthread -o thread_name */
