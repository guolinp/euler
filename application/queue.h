#ifndef __QUEUE_H__
#define __QUEUE_H__

struct queue;

int q_create(struct queue **q, unsigned int size);
int q_enqueue(struct queue *q, unsigned int data);
int q_dequeue(struct queue *q, unsigned int *data);
int q_is_full(struct queue *q);
int q_is_empty(struct queue *q);
void q_destory(struct queue *q);

#endif
