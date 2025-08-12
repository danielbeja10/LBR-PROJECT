
CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra


USER_SRCS = main.c lbrctl_common.c lbrctl_run.c $(wildcard lbrctl_config.c)
USER_BIN  = lbrctl

$(USER_BIN): $(USER_SRCS) lbr_API.h lbrctl_common.h
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

obj-m += lbr_interface.o

lbr_interface-objs := lbr_interface.o lbr_control.o lbr_info.o lbr_logger.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

km:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

kclean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: all clean km kclean
all: $(USER_BIN)
clean:
	rm -f $(USER_BIN)
	$(MAKE) -C $(KDIR) M=$(PWD) clean >/dev/null 2>&1 || true
