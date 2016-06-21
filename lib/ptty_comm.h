#ifndef __PTTY_SOCKET_H__
#define __PTTY_SOCKET_H__

/* A simple ptty wrapper function implemation */

struct ptty_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct ptty_conn on success
 * return NULL on failed
 * */
struct ptty_conn *ptty_accept_connection(const char *pathname);

/* connect the server though path
 * return a ptr points to a struct ptty_conn on success
 * return NULL on failed
 * */
struct ptty_conn *ptty_connect(const char *pathname);

/* send data
 * return data length sent on success
 * return -1 on failed
 * **/
int ptty_send(struct ptty_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int ptty_recv(struct ptty_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void ptty_conn_free(struct ptty_conn *conn);

/* get the file descriptor for a connection */
int ptty_conn_fd(struct ptty_conn *conn);

#endif
