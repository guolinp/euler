#include <stdio.h>
#include <sched.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

static void show_thread_policy(pthread_attr_t * attr)
{
	int policy = 0;
	int rs = pthread_attr_getschedpolicy(attr, &policy);

	assert(rs == 0);

	switch (policy) {
	case SCHED_FIFO:
		printf("policy = SCHED_FIFO(%d)\n", SCHED_FIFO);
		break;

	case SCHED_RR:
		printf("policy = SCHED_RR(%d)\n", SCHED_RR);
		break;

	case SCHED_OTHER:
		printf("policy = SCHED_OTHER(%d)\n", SCHED_OTHER);
		break;

	default:
		printf("policy = UNKNOWN(%d)\n", policy);
		break;
	}
}

static void show_thread_priority_limit(pthread_attr_t * attr, int policy)
{
	int priority;

	priority = sched_get_priority_max(policy);
	assert(priority != -1);
	printf("max_priority = %d\n", priority);

	priority = sched_get_priority_min(policy);
	assert(priority != -1);
	printf("min_priority = %d\n", priority);
}

static void show_thread_priority(pthread_attr_t * attr)
{
	struct sched_param param;

	int rs = pthread_attr_getschedparam(attr, &param);
	assert(rs == 0);
	printf("priority = %d\n", param.__sched_priority);

}

static void set_thread_policy(pthread_attr_t * attr, int policy)
{
	int rs = pthread_attr_setschedpolicy(attr, policy);
	assert(rs == 0);
}

int main(void)
{
	pthread_attr_t attr;
	struct sched_param sched;
	int rs;
	int policy = SCHED_RR;

	rs = pthread_attr_init(&attr);
	assert(rs == 0);

	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

	printf("Set thread policy = %d\n", policy);
	set_thread_policy(&attr, policy);

	sched.sched_priority = 50;
	pthread_attr_setschedparam(&attr, &sched);

	printf("Show thread policy\n");
	show_thread_policy(&attr);

	printf("Show current configuration of priority\n");
	show_thread_priority_limit(&attr, policy);

	printf("Show current priority\n");
	show_thread_priority(&attr);

	rs = pthread_attr_destroy(&attr);
	assert(rs == 0);

	return 0;
}

/* gcc thread_attr.c -pthread -o thread_attr */
