#ifndef __PCAP_SOCKET_H__
#define __PCAP_SOCKET_H__

/* A simple libpcap wrapper function implemation */

#include <linux/filter.h>

#define CONFIG_MAX_PACKET_SIZE     1600
#define CONFIG_PROMISC_ENABLED     0
#define CONFIG_TIMEOUT             1000

struct pcap_conn;

/* build a server and wait a client connection
 * return a ptr points to a struct pcap_conn on success
 * return NULL on failed
 * */
struct pcap_conn *pcap_accept_connection(const char *itfname);

/* connect the server though path
 * return a ptr points to a struct pcap_conn on success
 * return NULL on failed
 * */
struct pcap_conn *pcap_connect(const char *itfname);

/* send data
 * return data length sent on success
 * return -1 on failed
 * **/
int pcap_send(struct pcap_conn *conn, char *buf, int size);

/* receive data
 * return data length received on success
 * return -1 on failed
 * */
int pcap_recv(struct pcap_conn *conn, char *buf, int size);

/* free a connection resource
 * */
void pcap_conn_free(struct pcap_conn *conn);

/* get the file descriptor for a connection */
int pcap_conn_fd(struct pcap_conn *conn);

/* attach the BPF socket filter  */
int pcap_conn_attach_filter(struct pcap_conn *conn, struct sock_filter *bpf_code, int nr);

#endif
