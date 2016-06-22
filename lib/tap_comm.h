#ifndef __TAP_SOCKET_H__
#define __TAP_SOCKET_H__

/* A simple TAP wrapper function implemation */

struct tap_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct tap_conn on success
 * return NULL on failed
 * */
struct tap_conn *tap_accept_connection(const char *itfname);

/* connect the server though path
 * return a ptr points to a struct tap_conn on success
 * return NULL on failed
 * */
struct tap_conn *tap_connect(const char *itfname);

/* send data
 * return data length sent on success
 * return -1 on failed
 * **/
int tap_send(struct tap_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int tap_recv(struct tap_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void tap_conn_free(struct tap_conn *conn);

/* get the file descriptor for a connection */
int tap_conn_fd(struct tap_conn *conn);

#endif
