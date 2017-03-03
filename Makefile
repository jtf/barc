CC=gcc
CFLAGS=-std=gnu11 -Wpedantic -Wall -Wextra -O3 -I.
BIN=barc
DEPS=barc-args.h  barc-ean.h  barc.h  barc-parse.h barc.c barc-args.c barc-ean.c barc-parse.c

barc: $(DEPS)
	$(CC) $(CFLAGS)  barc.c barc-args.c barc-ean.c barc-parse.c -o $(BIN)

.PHONY: clean
clean:
	rm $(BIN)
