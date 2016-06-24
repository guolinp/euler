#ifndef __COMMON_SOCKET_FILTER_INTERFACE_H__
#define __COMMON_SOCKET_FILTER_INTERFACE_H__

int common_attach_socket_filter(int sock, struct sock_filter *bpf_code, int nr);
int common_detach_socket_filter(int sock, struct sock_filter *bpf_code, int nr);

#endif
