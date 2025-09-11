# =========================
# Unified Makefile
# =========================

# ---- paths for kernel build ----
PWD  := $(shell pwd)
KDIR ?= /lib/modules/$(shell uname -r)/build

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
  lbrctl_status.c \
  main.c

USER_HDR := \
  lbr_API.h \
  lbrctl_common.h \
  lbrctl_config.h \
  lbrctl_run.h \
  lbrctl_status.h

USER_OBJ := $(USER_SRC:.c=.o)

# ---- kernel module (lbr_driver.ko) ----
# Kbuild links these objects into lbr_driver.ko
obj-m := lbr_driver.o
lbr_driver-y := \
  lbr_interface.o \
  lbr_control.o  \
  lbr_info.o     \
  lbr_logger.o

# -------- Targets --------
.PHONY: all user module clean run install deps

all: user module

# --- User-space binary ---
user: $(BIN)

$(BIN): $(USER_OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(USER_OBJ) $(LDFLAGS)

%.o: %.c $(USER_HDR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

# Run with optional ARGS: make run ARGS="status"
run: $(BIN)
	./$(BIN) $(ARGS)

install: $(BIN)
	install -Dm755 $(BIN) /usr/local/bin/$(BIN)

deps:
	sudo apt update
	# headers לקרנל: יעבוד על Ubuntu אמיתי/VM; ב-WSL לרוב לא זמין
	sudo apt install -y build-essential linux-headers-$(shell uname -r)

# --- Kernel module ---
module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# --- Cleaning both ---
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean || true
	rm -f $(USER_OBJ) $(BIN)
