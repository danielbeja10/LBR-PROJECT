# ===== מודול קרנל: lbr_interface.ko =====
obj-m += lbr_interface.o
lbr_interface-objs := lbr_interface.o lbr_control.o lbr_info.o lbr_logger.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

# ===== בניית כלי משתמש: lbrctl =====
CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra -std=gnu11 -D_GNU_SOURCE
LDFLAGS ?=
USR_SRCS := main.c lbrctl_common.c lbrctl_config.c lbrctl_run.c
USR_BIN  := lbrctl

.PHONY: all module user clean load unload reinstall

# ברירת מחדל: לבנות גם מודול וגם כלי משתמש
all: module user

# מודול קרנל
module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# כלי משתמש
user: $(USR_BIN)

$(USR_BIN): $(USR_SRCS) lbr_API.h
	$(CC) $(CFLAGS) -o $@ $(USR_SRCS) $(LDFLAGS)

# ניקוי
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) -f $(USR_BIN) Module.symvers modules.order

# נוחות: טעינת מודול וצפייה ב-dmesg
load: module
	-@sudo rmmod lbr_interface 2>/dev/null || true
	sudo insmod lbr_interface.ko
	dmesg | tail -n 20

# נוחות: פריקת מודול
unload:
	-@sudo rmmod lbr_interface || true
	dmesg | tail -n 20
