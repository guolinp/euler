#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "utility.h"

#define BUFF_SIZE           1024

#define END_MARK_STRING     "end"
#define END_MARK_LENGTH     strlen(END_MARK_STRING)

struct buffer {
	pthread_mutex_t lock;
	char *buff;
	int buff_size;
};

static inline int buff_has_data(struct buffer *buff)
{
	return buff->buff[0];
}

static inline void buff_reset(struct buffer *buff)
{
	buff->buff[0] = '\0';
}

static inline int should_exit_thread(char *cmd)
{
	return !strncmp(cmd, END_MARK_STRING, END_MARK_LENGTH);
}

static void *in_func(void *data)
{
	struct buffer *b = (struct buffer *) data;
	char *p;

	GODIE_ON(b == NULL, "invalid input");

	for (;;) {
		if (buff_has_data(b)) {
			usleep(1000);
			continue;
		}
		pthread_mutex_lock(&b->lock);
		printf("IN:");
		p = fgets(b->buff, b->buff_size, stdin);
		if (!p || should_exit_thread(b->buff)) {
			pthread_mutex_unlock(&b->lock);
			break;
		}
		pthread_mutex_unlock(&b->lock);
	}
	return 0;
}

static void *out_func(void *data)
{
	struct buffer *b = (struct buffer *) data;

	GODIE_ON(b == NULL, "invalid input");

	for (;;) {
		if (!buff_has_data(b)) {
			usleep(1000);
			continue;
		}
		pthread_mutex_lock(&b->lock);
		if (should_exit_thread(b->buff)) {
			pthread_mutex_unlock(&b->lock);
			break;
		}
		printf("OUT: %s", b->buff);
		buff_reset(b);
		pthread_mutex_unlock(&b->lock);
	}
	return 0;
}

void init_buffer(struct buffer **buf)
{
	struct buffer *b = MALLOC(sizeof(*b) + BUFF_SIZE);
	b->buff = (char *) &b[1];
	b->buff_size = BUFF_SIZE;
	GODIE_ON(pthread_mutex_init(&b->lock, NULL), "Init mutex lock failed");
	*buf = b;
}

void free_buffer(struct buffer *buf)
{
	pthread_mutex_destroy(&buf->lock);
	FREE(buf);
}

int main(int argc, char *argv[])
{
	struct buffer *buf;
	pthread_t in_tid;
	pthread_t out_tid;

	init_buffer(&buf);

	in_tid = NEW_THREAD(in_func, buf);
	out_tid = NEW_THREAD(out_func, buf);

	GODIE_ON(pthread_join(in_tid, NULL), "join 'in' thread failed");
	GODIE_ON(pthread_join(out_tid, NULL), "join 'out' thread failed");

	free_buffer(buf);

	return 0;
}

/* gcc thread_mutex.c -pthread -o thread_mutex */
