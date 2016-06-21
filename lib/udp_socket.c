#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "print.h"
#include "udp_socket.h"

struct udp_socket_conn {
	int sock;
	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
	unsigned int remote_addr_len;
};

static struct udp_socket_conn *udp_socket_conn_new(void)
{
	struct udp_socket_conn *conn = malloc(sizeof(*conn));
	if (conn) {
		memset(conn, sizeof(*conn), 0);
		conn->sock = -1;
	}
	return conn;
}

void udp_socket_conn_free(struct udp_socket_conn *conn)
{
	if (conn && conn->sock > 0)
		close(conn->sock);
}

int udp_socket_send(struct udp_socket_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->sock < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = sendto(conn->sock, buf, size, 0, (struct sockaddr *) &conn->remote_addr, sizeof(conn->remote_addr));
	return ret;
}

int udp_socket_recv(struct udp_socket_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->sock < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = recvfrom(conn->sock, buf, size, 0, (struct sockaddr *) &conn->remote_addr, &conn->remote_addr_len);
	return ret;
}

struct udp_socket_conn *udp_socket_accept_connection(const char *local_ip, unsigned short port)
{
	struct udp_socket_conn *conn;
	int s_sock;

	if (!local_ip) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = udp_socket_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	if ((s_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		dbg_printf("cannot create udp socket, errno %d\n", errno);
		goto err_sock;
	}

	conn->local_addr.sin_family = AF_INET;
	conn->local_addr.sin_addr.s_addr = inet_addr(local_ip);
	conn->local_addr.sin_port = htons(port);

	if (bind(s_sock, (struct sockaddr *) &conn->local_addr, sizeof(conn->local_addr))) {
		dbg_printf("cannot bind the address, errno %d\n", errno);
		goto err_sock;
	}

	conn->sock = s_sock;

	return conn;

  err_sock:
	close(s_sock);
	udp_socket_conn_free(conn);
  err_conn:
	return NULL;
}

struct udp_socket_conn *udp_socket_connect(const char *target_ip, unsigned short port)
{
	struct udp_socket_conn *conn;
	int c_sock;

	if (!target_ip) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = udp_socket_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	if ((c_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		dbg_printf("cannot create udp socket, errno %d\n", errno);
		goto err_sock;
	}

	conn->remote_addr.sin_family = AF_INET;
	conn->remote_addr.sin_addr.s_addr = inet_addr(target_ip);
	conn->remote_addr.sin_port = htons(port);

	conn->sock = c_sock;

	return conn;

  err_sock:
	close(c_sock);
	udp_socket_conn_free(conn);
  err_conn:
	return NULL;
}

int udp_socket_conn_fd(struct udp_socket_conn *conn)
{
	if (conn)
		return conn->sock;
	return -1;
}
