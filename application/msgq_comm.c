#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"
#include "msgq_comm.h"

static void run_server(const char *pathname)
{
	struct msgq_conn *conn;
	char buf[8192];
	int len;

	while (1) {
		conn = msgq_accept_connection(pathname);
		if (!conn) {
			dbg_printf("accept failed\n");
			sleep(1);
			continue;
		}

		while (1) {
			len = msgq_recv(conn, buf, 8192);
			if (len < 0) {
				dbg_printf("receive failed\n");
				msgq_conn_free(conn);
				break;
			}
			buf[len] = '\0';
			trc_printf("receive data: %s\n", buf);
		}
	}
}

static void run_client(const char *pathname)
{
	struct msgq_conn *conn;
	char *str = "this is a msgq ipc tx/tx test string\n";
	int len;

	conn = msgq_connect(pathname);
	if (!conn) {
		dbg_printf("connect failed\n");
		return;
	}

	len = msgq_send(conn, str, strlen(str));
	if (len < 0)
		dbg_printf("send failed\n");
	else
		trc_printf("send data: %s\n", str);

	msgq_conn_free(conn);
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
		fprintf(stderr, "Usage: %s <server|client> </msgq_file_pathname>\n", argv[0]);
	}

	return 0;
}
