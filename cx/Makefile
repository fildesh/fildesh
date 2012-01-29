
CC = gcc

CFLAGS = -ansi -pedantic
CFLAGS += -g
CFLAGS += -Wall -Wextra

all: cx verify

verify: verif.c bstree.o rbtree.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

cx: cx.c bstree.o rbtree.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

