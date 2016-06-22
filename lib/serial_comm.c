#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pty.h>
#include <termios.h>
#include <sys/stat.h>

#include "print.h"
#include "serial_comm.h"

struct serial_conn {
	int fd;
};

static struct serial_conn *serial_conn_new(void)
{
	struct serial_conn *conn = malloc(sizeof(*conn));
	if (conn)
		conn->fd = -1;
	return conn;
}

void serial_conn_free(struct serial_conn *conn)
{
	if (conn && conn->fd > 0) {
		close(conn->fd);
		free(conn);
	}
}

int serial_send(struct serial_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->fd < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = write(conn->fd, buf, size);
	return ret;
}

int serial_recv(struct serial_conn *conn, char *buf, int size)
{
	int ret;
	if (!conn || conn->fd < 0 || !buf || size <= 0)
		return -EINVAL;
	ret = read(conn->fd, buf, size);
	return ret;
}

static struct serial_conn *serial_common_init(const char *pathname)
{
	struct serial_conn *conn;
	struct termios newtio, oldtio;
	int fd;

	if (!pathname) {
		dbg_printf("invalid input\n");
		goto err_conn;
	}

	conn = serial_conn_new();
	if (!conn) {
		dbg_printf("cannot alloc connection resource\n");
		goto err_conn;
	}

	fd = open(pathname, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		dbg_printf("cannot open serial port %s, errno %d\n", pathname, errno);
		goto err_fd;
	}

	/* set serial to block status wait for read from serial port */
	if (fcntl(fd, F_SETFL, 0) < 0) {
		dbg_printf("cannot set serial port %s, errno %d\n", pathname, errno);
		goto err_op;
	}

	if (tcgetattr(fd, &oldtio) < 0) {
		dbg_printf("tcgetattr failed, errno %d\n", errno);
		goto err_op;
	}

	memset(&newtio, 0, sizeof(struct termios));
	memcpy(&newtio, &oldtio, sizeof(struct termios));

	if (CONFIG_SERIAL_MODE == SERIAL_RAW_MODE) {
		/* Set raw mode: the remote application will handle all terminal characters */
		cfmakeraw(&newtio);
	}

	switch (CONFIG_SERIAL_BITS) {
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}

	switch (CONFIG_SERIAL_PARITY) {
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E':
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':
		newtio.c_cflag &= ~PARENB;
		break;
	}

	switch (CONFIG_SERIAL_SPEED) {
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}

	switch (CONFIG_SERIAL_STOP) {
	case 1:
		newtio.c_cflag &= ~CSTOPB;
		break;
	case 2:
		newtio.c_cflag |= CSTOPB;
		break;
	default:
		newtio.c_cflag &= ~CSTOPB;
		break;
	}

	/*
	 * newtio.c_cflag |= CLOCAL | CREAD;
	 * newtio.c_cflag &= ~CSIZE;
	 * newtio.c_cc[VTIME] = 0;
	 * newtio.c_cc[VMIN] = 0;
	 */
	if (CONFIG_SERIAL_MODE == SERIAL_RAW_MODE) {
		newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		newtio.c_oflag &= ~OPOST;
		newtio.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	}

	tcflush(fd, TCIFLUSH);

	if (tcsetattr(fd, TCSANOW, &newtio) < 0) {
		dbg_printf("tcsetattr failed, errno %d\n", errno);
		goto err_op;
	}

	conn->fd = fd;

	return conn;

  err_op:
	close(fd);
  err_fd:
	serial_conn_free(conn);
  err_conn:
	return NULL;
}

struct serial_conn *serial_accept_connection(const char *pathname)
{
	return serial_common_init(pathname);
}

struct serial_conn *serial_connect(const char *pathname)
{
	return serial_common_init(pathname);
}

int serial_conn_fd(struct serial_conn *conn)
{
	if (conn)
		return conn->fd;
	return -1;
}
