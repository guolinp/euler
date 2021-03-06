#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"
#include "fifo_comm.h"

static void run_server(const char *pathname)
{
	struct fifo_conn *conn;
	char buf[128];
	int len;

	while (1) {
		conn = fifo_conn_accept_connection(pathname);
		if (!conn) {
			dbg_printf("accept failed\n");
			continue;
		}

		while (1) {
			len = fifo_conn_recv(conn, buf, 127);
			if (len <= 0) {
				dbg_printf("receive failed\n");
				fifo_conn_free(conn);
				break;
			}
			buf[len] = '\0';
			trc_printf("receive data: %s\n", buf);
		}
	}
}

static void run_client(const char *pathname)
{
	struct fifo_conn *conn;
	char *str = "this is a fifo ipc tx/tx test string\n";
	int len;

	conn = fifo_conn_connect(pathname);
	if (!conn) {
		dbg_printf("connect failed\n");
		return;
	}

	len = fifo_conn_send(conn, str, strlen(str));
	if (len <= 0)
		dbg_printf("send failed\n");
	else
		trc_printf("send data: %s\n", str);

	fifo_conn_free(conn);
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
		fprintf(stderr, "Usage: %s <server|client> <fifo_file_pathname>\n", argv[0]);
	}

	return 0;
}
