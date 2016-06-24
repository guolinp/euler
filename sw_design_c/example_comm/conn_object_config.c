#include <stdio.h>
#include "conn_interface.h"

#include "unix_conn_object.h"
#include "fifo_conn_object.h"
#include "raw_socket_object.h"

struct conn_object_config {
	const char *obj_name;
	struct conn_interface *(*get_obj_interface) (void);
};

static struct conn_object_config all_support_objects[] = {
	{
	 .obj_name = "UNIX_SOCKET",
	 .get_obj_interface = unix_obj_interface,
	 },
	{
	 .obj_name = "FIFO",
	 .get_obj_interface = fifo_obj_interface,
	 },
	{
	 .obj_name = "RAW_SOCKET",
	 .get_obj_interface = raw_socket_obj_interface,
	 },
	{
	 NULL,
	 NULL,
	 },
};

struct conn_interface *find_object_interface(const char *objname)
{
	struct conn_object_config *conf;
	struct conn_interface *interface;

	conf = all_support_objects;
	while (conf->obj_name) {
		if (strcmp(conf->obj_name, objname) == 0)
			break;
		conf++;
	}

	if (!conf->obj_name || !conf->get_obj_interface)
		return NULL;

	return conf->get_obj_interface();
}
