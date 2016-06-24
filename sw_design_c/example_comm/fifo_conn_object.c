#include "fifo_comm.h"
#include "fifo_conn_object.h"

static void *fifo_accept(const char *pathname, void *options)
{
	struct fifo_conn *fifo_conn;
	fifo_conn = fifo_conn_accept_connection(pathname);
	return fifo_conn;
}

static void *fifo_connect(const char *pathname, void *options)
{
	struct fifo_conn *fifo_conn;
	fifo_conn = fifo_conn_connect(pathname);
	return fifo_conn;
}

static int fifo_fd(void *conn)
{
	struct fifo_conn *fifo_conn = conn;
	return fifo_conn_fd(fifo_conn);
}

static void fifo_free(void *conn)
{
	struct fifo_conn *fifo_conn = conn;
	return fifo_conn_free(fifo_conn);
}

static struct conn_interface fifo_conn_interface = {
	.conn_accept = fifo_accept,
	.conn_connect = fifo_connect,
	.conn_fd = fifo_fd,
	.conn_free = fifo_free,
};

struct conn_interface *fifo_obj_interface()
{
	return &fifo_conn_interface;
}
