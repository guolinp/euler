#ifndef __UNIX_SOCKET_H__
#define __UNIX_SOCKET_H__

/* A simple Unix Domain Socket wrapper function implemation */

struct unix_socket_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct unix_socket_conn on success
 * return NULL on failed
 * */
struct unix_socket_conn *unix_socket_accept_connection(const char *pathname);

/* connect the server though path
 * return a ptr points to a struct unix_socket_conn on success
 * return NULL on failed
 * */
struct unix_socket_conn *unix_socket_connect(const char *pathname);

/* send data
 * return data length sent on success
 * return -1 on failed
 * **/
int unix_socket_send(struct unix_socket_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int unix_socket_recv(struct unix_socket_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void unix_socket_conn_free(struct unix_socket_conn *conn);

/* get the file descriptor for a connection */
int unix_socket_conn_fd(struct unix_socket_conn *conn);

#endif
