#ifndef __FIFO_COMM_H__
#define __FIFO_COMM_H__

/* A simple fifo ipc wrapper function implemation */

struct fifo_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct fifo_conn on success
 * return NULL on failed
 * */
struct fifo_conn *fifo_accept_connection(const char *pathname);

/* connect the server though pathname
 * return a ptr points to a struct fifo_conn on success
 * return NULL on failed
 * */
struct fifo_conn *fifo_connect(const char *pathname);

/* send data
 * return data length sent on success
 * return -1 on failed
 * **/
int fifo_send(struct fifo_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int fifo_recv(struct fifo_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void fifo_conn_free(struct fifo_conn *conn);

/* get the file descriptor for a connection */
int fifo_conn_fd(struct fifo_conn *conn);

#endif
