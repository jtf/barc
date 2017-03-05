CC=gcc
CFLAGS=-std=gnu11 -Wpedantic -Wall -Wextra -O3 -I.
BIN=barc
DEPS=barc-args.h  barc-ean.h  barc.h  barc-parse.h barc.c barc-args.c barc-ean.c barc-parse.c gitversion.h


barc: $(DEPS)
	$(CC) $(CFLAGS)  barc.c barc-args.c barc-ean.c barc-parse.c -o $(BIN)

gitversion.h: .git/HEAD .git/index
	echo "const char *gitversion = \"$(shell git describe --tags --always)\";" > $@

.PHONY: clean

clean:
	rm $(BIN)
	rm gitversion.h

