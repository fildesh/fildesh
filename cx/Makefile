
CC = gcc

CFLAGS =
CFLAGS += -ansi -pedantic
CFLAGS += -Wall -Wextra
CFLAGS += -g

CExes = cx verify chatty

all: $(CExes)

verify: verif.c bstree.o fileb.o rbtree.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

cx: cx.c bstree.o fileb.o rbtree.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

chatty: chatty.c fileb.o
	$(CC) $(filter-out -ansi,$(CFLAGS)) $^ -o $@ -lrt $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(CExes)

