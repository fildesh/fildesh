
CC = gcc
CFLAGS =
CFLAGS += -g
#CFLAGS += -pg -O2
#CFLAGS += -O3
CFLAGS += -ansi -pedantic
CFLAGS += -Wall -Wextra -Wstrict-aliasing

cx_path = ../cx
bin_path = ../bin

IFLAGS = -I..

CFLAGS += $(IFLAGS)

exe_list = lace add best-match xpipe void cat1 ssh-all ujoin \
		   godo waitdo \
		   chatty
exe_list := $(addprefix $(bin_path)/,$(exe_list))

all: $(exe_list)

$(bin_path)/lace: lace.c \
	$(cx_path)/fileb.o $(cx_path)/bstree.o \
	$(cx_path)/rbtree.o $(cx_path)/syscx.o
	$(CC) $(CFLAGS) \
		"-DUtilBin=\"$(abspath $(bin_path))\"" \
		-o $@ $^

$(bin_path)/add: add.c $(cx_path)/fileb.o $(cx_path)/syscx.o
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/best-match: best-match.c $(cx_path)/fileb.o $(cx_path)/syscx.o
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/xpipe: xpipe.c
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/void: void.c
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/cat1: cat1.c
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/ssh-all: ssh-all.c $(cx_path)/fileb.o $(cx_path)/ospc.o $(cx_path)/syscx.o
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/ujoin: ujoin.c \
	$(cx_path)/fileb.o $(cx_path)/bstree.o $(cx_path)/rbtree.o \
	$(cx_path)/syscx.o
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/godo: godo.c
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/waitdo: waitdo.c $(cx_path)/fileb.o $(cx_path)/syscx.o
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/chatty: chatty.c $(cx_path)/fileb.o $(cx_path)/ospc.o $(cx_path)/syscx.o
	$(CC) $(filter-out -ansi,$(CFLAGS)) $^ -o $@ -lrt $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

$(exe_list): | $(bin_path)

$(bin_path):
	mkdir -p $(bin_path)

.PHONY: clean
clean:
	rm -f *.o $(cx_path)/*.o $(exe_list)

