SHELL = /bin/sh
.SUFFIXES: .o .c
.PHONY: setup clean

CC = gcc
CFLAGS = -g -Wall

bindir = bin
libdir = obj
srcdir = src

objects = $(libdir)/test.o $(libdir)/jpeg.o $(libdir)/hufftree.o $(libdir)/quanttable.o $(libdir)/stream.o $(libdir)/image.o

all: $(bindir)/test

$(bindir)/test: setup $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

$(libdir)/%.o: $(srcdir)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

setup:
	[ ! -d $(libdir) ] && mkdir $(libdir) || true
	[ ! -d $(bindir) ] && mkdir $(bindir) || true

clean:
	rm -f $(bindir)/* $(libdir)/*