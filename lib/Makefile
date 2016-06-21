SRCS := $(shell ls *.c)
OBJS := $(SRCS:%.c=%.o)

LIBOBJ=libeuler.a

all: $(LIBOBJ)

$(LIBOBJ): $(OBJS)
	$(AR) rcs $@ $^

clean:
	@rm -f $(OBJS) $(LIBOBJ)
	
