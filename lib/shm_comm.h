#ifndef __SHM_COMM_H__
#define __SHM_COMM_H__

/* A simple share memory wrapper function implemation */

struct shm_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct shm_conn on success
 * return NULL on failed
 * */
struct shm_conn *shm_accept_connection(const char *pathname, int size);

/* connect the server though pathname
 * return a ptr points to a struct shm_conn on success
 * return NULL on failed
 * */
struct shm_conn *shm_connect(const char *pathname, int size);

/* send data
 * return data length sent on success
 * return -1 on failed
 * **/
int shm_send(struct shm_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int shm_recv(struct shm_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void shm_conn_free(struct shm_conn *conn);

/* get the file descriptor for a connection */
int shm_conn_fd(struct shm_conn *conn);

#endif
