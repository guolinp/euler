#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "print.h"
#include "tcp_socket.h"

struct tcp_socket_conn {
	int c_sock;
	int s_sock;
};

static struct tcp_socket_conn *tcp_socket_conn_new(void)
{
	struct tcp_socket_conn *conn = malloc(sizeof(*conn));
	if (conn) {
		conn->c_sock = -1;
		conn->s_sock = -1;
	}
	return conn;
}

void tcp_socket_conn_free(struct tcp_socket_conn *conn)
{
	if (!conn)
		return;

	if (conn->c_sock > 0)
		close(conn->c_sock);

	if (conn->s_sock > 0)
		close(conn->s_sock);

	free(conn);
}

int tcp_socket_send(struct tcp_socket_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->c_sock < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = send(conn->c_sock, buf, size, 0);
	return ret;
}

int tcp_socket_recv(struct tcp_socket_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->c_sock < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = recv(conn->c_sock, buf, size, 0);
	return ret;
}

struct tcp_socket_conn *tcp_socket_accept_connection(const char *local_ip, unsigned short port)
{
	struct tcp_socket_conn *conn;
	struct sockaddr_in host_addr;
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	int s_sock;
	int c_sock;

	if (!local_ip) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = tcp_socket_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	memset((void *) &host_addr, 0, sizeof(struct sockaddr_in));
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(port);
	host_addr.sin_addr.s_addr = inet_addr(local_ip);

	if ((s_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		dbg_printf("cannot create tcp socket, errno %d\n", errno);
		goto err_sock;
	}

	if (bind(s_sock, (struct sockaddr *) &host_addr, sizeof(host_addr))) {
		dbg_printf("cannot bind the address, errno %d\n", errno);
		goto err_op;
	}

	if (listen(s_sock, 1)) {
		dbg_printf("cannot listen connection, errno %d\n", errno);
		goto err_op;
	}

	c_sock = accept(s_sock, (struct sockaddr *) &client_addr, &client_addr_len);
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
	tcp_socket_conn_free(conn);
  err_conn:
	return NULL;
}

struct tcp_socket_conn *tcp_socket_connect(const char *target_ip, unsigned short port)
{
	struct tcp_socket_conn *conn;
	struct sockaddr_in server_addr;
	int c_sock;

	if (!target_ip) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = tcp_socket_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		dbg_printf("cannot create tcp socket, errno %d\n", errno);
		goto err_sock;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_pton(AF_INET, target_ip, &server_addr.sin_addr);

	if (connect(c_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		dbg_printf("accept a new connection failed, errno %d\n", errno);
		goto err_sock;
	}

	conn->c_sock = c_sock;

	return conn;

  err_sock:
	close(c_sock);
	tcp_socket_conn_free(conn);
  err_conn:
	return NULL;
}

int tcp_socket_conn_fd(struct tcp_socket_conn *conn)
{
	if (conn)
		return conn->c_sock;
	return -1;
}
