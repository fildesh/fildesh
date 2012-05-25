
CC = gcc
CFLAGS = -ansi -pedantic -Wall -Wextra
CFLAGS += -D_SVID_SOURCE
CFLAGS += -g
#CFLAGS += -O3

utils = lace add best-match xpipe void cat1 ssh-all ujoin godo
util_exes = $(addprefix bin/,$(utils))

all: $(util_exes)

bin/lace: lace.c cx/fileb.o cx/bstree.o cx/rbtree.o cx/sys-cx.o
	$(CC) $(CFLAGS) \
		"-DUtilBin=\"$(PWD)/bin\"" \
		-o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

bin/add: add.c cx/fileb.o
	$(CC) $(CFLAGS) -o $@ $^

bin/best-match: best-match.c cx/fileb.o
	$(CC) $(CFLAGS) -o $@ $^

bin/xpipe: xpipe.c
	$(CC) $(CFLAGS) -o $@ $^

bin/void: void.c
	$(CC) $(CFLAGS) -o $@ $^

bin/cat1: cat1.c
	$(CC) $(CFLAGS) -o $@ $^

bin/ssh-all: ssh-all.c
	$(CC) $(CFLAGS) -o $@ $^

bin/ujoin: ujoin.c cx/fileb.o cx/bstree.o cx/rbtree.o
	$(CC) $(CFLAGS) -o $@ $^

bin/godo: godo.c
	$(CC) $(CFLAGS) -o $@ $^

$(util_exes): | bin

bin:
	mkdir -p bin

.PHONY: clean
clean:
	rm -f $(util_exes)

