#ifndef __MSGQ_COMM_H__
#define __MSGQ_COMM_H__

/* A simple msgq ipc wrapper function implemation */

#define CONFIG_MQ_MSG_NR       128
#define CONFIG_MQ_MSG_SIZE     1024

struct msgq_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct msgq_conn on success
 * return NULL on failed
 * */
struct msgq_conn *msgq_accept_connection(const char *pathname);

/* connect the server though pathname
 * return a ptr points to a struct msgq_conn on success
 * return NULL on failed
 * */
struct msgq_conn *msgq_connect(const char *pathname);

/* send data
 * return  0 on success
 * return -1 on failed
 * **/
int msgq_send(struct msgq_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int msgq_recv(struct msgq_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void msgq_conn_free(struct msgq_conn *conn);

/* get the file descriptor for a connection */
int msgq_conn_fd(struct msgq_conn *conn);

#endif
