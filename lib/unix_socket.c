#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "print.h"
#include "unix_socket.h"

struct unix_socket_conn {
	int c_sock;
	int s_sock;
};

static struct unix_socket_conn *unix_socket_conn_new(void)
{
	struct unix_socket_conn *conn = malloc(sizeof(*conn));
	if (conn) {
		conn->c_sock = -1;
		conn->s_sock = -1;
	}
	return conn;
}

void unix_socket_conn_free(struct unix_socket_conn *conn)
{
	if (!conn)
		return;

	if (conn->c_sock > 0)
		close(conn->c_sock);

	if (conn->s_sock > 0)
		close(conn->s_sock);

	free(conn);
}

int unix_socket_send(struct unix_socket_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->c_sock < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = write(conn->c_sock, buf, size);
	return ret;
}

int unix_socket_recv(struct unix_socket_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->c_sock < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = read(conn->c_sock, buf, size);
	return ret;
}

struct unix_socket_conn *unix_socket_accept_connection(const char *pathname)
{
	struct unix_socket_conn *conn;
	struct sockaddr_un address;
	int s_sock, c_sock;
	socklen_t addr_len;

	if (!pathname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = unix_socket_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	if ((s_sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
		dbg_printf("cannot create unix socket, errno %d\n", errno);
		goto err_sock;
	}

	/* Remove any preexisting socket (or other file) */
	unlink(pathname);

	address.sun_family = AF_UNIX;	/* Unix domain socket */
	strcpy(address.sun_path, pathname);

	/* The total length of the address includes the sun_family element */
	addr_len = sizeof(address.sun_family) + strlen(address.sun_path);

	if (bind(s_sock, (struct sockaddr *) &address, addr_len)) {
		dbg_printf("cannot bind the address, errno %d\n", errno);
		goto err_op;
	}

	if (listen(s_sock, 1)) {
		dbg_printf("cannot listen connection, errno %d\n", errno);
		goto err_op;
	}

	c_sock = accept(s_sock, (struct sockaddr *) &address, &addr_len);
	if (c_sock < 0) {
		dbg_printf("accept a new connection failed, errno %d\n", errno);
		goto err_op;
	}

	conn->c_sock = c_sock;
	conn->s_sock = s_sock;

	return conn;

  err_op:
	close(s_sock);
  err_sock:
	unix_socket_conn_free(conn);
  err_conn:
	return NULL;
}

struct unix_socket_conn *unix_socket_connect(const char *pathname)
{
	struct unix_socket_conn *conn;
	struct sockaddr_un address;
	int c_sock;
	size_t addr_len;

	if (!pathname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = unix_socket_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	if ((c_sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
		dbg_printf("cannot create unix socket, errno %d\n", errno);
		goto err_sock;
	}

	address.sun_family = AF_UNIX;	/* Unix domain socket */
	strcpy(address.sun_path, pathname);

	/* The total length of the address includes the sun_family element */
	addr_len = sizeof(address.sun_family) + strlen(address.sun_path);

	if (connect(c_sock, (struct sockaddr *) &address, addr_len) < 0) {
		dbg_printf("accept a new connection failed, errno %d\n", errno);
		goto err_op;
	}

	conn->c_sock = c_sock;

	return conn;

  err_op:
	close(c_sock);
  err_sock:
	unix_socket_conn_free(conn);
  err_conn:
	return NULL;
}

int unix_socket_conn_fd(struct unix_socket_conn *conn)
{
	if (conn)
		return conn->c_sock;
	return -1;
}
