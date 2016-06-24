#ifndef __CONN_OBJECT_H__
#define __CONN_OBJECT_H__

#include "conn_interface.h"

struct conn_object;

struct conn_object *conn_object_new(const char *objname);
void conn_object_delete(struct conn_object *obj);
void conn_object_connection_free(struct conn_object *obj);
int conn_object_accept(struct conn_object *obj, const char *pathname, void *options);
int conn_object_connect(struct conn_object *obj, const char *pathname, void *options);
int conn_object_send(struct conn_object *obj, char *buf, int size);
int conn_object_recv(struct conn_object *obj, char *buf, int size);
int conn_object_fd(struct conn_object *obj);
void *conn_object_real_obj(struct conn_object *obj);
int conn_object_typeof(struct conn_object *obj, const char *objname);

#endif
