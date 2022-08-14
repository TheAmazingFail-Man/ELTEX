BINS = server client
SOURCES = $(BINS:=.c)
OBJECTS = $(BINS:=.o)

CC = gcc
CFLAGS = -g
LDFLAGS =

.PHONY: clean

all: $(BINS)

$(BINS): $(OBJECTS)
	$(CC) $(LDFLAGS) $@.o -o $@

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -rf *.o $(BINS)
