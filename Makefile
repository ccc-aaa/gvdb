ifeq "${ASAN}" "1"
	LDFLAGS+= -fsanitize=address
	STATIC=0
	DEBUG=1
endif

ifeq "${DEBUG}" "1"
	CFLAGS=-Og -g
else
	CFLAGS=-O3
endif

ifeq "${STATIC}" "0"
	CFLAGS+= $(shell pkg-config --cflags glib-2.0)
	LDFLAGS+= $(shell pkg-config --libs glib-2.0)
else
	CFLAGS+= $(shell pkg-config --static --cflags glib-2.0)
	LDFLAGS+= --static $(shell pkg-config --static --libs glib-2.0)
endif

all: gvdb

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

gvdb: reader.o gvdb-reader.o
	$(CC) gvdb-reader.o reader.o -o gvdb $(LDFLAGS)

clean:
	rm -f gvdb-reader.o reader.o reader gvdb

