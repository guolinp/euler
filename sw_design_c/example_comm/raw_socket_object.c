#include "raw_socket.h"
#include "raw_socket_object.h"

#include "socket_filter_interface.h"
#include "common_socket_filter_interface.h"

static void *raw_conn_accept(const char *pathname, void *options)
{
	struct raw_socket_conn *raw_conn;
	raw_conn = raw_socket_accept_connection(pathname);
	return raw_conn;
}

static void *raw_conn_connect(const char *pathname, void *options)
{
	struct raw_socket_conn *raw_conn;
	raw_conn = raw_socket_connect(pathname);
	return raw_conn;
}

static int raw_conn_fd(void *conn)
{
	struct raw_socket_conn *raw_conn = conn;
	return raw_socket_conn_fd(raw_conn);
}

static void raw_conn_free(void *conn)
{
	struct raw_socket_conn *raw_conn = conn;
	return raw_socket_conn_free(raw_conn);
}

static int raw_socket_attach_filter(void *conn, struct sock_filter *bpf_code, int nr)
{
	struct raw_socket_conn *raw_conn = conn;
	int fd = raw_socket_conn_fd(conn);
	return common_attach_socket_filter(fd, bpf_code, nr);
}

static int raw_socket_detach_filter(void *conn, struct sock_filter *bpf_code, int nr)
{
	struct raw_socket_conn *raw_conn = conn;
	int fd = raw_socket_conn_fd(conn);
	return common_detach_socket_filter(fd, bpf_code, nr);

}

static struct conn_interface raw_conn_interface = {
	.conn_accept = raw_conn_accept,
	.conn_connect = raw_conn_connect,
	.conn_fd = raw_conn_fd,
	.conn_free = raw_conn_free,
};

static struct socket_filter_interface filter_interface = {
	.attach = raw_socket_attach_filter,
	.detach = raw_socket_detach_filter,
};

struct conn_interface *raw_socket_obj_interface()
{
	return &raw_conn_interface;
}

struct socket_filter_interface *raw_socket_filter_interface()
{
	return &filter_interface;
}
