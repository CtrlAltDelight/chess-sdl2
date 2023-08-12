SRC_C:=$(wildcard *.c)
SRC_H:=$(wildcard *.h)
OBJS:=$(SRC_C:.c=.o)
CFLAGS:=-std=c11 -pedantic -Wvla -Wall -Wshadow -g
SHELL:=/bin/bash
EXECUTABLE:=chess
CC:=gcc
LINK_SDL:=`sdl2-config --libs --cflags` -lSDL2_image

$(EXECUTABLE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(CFLAGS) $(LINK_SDL)

%.o: %.c $(SRC_H)
	$(CC) -c $< $(CFLAGS) -o $@

test: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) $(OBJS)

all: $(EXECUTABLE)

valgrind: $(EXECUTABLE)
	valgrind --suppressions=./valgrind.sup ./$(EXECUTABLE)
