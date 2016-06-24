#include <errno.h>
#include <linux/filter.h>

#include "socket_filter_interface.h"

int attach_socket_filter(struct socket_filter_interface *inerface, void *obj, struct sock_filter *bpf_code, int nr)
{
	if (inerface && inerface->attach)
		return inerface->attach(obj, bpf_code, nr);
	return -EINVAL;
}

int detach_socket_filter(struct socket_filter_interface *inerface, void *obj, struct sock_filter *bpf_code, int nr)
{
	if (inerface && inerface->detach)
		return inerface->detach(obj, bpf_code, nr);
	return -EINVAL;
}
