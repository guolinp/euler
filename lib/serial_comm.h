#ifndef __SERIAL_COMM_H__
#define __SERIAL_COMM_H__

/* A simple serial ipc wrapper function implemation */

#define SERIAL_RAW_MODE       0
#define SERIAL_NORMAL_MODE    1

#define CONFIG_SERIAL_MODE    SERIAL_RAW_MODE
#define CONFIG_SERIAL_SPEED   115200
#define CONFIG_SERIAL_BITS    8
#define CONFIG_SERIAL_PARITY  'N'
#define CONFIG_SERIAL_STOP    1

struct serial_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct serial_conn on success
 * return NULL on failed
 * */
struct serial_conn *serial_accept_connection(const char *pathname);

/* connect the server though pathname
 * return a ptr points to a struct serial_conn on success
 * return NULL on failed
 * */
struct serial_conn *serial_connect(const char *pathname);

/* send data
 * return data length sent on success
 * return -1 on failed
 * **/
int serial_send(struct serial_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int serial_recv(struct serial_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void serial_conn_free(struct serial_conn *conn);

/* get the file descriptor for a connection */
int serial_conn_fd(struct serial_conn *conn);

#endif
