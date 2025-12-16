CFILES = $(shell find . -name "*.c")
OFILES = $(CFILES:.c=.o)

CC = clang
CFLAGS = -Wall -pedantic -Iinclude/

.PHONY: all
all: $(OFILES)
	$(CC) $^ -o batmon

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
