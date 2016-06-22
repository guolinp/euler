#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "print.h"
#include "hexdump.h"
#include "pcap_comm.h"

static void run_server(const char *path)
{
	struct pcap_conn *conn;
	char buf[1024];
	int len;

	while (1) {
		conn = pcap_accept_connection(path);
		if (!conn) {
			dbg_printf("accept failed\n");
			continue;
		}

		while (1) {
			len = pcap_recv(conn, buf, 1024);
			if (len < 0) {
				dbg_printf("receive failed, len %d, errno %d\n", len, errno);
				pcap_conn_free(conn);
				break;
			} else if (len > 0) {
				trc_printf("receive data: \n");
				hexdump(buf, len);
			} else {
				trc_printf("no data\n");
			}
		}
	}
}

static void run_client(const char *path)
{
	struct pcap_conn *conn;
	char *str = "this is a libpcap tx/tx test string\n";
	int len;

	conn = pcap_connect(path);
	if (!conn) {
		dbg_printf("connect failed\n");
		return;
	}

	len = pcap_send(conn, str, strlen(str));
	if (len <= 0)
		dbg_printf("send failed\n");
	else
		trc_printf("send data: %s\n", str);

	pcap_conn_free(conn);
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
