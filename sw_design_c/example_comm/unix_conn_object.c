#include "unix_socket.h"
#include "unix_conn_object.h"

static void *unix_conn_accept(const char *pathname, void *options)
{
	struct unix_socket_conn *unix_conn;
	unix_conn = unix_socket_accept_connection(pathname);
	return unix_conn;
}

static void *unix_conn_connect(const char *pathname, void *options)
{
	struct unix_socket_conn *unix_conn;
	unix_conn = unix_socket_connect(pathname);
	return unix_conn;
}

static int unix_conn_fd(void *conn)
{
	struct unix_socket_conn *unix_conn = conn;
	return unix_socket_conn_fd(unix_conn);
}

static void unix_conn_free(void *conn)
{
	struct unix_socket_conn *unix_conn = conn;
	return unix_socket_conn_free(unix_conn);
}

static struct conn_interface unix_conn_interface = {
	.conn_accept = unix_conn_accept,
	.conn_connect = unix_conn_connect,
	.conn_fd = unix_conn_fd,
	.conn_free = unix_conn_free,
};

struct conn_interface *unix_obj_interface()
{
	return &unix_conn_interface;
}
