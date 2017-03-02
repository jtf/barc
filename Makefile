CC=gcc
CFLAGS=-std=gnu11 -Wpedantic -Wall -Wextra -O3 -I.

DEPS=barc-args.h  barc-ean.h  barc.h  barc-parse.h

barc: $(DEPS)
	$(CC) $(CFLAGS)  barc.c barc-args.c barc-ean.c barc-parse.c -o barc

