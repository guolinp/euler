###compiler settings
ARCH          ?=
CROSS_COMPILE ?=

CC      := $(CROSS_COMPILE)gcc
LD      := $(CROSS_COMPILE)ld
CXX     := $(CROSS_COMPILE)g++
AR      := $(CROSS_COMPILE)ar
NM      := $(CROSS_COMPILE)nm
LDR     := $(CROSS_COMPILE)ldr
STRIP   := $(CROSS_COMPILE)strip
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

### path
TOPDIR ?= $(PWD)/..
ifeq ($(TOPDIR),)
	TOPDIR := ..
endif

### lib options
LIBPATH += -L$(TOPDIR)/lib
LIBS    := -leuler -lpthread

### compiling options
CFLAGS  := -g #-O2 -Wall -Wextra -std=c99
CFLAGS  += -I. -I$(TOPDIR)/lib
CFLAGS  += $(LIBPATH)

#CPPFLAGS := -g
#CPPFLAGS += -I. -I$(TOPDIR)/lib
CPPFLAGS += $(LIBPATH)

OUTPUT ?= $(TOPDIR)/output/
$(shell mkdir -p $(OUTPUT))


##################
#MAKE FROM OBJECTS
##################
#c version
#$(call MFO, <app>[, <dep1> <dep2> <dep3> ...], [flags])
define MFO
	$(CC) $(strip $3) $(CFLAGS) $(if $(strip $2), $(strip $2), $(strip $1).o) -o $(OUTPUT)/$(strip $1) $(LIBS)
endef

#c++ version
#$(call MFO_CXX, <app>[, <dep1> <dep2> <dep3> ...], [flags])
define MFO_CXX
	$(CXX) $(strip $3) $(CPPFLAGS) $(if $(strip $2), $(strip $2), $(strip $1).o) -o $(OUTPUT)/$(strip $1) $(LIBS)
endef

##################
#MAKE FROM SOURCES
##################
#$(call MFS, <app>[, <src1> <src2> <src2> ...], [flags])
define MFS
	$(CC) $(strip $3) $(CFLAGS) $(strip $2) -o $(OUTPUT)/$(strip $1) $(LIBS)
endef

#CLEAN OUTPUTS
CLEAN_IGNORE := Makefile %.h %.c %.hpp %.cpp
define CLEAN
	@(rm -rf $(filter-out $(CLEAN_IGNORE), $(shell ls )))
	@(cd $(OUTPUT) && rm -rf $(filter-out $(CLEAN_IGNORE), $(shell ls $(OUTPUT)/)))
endef

##################
# RULES
##################

all: lib_all app_all

clean: lib_clean app_clean

lib_all:
	@cd $(TOPDIR)/lib && make

lib_clean:
	@cd $(TOPDIR)/lib && make clean

