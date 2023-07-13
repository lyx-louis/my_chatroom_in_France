SOURCE = $(wildcard *.c)
TARGETS = $(patsubst %.c, %, $(SOURCE))

CC = gcc
CFLAGS = -Wall -g
deps =	msg_struct.h
all:$(TARGETS)

$(TARGETS):%:%.c
		$(CC) $< $(CFLAGS) -o $@ $(deps)

.PHONY:clean all
clean:
	-rm -rf $(TARGETS)