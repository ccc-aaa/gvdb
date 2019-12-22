CC=gcc
CFLAGS=-O3 -march=native $(shell pkg-config --cflags glib-2.0)
LDFLAGS=$(shell pkg-config --libs glib-2.0)

all: reader

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

reader: reader.o gvdb-reader.o
	$(CC) gvdb-reader.o reader.o -o reader $(LDFLAGS)

clean: gvdb-reader.o reader
	rm gvdb-reader.o reader.o reader

