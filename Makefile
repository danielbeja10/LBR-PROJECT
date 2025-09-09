# ===== lbrctl (user-space) =====

# קבצי מקור לפי העץ בתמונה
SRC := \
  lbr_control.c \
  lbr_info.c \
  lbr_interface.c \
  lbr_logger.c \
  lbrctl_common.c \
  main.c

# כותרות (אם חלק לא קיימים אצלך – אפשר למחוק מהרשימה)
HDR := \
  lbr_API.h \
  lbr_control.h \
  lbr_info.h \
  lbr_interface.h \
  lbr_logger.h \
  lbrctl_common.h

BIN := lbrctl

# דגלי קומפילציה
CC       ?= gcc
CFLAGS   ?= -std=c11 -O2 -g -Wall -Wextra -Wpedantic -fno-omit-frame-pointer
CPPFLAGS ?= -D_GNU_SOURCE
LDFLAGS  ?=

OBJ := $(SRC:.c=.o)

.PHONY: all clean run install deps

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# כלל כללי לקבצי ‎.o‎ – תלויות גם בכותרות
%.o: %.c $(HDR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

install: $(BIN)
	install -Dm755 $(BIN) /usr/local/bin/$(BIN)

clean:
	rm -f $(OBJ) $(BIN)

# התקנת תלותים בסביבת לייב אופציונלית
deps:
	sudo apt update
	sudo apt install -y build-essential git
