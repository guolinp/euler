#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "print.h"
#include "fifo_comm.h"

struct fifo_conn {
	int fd;
};

static struct fifo_conn *fifo_conn_new(void)
{
	struct fifo_conn *conn = malloc(sizeof(*conn));
	if (conn)
		conn->fd = -1;
	return conn;
}

void fifo_conn_free(struct fifo_conn *conn)
{
	if (conn && conn->fd > 0) {
		close(conn->fd);
		free(conn);
	}
}

int fifo_send(struct fifo_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->fd < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = write(conn->fd, buf, size);
	return ret;
}

int fifo_recv(struct fifo_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->fd < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = read(conn->fd, buf, size);
	return ret;
}

struct fifo_conn *fifo_accept_connection(const char *pathname)
{
	struct fifo_conn *conn;
	int fd;

	if (!pathname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = fifo_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	/* remove any preexisting other file */
	unlink(pathname);

	if ((mkfifo(pathname, S_IRWXU) < 0) && (errno != EEXIST)) {
		dbg_printf("cannot create fifo file %s, errno %d\n", pathname, errno);
		goto err_fifo;
	}

	fd = open(pathname, O_RDONLY);
	if (fd < 0) {
		dbg_printf("cannot open fifo file %s, errno %d\n", pathname, errno);
		goto err_fd;
	}

	conn->fd = fd;

	return conn;

  err_fd:
	close(fd);
  err_fifo:
	fifo_conn_free(conn);
  err_conn:
	return NULL;
}

struct fifo_conn *fifo_connect(const char *pathname)
{
	struct fifo_conn *conn;
	int fd;

	if (!pathname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = fifo_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	fd = open(pathname, O_WRONLY);
	if (fd < 0) {
		dbg_printf("cannot open fifo file %s, errno %d\n", pathname, errno);
		goto err_fd;
	}
	conn->fd = fd;

	return conn;

  err_fd:
	fifo_conn_free(conn);
  err_conn:
	return NULL;
}

int fifo_conn_fd(struct fifo_conn *conn)
{
	if (conn)
		return conn->fd;
	return -1;
}
