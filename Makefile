CC := gcc
CFLAGS := -O2 -Wall -Wextra -std=gnu11

CLI_OBJS := main.o lbrctl_common.o lbrctl_config.o lbrctl_run.o

all: lbrctl

lbrctl: $(CLI_OBJS)
	$(CC) $(CFLAGS) -o $@ $(CLI_OBJS)

%.o: %.c lbr_API.h lbrctl_common.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(CLI_OBJS) lbrctl
