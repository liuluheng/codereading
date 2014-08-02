VERSION = 0.1
#CC = gcc
#LD = gcc

CC = clang
LD = clang

CFLAGS  = -g -O2 -Wall -DHAVE_CONFIG -DREALTIME_SCHEDULING -DVERSION=\"$(VERSION)\" -D_REENTRANT
LDFLAGS =  -ldl -lpthread -lm -lncurses

main.o server.o: CFLAGS += -DDEFAULT_PORT=3000

SRC_LEARN = $(wildcard *learn*.c)
OBJ_LEARN = $(SRC_LEARN:.c=.o)
BIN_LEARN = $(SRC_LEARN:.c=)

SRC = $(filter-out $(SRC_LEARN), $(wildcard *.c))
obj = $(SRC:.c=.o)

all: $(BIN_LEARN)

include lib.mk

learn : $(obj)
	$(call cmd,ld,$(LDFLAGS))

file_learn: file_learn.o file.o xmalloc.o
	$(call cmd,ld,$(LDFLAGS))

path_learn: path_learn.o path.o xmalloc.o
	$(call cmd,ld,$(LDFLAGS))

worker_learn: worker_learn.o worker.o xmalloc.o locking.o debug.o prog.o
	$(call cmd,ld,$(LDFLAGS))

list_mergesort_learn: list_mergesort_learn.o xmalloc.o mergesort.o
	$(call cmd,ld,$(LDFLAGS))

clean += $(obj) $(OBJ_LEARN)

.PHONY: all realclean clean
