#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"
#include "tcp_socket.h"

static void run_server(const char *local_ip, unsigned short port)
{
	struct tcp_socket_conn *conn;
	char buf[128];
	int len;

	while (1) {
		conn = tcp_socket_accept_connection(local_ip, port);
		if (!conn) {
			dbg_printf("accept failed\n");
			continue;
		}

		while (1) {
			len = tcp_socket_recv(conn, buf, 127);
			if (len <= 0) {
				dbg_printf("receive failed\n");
				tcp_socket_conn_free(conn);
				break;
			}
			buf[len] = '\0';
			trc_printf("receive data: %s\n", buf);
		}
	}
}

static void run_client(const char *target_ip, unsigned short port)
{
	struct tcp_socket_conn *conn;
	char *str = "this is a tcp socket tx/tx test string\n";
	int len;

	conn = tcp_socket_connect(target_ip, port);
	if (!conn) {
		dbg_printf("connect failed\n");
		return;
	}

	len = tcp_socket_send(conn, str, strlen(str));
	if (len <= 0)
		dbg_printf("send failed\n");
	else
		trc_printf("send data: %s\n", str);

	tcp_socket_conn_free(conn);
}

#define IP   "127.0.0.1"
#define PORT 5678
int main(int argc, char *argv[])
{
	if (argc >= 2) {
		if (strcmp(argv[1], "server") == 0)
			run_server(IP, PORT);
		else if (strcmp(argv[1], "client") == 0)
			run_client(IP, PORT);
		else
			fprintf(stderr, "bad input\n");
	} else {
		fprintf(stderr, "Usage: %s <server|client>\n", argv[0]);
	}

	return 0;
}
