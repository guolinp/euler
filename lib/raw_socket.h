#ifndef __RAW_SOCKET_H__
#define __RAW_SOCKET_H__

/* A simple Raw Domain Socket wrapper function implemation */

#include <linux/filter.h>

struct raw_socket_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct raw_socket_conn on success
 * return NULL on failed
 * */
struct raw_socket_conn *raw_socket_accept_connection(const char *itfname);

/* connect the server though path
 * return a ptr points to a struct raw_socket_conn on success
 * return NULL on failed
 * */
struct raw_socket_conn *raw_socket_connect(const char *itfname);

/* send data
 * return data length sent on success
 * return -1 on failed
 * **/
int raw_socket_send(struct raw_socket_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int raw_socket_recv(struct raw_socket_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void raw_socket_conn_free(struct raw_socket_conn *conn);

/* get the file descriptor for a connection */
int raw_socket_conn_fd(struct raw_socket_conn *conn);

/* attach the BPF socket filter  */
int raw_socket_conn_attach_filter(struct raw_socket_conn *conn, struct sock_filter *bpf_code, int nr);

#endif
