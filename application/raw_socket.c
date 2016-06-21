#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"
#include "raw_socket.h"

static void run_server(const char *path)
{
	struct raw_socket_conn *conn;
	char buf[128];
	int len;

	while (1) {
		conn = raw_socket_accept_connection(path);
		if (!conn) {
			dbg_printf("accept failed\n");
			continue;
		}

		while (1) {
			len = raw_socket_recv(conn, buf, 127);
			if (len <= 0) {
				dbg_printf("receive failed\n");
				raw_socket_conn_free(conn);
				break;
			}
			buf[len] = '\0';
			trc_printf("receive data: %s\n", buf);
		}
	}
}

static void run_client(const char *path)
{
	struct raw_socket_conn *conn;
	char *str = "this is a raw socket tx/tx test string\n";
	int len;

	conn = raw_socket_connect(path);
	if (!conn) {
		dbg_printf("connect failed\n");
		return;
	}

	len = raw_socket_send(conn, str, strlen(str));
	if (len <= 0)
		dbg_printf("send failed\n");
	else
		trc_printf("send data: %s\n", str);

	raw_socket_conn_free(conn);
}

int main(int argc, char *argv[])
{
	if (argc >= 3) {
		if (strcmp(argv[1], "server") == 0)
			run_server(argv[2]);
		else if (strcmp(argv[1], "client") == 0)
			run_client(argv[2]);
		else
			fprintf(stderr, "bad input\n");
	} else {
		fprintf(stderr, "Usage: %s <server|client> <itfname>\n", argv[0]);
	}

	return 0;
}
