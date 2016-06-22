#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "print.h"
#include "msgq_comm.h"

struct msgq_conn {
	int msgq_id;
};

static struct msgq_conn *msgq_conn_new(void)
{
	struct msgq_conn *conn = malloc(sizeof(*conn));
	if (conn)
		conn->msgq_id = -1;
	return conn;
}

void msgq_conn_free(struct msgq_conn *conn)
{
	if (conn && conn->msgq_id > 0) {
		mq_close(conn->msgq_id);
		free(conn);
	}
}

int msgq_send(struct msgq_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->msgq_id < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = mq_send(conn->msgq_id, buf, size, 0);
	return ret;
}

int msgq_recv(struct msgq_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->msgq_id < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = mq_receive(conn->msgq_id, buf, size, NULL);
	return ret;
}

struct msgq_conn *msgq_accept_connection(const char *pathname)
{
	struct msgq_conn *conn;
	int msgq_id;
	struct mq_attr attr;

	if (!pathname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = msgq_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	/* remove any preexisting other file */
	mq_unlink(pathname);

	attr.mq_maxmsg = CONFIG_MQ_MSG_NR;
	attr.mq_msgsize = CONFIG_MQ_MSG_SIZE;
	msgq_id = mq_open(pathname, O_RDWR | O_CREAT, 0666, &attr);
	if (msgq_id < 0) {
		dbg_printf("cannot create msgq file %s, errno %d\n", pathname, errno);
		goto err_msgq;
	}

	conn->msgq_id = msgq_id;

	return conn;

  err_msgq:
	msgq_conn_free(conn);
  err_conn:
	return NULL;
}

struct msgq_conn *msgq_connect(const char *pathname)
{
	struct msgq_conn *conn;
	int msgq_id;

	if (!pathname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = msgq_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	msgq_id = mq_open(pathname, O_RDWR);
	if (msgq_id < 0) {
		dbg_printf("cannot open msgq file %s, errno %d\n", pathname, errno);
		goto err_msgq;
	}

	conn->msgq_id = msgq_id;

	return conn;

  err_msgq:
	msgq_conn_free(conn);
  err_conn:
	return NULL;
}

int msgq_conn_fd(struct msgq_conn *conn)
{
	if (conn)
		return conn->msgq_id;
	return -1;
}
