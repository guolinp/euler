#ifndef __CONN_INTERFACE_H__
#define __CONN_INTERFACE_H__

struct conn_interface {
	void *(*conn_accept) (const char *pathname, void *options);
	void *(*conn_connect) (const char *pathname, void *options);
	int (*conn_send) (void *conn, char *buf, int size);
	int (*conn_recv) (void *conn, char *buf, int size);
	int (*conn_fd) (void *conn);
	void (*conn_free) (void *conn);
};

#endif
