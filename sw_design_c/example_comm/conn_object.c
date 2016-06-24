#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "print.h"

#include "conn_interface.h"
#include "conn_object_config.h"
#include "conn_object.h"

struct conn_object {
	const char *obj_name;
	struct conn_interface *obj_interface;
	void *obj_private;
};

static void fake_free(void *conn)
{
	dbg_printf("BAD CALLING\n");
	return;
}

static void *fake_accept(const char *pathname, void *options)
{
	dbg_printf("BAD CALLING\n");
	return NULL;
}

static void *fake_connect(const char *pathname, void *options)
{
	dbg_printf("BAD CALLING\n");
	return NULL;
}

static int fake_fd(void *conn)
{
	dbg_printf("BAD CALLING\n");
	return -ENODEV;
}

static void fill_fake_interface(struct conn_interface *interface)
{
	if (!interface->conn_accept)
		interface->conn_accept = fake_accept;
	if (!interface->conn_connect)
		interface->conn_connect = fake_connect;
	if (!interface->conn_fd)
		interface->conn_fd = fake_fd;
}

struct conn_object *conn_object_new(const char *objname)
{
	struct conn_interface *interface;
	struct conn_object *obj;

	interface = find_object_interface(objname);
	if (!interface)
		return NULL;

	obj = malloc(sizeof(*obj));
	if (!obj)
		return NULL;

	fill_fake_interface(interface);

	obj->obj_name = objname;
	obj->obj_interface = interface;
	obj->obj_private = NULL;

	return obj;
}

void conn_object_connection_free(struct conn_object *obj)
{
	if (obj)
		obj->obj_interface->conn_free(obj->obj_private);
}

void conn_object_delete(struct conn_object *obj)
{
	if (obj) {
		conn_object_connection_free(obj);
		free(obj);
	}
}

int conn_object_accept(struct conn_object *obj, const char *pathname, void *options)
{
	void *conn;
	if (!obj || !pathname)
		return -EINVAL;
	conn = obj->obj_interface->conn_accept(pathname, options);
	if (!conn)
		return -ENODEV;
	obj->obj_private = conn;
	return 0;
}

int conn_object_connect(struct conn_object *obj, const char *pathname, void *options)
{
	void *conn;
	if (!obj || !pathname)
		return -EINVAL;
	conn = obj->obj_interface->conn_connect(pathname, options);
	if (!conn)
		return -ENODEV;
	obj->obj_private = conn;
	return 0;
}

int conn_object_send(struct conn_object *obj, char *buf, int size)
{
	int fd;
	void *conn;

	if (!obj || !buf || size <= 0)
		return -EINVAL;

	conn = obj->obj_private;
	if (obj->obj_interface->conn_send)
		return obj->obj_interface->conn_send(conn, buf, size);

	fd = conn_object_fd(obj);
	if (fd < 0)
		return -EINVAL;

	return write(fd, buf, size);
}

int conn_object_recv(struct conn_object *obj, char *buf, int size)
{
	int fd;
	void *conn;
	if (!obj || !buf || size <= 0)
		return -EINVAL;

	conn = obj->obj_private;
	if (obj->obj_interface->conn_recv)
		return obj->obj_interface->conn_recv(conn, buf, size);

	fd = conn_object_fd(obj);
	if (fd < 0)
		return -EINVAL;

	return read(fd, buf, size);
}

int conn_object_fd(struct conn_object *obj)
{
	void *conn;
	if (!obj)
		return -EINVAL;
	conn = obj->obj_private;
	return obj->obj_interface->conn_fd(conn);
}

void *conn_object_real_obj(struct conn_object *obj)
{
	if (obj)
		return obj->obj_private;
	return NULL;
}

int conn_object_typeof(struct conn_object *obj, const char *objname)
{
	if (obj && objname)
		return strcmp(obj->obj_name, objname) == 0;
	return 0;
}
