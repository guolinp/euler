#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

/* A simple TCP Domain Socket wrapper function implemation */

struct tcp_socket_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct tcp_socket_conn on success
 * return NULL on failed
 * */
struct tcp_socket_conn *tcp_socket_accept_connection(const char *local_ip, unsigned short port);

/* connect the server though path
 * return a ptr points to a struct tcp_socket_conn on success
 * return NULL on failed
 * */
struct tcp_socket_conn *tcp_socket_connect(const char *target_ip, unsigned short port);

/* send data
 * return data length sent on success
 * return -1 on failed
 * **/
int tcp_socket_send(struct tcp_socket_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int tcp_socket_recv(struct tcp_socket_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void tcp_socket_conn_free(struct tcp_socket_conn *conn);

/* get the file descriptor for a connection */
int tcp_socket_conn_fd(struct tcp_socket_conn *conn);

#endif
