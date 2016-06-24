ifeq ($(WHEREAMI), ASB)
UNSUPPORT_FUNCTIONALS  = pcap_comm.c  # cannot found the file <pcap.h>
#UNSUPPORT_FUNCTIONALS += xxxx.c      # why?
endif

SRCS := $(shell ls *.c)
SRCS := $(filter-out $(UNSUPPORT_FUNCTIONALS), $(SRCS))
OBJS := $(SRCS:%.c=%.o)

LIBOBJ=libeuler.a

all: $(LIBOBJ)

$(LIBOBJ): $(OBJS)
	$(AR) rcs $@ $^

clean:
	@rm -f $(OBJS) $(LIBOBJ)
	
