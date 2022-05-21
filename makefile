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

$(bindir)/test: $(objects)
	$(CC) $(CFLAGS) $^ -o $@

$(libdir)/%.o: $(srcdir)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

setup:
	mkdir $(bindir) $(libdir)

clean:
	rm -f $(bindir)/* $(libdir)/*