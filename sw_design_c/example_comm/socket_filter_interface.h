#ifndef __SOCKET_FILTER_INTERFACE_H__
#define __SOCKET_FILTER_INTERFACE_H__

#include <linux/filter.h>

struct socket_filter_interface {
	int (*attach) (void *conn, struct sock_filter * bpf_code, int nr);
	int (*detach) (void *conn, struct sock_filter * bpf_code, int nr);
};

int attach_socket_filter(struct socket_filter_interface *inerface, void *conn, struct sock_filter *bpf_code, int nr);
int detach_socket_filter(struct socket_filter_interface *inerface, void *conn, struct sock_filter *bpf_code, int nr);

#endif
