CC=gcc

ifeq "${DEBUG}" "1"
	CFLAGS=-Og -g
else
	CFLAGS=-O3
endif

ifeq "${ASAN}" "1"
	LDFLAGS+= -fsanitize=address
	STATIC=0
endif

ifeq "${STATIC}" "1"
	CFLAGS+= $(shell pkg-config --static --cflags glib-2.0)
	LDFLAGS+= --static $(shell pkg-config --static --libs glib-2.0)
else
	CFLAGS+= $(shell pkg-config --cflags glib-2.0)
	LDFLAGS+= $(shell pkg-config --libs glib-2.0)
endif

all: reader

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

reader: reader.o gvdb-reader.o
	$(CC) gvdb-reader.o reader.o -o reader $(LDFLAGS)

clean: gvdb-reader.o reader
	rm gvdb-reader.o reader.o reader

