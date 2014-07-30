VERSION = 0.1
CC = gcc
LD = gcc

all: learn

include lib.mk

CFLAGS = -g -O2 -Wall -DHAVE_CONFIG -DREALTIME_SCHEDULING -DVERSION=\"$(VERSION)\" -D_REENTRANT
LDFLAGS =  -ldl -lpthread -lm -lncurses

main.o server.o: CFLAGS += -DDEFAULT_PORT=3000

TEST = $(wildcard *test*.c)
testobj = $(TEST:.c=.o)

SRC = $(filter-out $(TEST), $(wildcard *.c))
obj = $(SRC:.c=.o)


learn : $(obj)
	$(call cmd,ld,$(LDFLAGS))

file_test: file_test.o file.o xmalloc.o
	$(call cmd,ld,$(LDFLAGS))

path_test: path_test.o path.o xmalloc.o
	$(call cmd,ld,$(LDFLAGS))

worker_learn: worker_learn.o worker.o xmalloc.o locking.o debug.o prog.o
	$(call cmd,ld,$(LDFLAGS))

tabexp_file_test: tabexp_file_test.o 

clean += $(obj) $(testobj)
.PHONY: all target realclean clean
