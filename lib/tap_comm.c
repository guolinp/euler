#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/if_arp.h>
#include <linux/if_tun.h>

#include "print.h"
#include "tap_comm.h"

struct tap_conn {
	int fd;
};

static struct tap_conn *tap_conn_new(void)
{
	struct tap_conn *conn = malloc(sizeof(*conn));
	if (conn) {
		conn->fd = -1;
	}
	return conn;
}

void tap_conn_free(struct tap_conn *conn)
{
	if (conn && conn->fd > 0) {
		close(conn->fd);
		free(conn);
	}
}

int tap_send(struct tap_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->fd < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = write(conn->fd, buf, size);
	return ret;
}

int tap_recv(struct tap_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->fd < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = read(conn->fd, buf, size);
	return ret;
}

static struct tap_conn *tap_common_init(const char *itfname)
{
	struct tap_conn *conn;
	struct ifreq ifr;
	int fd;

	if (!itfname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = tap_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
		dbg_printf("cannot open /dev/net/tun, errno %d\n", errno);
		goto err_conn;
	}

	memset(&ifr, 0, sizeof(ifr));

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	 *        IFF_TAP   - TAP device
	 *
	 *        IFF_NO_PI - Do not provide packet information
	 */
	ifr.ifr_flags = IFF_TUN;
	if (itfname)
		strncpy(ifr.ifr_name, itfname, IFNAMSIZ);

	if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
		dbg_printf("cannot set itfname %s, errno %d\n", itfname, errno);
		goto err_op;
	}

	conn->fd = fd;

	return conn;

  err_op:
	close(fd);
  err_fd:
	tap_conn_free(conn);
  err_conn:
	return NULL;
}

struct tap_conn *tap_accept_connection(const char *itfname)
{
	return tap_common_init(itfname);
}

struct tap_conn *tap_connect(const char *itfname)
{
	return tap_common_init(itfname);
}

int tap_conn_fd(struct tap_conn *conn)
{
	if (conn)
		return conn->fd;
	return -1;
}
