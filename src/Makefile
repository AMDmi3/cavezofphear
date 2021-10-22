OBJS = main.o frame.o isready.o loadmap.o misc.o gplot.o splash.o chk.o editor.o

DESTDIR = ..

CC = gcc
INSTALL = install
CFLAGS ?= -s -Wall -O2
LDFLAGS += -lncurses

all: phear install clean

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

phear: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ ${LDFLAGS}

install: install-game

install-game: phear
	$(INSTALL) $^ $(DESTDIR) 

clean:
	rm -f *.o phear
