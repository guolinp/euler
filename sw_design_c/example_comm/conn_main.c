#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"
#include "conn_object.h"
#include "socket_filter_interface.h"
#include "raw_socket_object.h"

static void attach_filter(struct conn_object *obj)
{
	struct socket_filter_interface *interface;
	void *real_obj = conn_object_real_obj(obj);

	if (!conn_object_typeof(obj, "RAW_SOCKET"))
		return;

	interface = raw_socket_filter_interface();
	if (interface) {
		attach_socket_filter(interface, real_obj, 0, 0);	//just a test calling
		detach_socket_filter(interface, real_obj, 0, 0);	//just a test calling
	}
}

static void run_server(const char *objname, const char *path)
{
	struct conn_object *obj;
	char buf[128];
	int ret;

	obj = conn_object_new(objname);
	if (!obj) {
		dbg_printf("cannot create %s object\n", objname);
		return;
	}

	while (1) {
		ret = conn_object_accept(obj, path, 0);
		if (ret) {
			dbg_printf("accept failed\n");
			sleep(1);
			continue;
		}

		attach_filter(obj);

		while (1) {
			ret = conn_object_recv(obj, buf, 127);
			if (ret <= 0) {
				dbg_printf("receive failed\n");
				conn_object_connection_free(obj);;
				break;
			}
			buf[ret] = '\0';
			trc_printf("receive data: %s\n", buf);
		}
	}
}

static void run_client(const char *objname, const char *path)
{
	struct conn_object *obj;
	char *str = "this is a comm tx/tx test string\n";
	int ret;

	obj = conn_object_new(objname);
	if (!obj) {
		dbg_printf("cannot create %s object\n", objname);
		return;
	}

	ret = conn_object_connect(obj, path, 0);
	if (ret) {
		dbg_printf("connect failed\n");
		return;
	}

	ret = conn_object_send(obj, str, strlen(str));
	if (ret <= 0)
		dbg_printf("send failed\n");
	else
		trc_printf("send data: %s\n", str);

	conn_object_delete(obj);
}

int main(int argc, char *argv[])
{
	if (argc >= 4) {
		if (strcmp(argv[1], "server") == 0)
			run_server(argv[2], argv[3]);
		else if (strcmp(argv[1], "client") == 0)
			run_client(argv[2], argv[3]);
		else
			fprintf(stderr, "bad input\n");
	} else {
		fprintf(stderr, "Usage: %s <server|client> <objname> <pathname>\n", argv[0]);
	}

	return 0;
}
