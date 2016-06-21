#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

/* A simple UDP Domain Socket wrapper function implemation */

struct udp_socket_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct udp_socket_conn on success
 * return NULL on failed
 * */
struct udp_socket_conn *udp_socket_accept_connection(const char *local_ip, unsigned short port);

/* connect the server though path
 * return a ptr points to a struct udp_socket_conn on success
 * return NULL on failed
 * */
struct udp_socket_conn *udp_socket_connect(const char *target_ip, unsigned short port);

/* send data
 * return data length sent on success
 * return -1 on failed
 * **/
int udp_socket_send(struct udp_socket_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int udp_socket_recv(struct udp_socket_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void udp_socket_conn_free(struct udp_socket_conn *conn);

/* get the file descriptor for a connection */
int udp_socket_conn_fd(struct udp_socket_conn *conn);

#endif
