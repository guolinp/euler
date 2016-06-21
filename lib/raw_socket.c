#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/filter.h>

#include "print.h"
#include "raw_socket.h"

struct raw_socket_conn {
	int sock;
	struct sockaddr_ll addr;
};

static struct raw_socket_conn *raw_socket_conn_new(void)
{
	struct raw_socket_conn *conn = malloc(sizeof(*conn));
	if (conn) {
		memset(conn, 0, sizeof(*conn));
		conn->sock = -1;
	}
	return conn;
}

void raw_socket_conn_free(struct raw_socket_conn *conn)
{
	if (conn && conn->sock > 0)
		close(conn->sock);
}

int raw_socket_send(struct raw_socket_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->sock < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = write(conn->sock, buf, size);
	return ret;
}

int raw_socket_recv(struct raw_socket_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->sock < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = read(conn->sock, buf, size);
	return ret;
}

static struct raw_socket_conn *raw_socket_common_init(const char *itfname)
{
	struct raw_socket_conn *conn;
	static struct sockaddr_ll eth_dest_addr;
	int sock;

	if (!itfname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = raw_socket_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	eth_dest_addr.sll_family = PF_PACKET;
	eth_dest_addr.sll_protocol = htons(ETH_P_ALL);
	eth_dest_addr.sll_ifindex = if_nametoindex(itfname);
	eth_dest_addr.sll_hatype = ARPHRD_ETHER;
	eth_dest_addr.sll_pkttype = PACKET_OTHERHOST;
	eth_dest_addr.sll_halen = ETH_ALEN;

	sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock < 0) {
		dbg_printf("cannot create raw socket, errno %d\n", errno);
		goto err_sock;
	}

	if (bind(sock, (struct sockaddr *) &eth_dest_addr, sizeof(eth_dest_addr)) < 0) {
		dbg_printf("cannot bind raw socket, errno %d\n", errno);
		goto err_op;
	}

	conn->sock = sock;
	conn->addr = eth_dest_addr;

	return conn;

  err_op:
	close(sock);
  err_sock:
	raw_socket_conn_free(conn);
  err_conn:
	return NULL;
}

struct raw_socket_conn *raw_socket_accept_connection(const char *itfname)
{
	return raw_socket_common_init(itfname);
}

struct raw_socket_conn *raw_socket_connect(const char *itfname)
{
	return raw_socket_common_init(itfname);
}

int raw_socket_conn_fd(struct raw_socket_conn *conn)
{
	if (conn)
		return conn->sock;
	return -1;
}

int raw_socket_conn_attach_filter(struct raw_socket_conn *conn, struct sock_filter *bpf_code, int nr)
{
	struct sock_fprog filter;
	int i;

	if (!conn || !bpf_code || nr <= 0)
		return -EINVAL;

	filter.len = nr;
	filter.filter = bpf_code;

	if (setsockopt(conn->sock, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter)) < 0) {
		dbg_printf("attach the bpf filter failed, errno %d\n", errno);
		return -EINVAL;
	}

	trc_printf("the socket filter: \n");
	for (i = 0; i < nr; i++)
		trc_printf("{ 0x%02x, %d, %d, 0x%08x }\n", bpf_code[i].code, bpf_code[i].jt, bpf_code[i].jf, bpf_code[i].k);
	return 0;
}
