
CC = gcc
CFLAGS = -ansi -pedantic
CFLAGS += -Wall -Wextra -Wstrict-aliasing
CFLAGS += -g
#CFLAGS += -O3

cx_path = ../cx
bin_path = ../bin

IFLAGS = -I$(cx_path)/..

CFLAGS += $(IFLAGS)

exe_list = lace add best-match xpipe void cat1 ssh-all ujoin godo satsyn
exe_list := $(addprefix $(bin_path)/,$(exe_list))

all: $(exe_list)

$(bin_path)/lace: lace.c \
	$(cx_path)/fileb.o $(cx_path)/bstree.o \
	$(cx_path)/rbtree.o $(cx_path)/sys-cx.o
	$(CC) $(CFLAGS) -D_SVID_SOURCE \
		"-DUtilBin=\"$(bin_path)\"" \
		-o $@ $^

$(bin_path)/add: add.c $(cx_path)/fileb.o $(cx_path)/sys-cx.o
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/best-match: best-match.c $(cx_path)/fileb.o $(cx_path)/sys-cx.o
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/xpipe: xpipe.c
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/void: void.c
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/cat1: cat1.c
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/ssh-all: ssh-all.c
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/ujoin: ujoin.c \
	$(cx_path)/fileb.o $(cx_path)/bstree.o $(cx_path)/rbtree.o \
	$(cx_path)/sys-cx.o
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/godo: godo.c
	$(CC) $(CFLAGS) -o $@ $^

$(bin_path)/satsyn: satsyn.c $(cx_path)/fileb.o $(cx_path)/sys-cx.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: $(cx_path)
$(cx_path):
	$(MAKE) -C $(cx_path)

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

$(exe_list): | $(bin_path)

$(bin_path):
	mkdir -p $(bin_path)

.PHONY: clean
clean:
	rm -f *.o $(exe_list)

