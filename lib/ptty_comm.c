#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pty.h>

#include "print.h"
#include "ptty_comm.h"

struct ptty_conn {
	int ptm_fd;
	int pts_fd;
};

static struct ptty_conn *ptty_conn_new(void)
{
	struct ptty_conn *conn = malloc(sizeof(*conn));
	if (conn) {
		conn->ptm_fd = -1;
		conn->pts_fd = -1;
	}
	return conn;
}

void ptty_conn_free(struct ptty_conn *conn)
{
	if (!conn)
		return;

	if (conn->ptm_fd > 0)
		close(conn->ptm_fd);

	if (conn->pts_fd > 0)
		close(conn->pts_fd);

	free(conn);
}

int ptty_send(struct ptty_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->ptm_fd < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = write(conn->ptm_fd, buf, size);
	return ret;
}

int ptty_recv(struct ptty_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->ptm_fd < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = read(conn->ptm_fd, buf, size);
	return ret;
}

struct ptty_conn *ptty_accept_connection(const char *pathname)
{
	struct ptty_conn *conn;
	int pts_fd, ptm_fd;
	char *pts_name = NULL;

	if (!pathname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = ptty_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	ptm_fd = open("/dev/ptmx", O_RDWR);
	if (ptm_fd < 0) {
		dbg_printf("cannot open file /dev/ptmx, errno %d\n", errno);
		goto err_ptm_fd;
	}

	pts_name = (char *) ptsname(ptm_fd);
	if (NULL == pts_name) {
		dbg_printf("cannot open get pts name, errno %d\n", errno);
		goto err_pts;
	}

	grantpt(ptm_fd);
	unlockpt(ptm_fd);

	pts_fd = open(pts_name, O_RDWR);
	if (pts_fd < 0) {
		dbg_printf("cannot open file %s, errno %d\n", pts_name, errno);
		goto err_pts;
	}

	/* tell fd which process to signal */
	if (fcntl(ptm_fd, F_SETOWN, getpid()) < 0) {
		dbg_printf("cannot set own, errno %d\n", errno);
		goto err_setown;
	}

	unlink(pathname);
	if (symlink(pts_name, pathname) < 0) {
		dbg_printf("cannot make symlink, %s -> %serrno %d\n", pathname, pts_name, errno);
		goto err_symlink;
	}

	trc_printf("symlink %s -> %s\n", pathname, pts_name);

	conn->ptm_fd = ptm_fd;
	conn->pts_fd = pts_fd;

	return conn;

  err_symlink:
  err_setown:
	close(pts_fd);
  err_pts:
	close(ptm_fd);
  err_ptm_fd:
	ptty_conn_free(conn);
  err_conn:
	return NULL;
}

struct ptty_conn *ptty_connect(const char *pathname)
{
	struct ptty_conn *conn;
	int ptm_fd;

	if (!pathname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = ptty_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	ptm_fd = open(pathname, O_RDWR);
	if (ptm_fd < 0) {
		dbg_printf("cannot open file %s, errno %d\n", pathname, errno);
		goto err_ptm_fd;
	}

	conn->ptm_fd = ptm_fd;

	return conn;

  err_ptm_fd:
	ptty_conn_free(conn);
  err_conn:
	return NULL;
}

int ptty_conn_fd(struct ptty_conn *conn)
{
	if (conn)
		return conn->ptm_fd;
	return -1;
}
