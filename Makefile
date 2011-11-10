
CC = gcc
CFLAGS = -ansi -pedantic -Wall -Wextra
CFLAGS += -D_SVID_SOURCE
CFLAGS += -g

all: lace best-match xpipe

lace: lace.c
	$(CC) $(CFLAGS) -o $@ $^

best-match: best-match.c
	$(CC) $(CFLAGS) -o $@ $^

xpipe: xpipe.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f best-match lace

