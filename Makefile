CC = gcc
CFLAGS = -O0 -ggdb -Wall -Werror $(shell pkg-config --cflags cairo librsvg-2.0)
LIBS = $(shell pkg-config --libs cairo librsvg-2.0)

generate: generate.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
