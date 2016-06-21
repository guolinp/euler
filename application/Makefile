include ../common.mk

SRCS := $(shell ls *.c)
SRCS_CXX := $(shell ls *.cpp)
OBJS := $(SRCS:%.c=%.o) $(SRCS_CXX:%.cpp=%.o)

LIBS += -lrt

app_all: $(OBJS) cxx_app c_app

cxx_app:
	$(call MFO_CXX, a_cpp_app, c_fun.o cpp_fun.o)

c_app:
	$(call MFO, hello)
	$(call MFO, xmath)
	$(call MFO, revert_string)
	$(call MFO, revert_single_list)
	$(call MFO, thread_private_data)
	$(call MFO, thread_attr)
	$(call MFO, thread_cancel)
	$(call MFO, thread_mutex)
	$(call MFO, thread_name)
	$(call MFO, thread_rwlock, thread_rwlock.o queue.o)
	$(call MFO, thread_cond, thread_cond.o queue.o)
	$(call MFO, timestamp)
	$(call MFO, hexdump)
	$(call MFO, line2argv)
	$(call MFO, log)
	$(call MFO, print)
	$(call MFO, unix_socket)
	$(call MFO, tcp_socket)
	$(call MFO, udp_socket)
	$(call MFO, raw_socket)
	$(call MFO, fifo_comm)
	$(call MFO, shm_comm)
	$(call MFO, ptty_comm)
	$(call MFO, msgq_comm)
	$(call MFO, serial_comm)

app_clean:
	$(call CLEAN)
