SHELL=/bin/sh
.SUFFIXES: .o .c
.PHONY: clean remake

CC = gcc
CFLAGS=-g -Wall -Wextra -Wshadow -pedantic -O3 -std=c99
LDFLAGS=

bindir=bin
libdir=obj
srcdir=src

objects=$(libdir)/test.o $(libdir)/jpeg.o $(libdir)/hufftree.o $(libdir)/bitstream.o $(libdir)/image.o $(libdir)/parsing.o $(libdir)/decoding.o
.SECONDARY: $(objects)

all: $(bindir)/test

$(libdir)/%.o: $(srcdir)/%.c | $(libdir)
	$(CC) -c $(CFLAGS) $< -o $@

$(bindir)/%: $(objects) | $(bindir)
	$(CC) $(LDFLAGS) $^ -o $@

$(libdir):
	mkdir $(libdir)

$(bindir):
	mkdir $(bindir)

clean:
	rm -rf $(bindir)/* $(libdir)/*

remake:
	make clean
	make
