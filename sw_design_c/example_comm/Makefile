TOPDIR = ../..
include ../../common.mk

SRCS := $(shell ls *.c)
OBJS := $(SRCS:%.c=%.o)

LIBS += -lrt

app_all: $(OBJS) c_app

c_app:
	$(call MFO, conn_main, $(OBJS))

app_clean:
	$(call CLEAN)
