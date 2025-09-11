# =========================
# Unified Makefile
# =========================

# ---- user-space (lbrctl) ----
CC       ?= gcc
CFLAGS   ?= -std=c11 -O2 -g -Wall -Wextra -Wpedantic -fno-omit-frame-pointer
CPPFLAGS ?= -D_GNU_SOURCE
LDFLAGS  ?=
BIN := lbrctl

USER_SRC := \
  lbrctl_common.c \
  lbrctl_config.c \
  lbrctl_run.c \
  main.c

USER_HDR := \
  lbr_API.h \
  lbrctl_common.h

USER_OBJ := $(USER_SRC:.c=.o)

.PHONY: all user module clean run install deps

all: user

user: $(BIN)

$(BIN): $(USER_OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(USER_OBJ) $(LDFLAGS)

%.o: %.c $(USER_HDR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

run: $(BIN)
	./$(BIN)

install: $(BIN)
	install -Dm755 $(BIN) /usr/local/bin/$(BIN)

deps:
	sudo apt update
	sudo apt install -y build-essential linux-headers-$(shell uname -r)

clean:
	rm -f $(USER_OBJ) $(BIN)
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean || true

# ---- kernel module (lbr_driver.ko) ----
# בונים עם Kbuild. ודא שהקבצים האלה הם קוד קרנל (לא כוללים stdio וכו').
obj-m := lbr_driver.o
lbr_driver-y := \
  lbr_interface.o \
  lbr_control.o \
  lbr_info.o \
  lbr_logger.o

module:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
