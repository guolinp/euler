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
#include "common_socket_filter_interface.h"

static int common_handle_socket_filter(int attach, int sock, struct sock_filter *bpf_code, int nr)
{
	struct sock_fprog filter;
	int i, op;

	if (sock < 0 || bpf_code || nr <= 0) {
		dbg_printf("invalid inputs, sock %d bpf_code %p nr %d\n", sock, bpf_code, nr);
		return -EINVAL;
	}

	filter.len = nr;
	filter.filter = bpf_code;

	op = attach ? SO_ATTACH_FILTER : SO_DETACH_FILTER;

	if (setsockopt(sock, SOL_SOCKET, op, &filter, sizeof(filter)) < 0) {
		dbg_printf("%s the bpf filter failed, errno %d\n", op ? "attch" : "detach", errno);
		return -EIO;
	}

	trc_printf("the socket filter: \n");
	for (i = 0; i < nr; i++)
		trc_printf("{ 0x%02x, %d, %d, 0x%08x }\n", bpf_code[i].code, bpf_code[i].jt, bpf_code[i].jf, bpf_code[i].k);
	return 0;

}

int common_attach_socket_filter(int sock, struct sock_filter *bpf_code, int nr)
{
	return common_handle_socket_filter(1, sock, bpf_code, nr);
}

int common_detach_socket_filter(int sock, struct sock_filter *bpf_code, int nr)
{
	return common_handle_socket_filter(0, sock, bpf_code, nr);
}
