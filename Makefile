SHELL = /bin/sh
.SUFFIXES: .o .c
.PHONY: clean

CC = gcc
CFLAGS = -g -Wall -O3
LDFLAGS =

bindir = bin
libdir = obj
srcdir = src

objects = $(libdir)/test.o $(libdir)/jpeg.o $(libdir)/hufftree.o $(libdir)/quanttable.o $(libdir)/stream.o $(libdir)/image.o
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
	rm -f $(bindir)/* $(libdir)/*
