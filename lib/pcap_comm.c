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

#include <pcap/pcap.h>

#include "print.h"
#include "pcap_comm.h"

struct pcap_conn {
	pcap_t *desc;
};

static int wait_resource_available(int fd)
{
	int ret;
	fd_set rfds;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	ret = select(fd + 1, &rfds, NULL, NULL, NULL);
	return ret;
}

static struct pcap_conn *pcap_conn_new(void)
{
	struct pcap_conn *conn = malloc(sizeof(*conn));
	if (conn) {
		memset(conn, 0, sizeof(*conn));
	}
	return conn;
}

void pcap_conn_free(struct pcap_conn *conn)
{
	if (!conn)
		return;
	if (conn->desc)
		pcap_close(conn->desc);
	free(conn);
}

int pcap_send(struct pcap_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || !conn->desc || !buf || size <= 0)
		return -EINVAL;
	ret = pcap_inject(conn->desc, buf, size);
	return ret;
}

int pcap_recv(struct pcap_conn *conn, char *buf, int size)
{
	struct pcap_pkthdr header;
	const unsigned char *packet;
	int fd;
	int ret;

	if (!conn || !conn->desc || !buf || size <= 0)
		return -EINVAL;

#if 0
	fd = pcap_conn_fd(conn);
	ret = read(fd, buf, size);
	return ret;
#else
	fd = pcap_conn_fd(conn);
	if (wait_resource_available(fd) <= 0)
		return 0;

	packet = pcap_next(conn->desc, &header);
	if (!packet)
		return 0;

	if (size > header.len)
		size = header.len;
	memcpy(buf, packet, size);
	return size;
#endif
}

static struct pcap_conn *pcap_common_init(const char *itfname)
{
	struct pcap_conn *conn;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *desc = NULL;

	int maxsize = CONFIG_MAX_PACKET_SIZE;
	int timeout = CONFIG_TIMEOUT;
	int promisc = CONFIG_PROMISC_ENABLED;

	if (!itfname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = pcap_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	memset(errbuf, 0, PCAP_ERRBUF_SIZE);
	desc = pcap_open_live(itfname, maxsize, promisc, timeout, errbuf);
	if (!desc) {
		dbg_printf("cannot open device %s, errno %d, errstr %s\n", itfname, errno, errbuf);
		goto err_desc;
	}

	conn->desc = desc;

	return conn;

  err_desc:
	pcap_conn_free(conn);
  err_conn:
	return NULL;
}

struct pcap_conn *pcap_accept_connection(const char *itfname)
{
	return pcap_common_init(itfname);
}

struct pcap_conn *pcap_connect(const char *itfname)
{
	return pcap_common_init(itfname);
}

int pcap_conn_fd(struct pcap_conn *conn)
{
	if (conn)
		return pcap_fileno(conn->desc);
	return -1;
}

int pcap_conn_attach_filter(struct pcap_conn *conn, struct sock_filter *bpf_code, int nr)
{
	struct sock_fprog filter;
	int i;
	int fd = pcap_fileno(conn->desc);

	if (!conn || !bpf_code || nr <= 0)
		return -EINVAL;

	filter.len = nr;
	filter.filter = bpf_code;

	if (setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter)) < 0) {
		dbg_printf("attach the bpf filter failed, errno %d\n", errno);
		return -EINVAL;
	}

	trc_printf("the socket filter: \n");
	for (i = 0; i < nr; i++)
		trc_printf("{ 0x%02x, %d, %d, 0x%08x }\n", bpf_code[i].code, bpf_code[i].jt, bpf_code[i].jf, bpf_code[i].k);
	return 0;
}
