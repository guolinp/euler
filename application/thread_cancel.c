#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "utility.h"

struct thread_setting {
	pthread_t tid;
	int index;
	int cancel_state;
	int cancel_type;
	int has_cancel_point;
};

struct thread_setting settings[] = {
	{0, 0, PTHREAD_CANCEL_ENABLE, PTHREAD_CANCEL_DEFERRED, 0},
	{0, 1, PTHREAD_CANCEL_ENABLE, PTHREAD_CANCEL_ASYNCHRONOUS, 0},
	{0, 2, PTHREAD_CANCEL_ENABLE, PTHREAD_CANCEL_DEFERRED, 1},
	{0, 3, PTHREAD_CANCEL_ENABLE, PTHREAD_CANCEL_ASYNCHRONOUS, 1},
	{0, 4, PTHREAD_CANCEL_DISABLE, 0, 0},
};

static void *run(void *data)
{
	struct thread_setting *s = (struct thread_setting *) data;
	int n = 0;

	P("enter thread %d\n", s->index);

	GODIE_ON(s == NULL, "invalid input");

	GODIE_ON(pthread_setcancelstate(s->cancel_state, NULL), "set thread cancel state failed");
	if (s->cancel_state != PTHREAD_CANCEL_DISABLE)
		GODIE_ON(pthread_setcanceltype(s->cancel_type, NULL), "set thread cancel type failed");

	AWALYS_LOOPS() {
		if (s->has_cancel_point) {
			/* print is also a cancel point */
			P("I am in thread %d, cancel state %s(cancel type %s)\n",
			  s->index,
			  s->cancel_state == PTHREAD_CANCEL_ENABLE ? "enable" : "disable",
			  s->cancel_type == PTHREAD_CANCEL_DEFERRED ? "deferred" : "asychronous");
			/* a api to check if thread has been cancelled */
			pthread_testcancel();
		} else {
			/*  no cancel point, thread can not be cancelled */
			n++;
		}
	}
}

int main(int argc, char *argv[])
{
	int i;

	for (i = 0; i < ARRAY_SIZE(settings); i++)
		settings[i].tid = NEW_THREAD(run, &settings[i]);

	sleep(1);

	for (i = 0; i < ARRAY_SIZE(settings); i++)
		GODIE_ON(pthread_cancel(settings[i].tid), "cancel thread %d failed", settings[i].index);

	AWALYS_LOOPS();

	return 0;
}

/* ls /proc/<pid>/task */
/* gcc thread_cancel.c -pthread -o thread_cancel */
