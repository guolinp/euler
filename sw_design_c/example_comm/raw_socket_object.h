#ifndef __RAW_SOCKET_CONN_OBJECT_H__
#define __RAW_SOCKET_CONN_OBJECT_H__

#include "conn_interface.h"
#include "socket_filter_interface.h"

struct conn_interface *raw_socket_obj_interface(void);
struct socket_filter_interface *raw_socket_filter_interface(void);

#endif
