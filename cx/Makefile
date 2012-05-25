
CC = gcc

CFLAGS =
CFLAGS += -ansi -pedantic
CFLAGS += -Wall -Wextra
CFLAGS += -g
#CFLAGS += -O3

CExes = cx verify chatty

all: $(CExes)

verify: verif.c bstree.o fileb.o rbtree.o sys-cx.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

cx: cx.c bstree.o fileb.o rbtree.o sys-cx.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

chatty: chatty.c fileb.o sys-cx.o
	$(CC) $(filter-out -ansi,$(CFLAGS)) $^ -o $@ -lrt $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(CExes)

