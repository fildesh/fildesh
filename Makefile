
CC = gcc
CFLAGS = -ansi -pedantic -Wall -Wextra
CFLAGS += -D_SVID_SOURCE
CFLAGS += -g

lace: lace.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f lace

