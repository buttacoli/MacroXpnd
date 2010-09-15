SHELL=/bin/sh
MAKEFLAGS=

CLIBS=
CFLAGS=-ggdb

BISON=/usr/bin/bison
FLEX=/usr/bin/flex

SRC=main.c \
    syntax.lex.c \
    findFile.c

OBJS=$(subst .c,.o,$(SRC))

    
.PHONY: clean
syntax: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS) 
syntax.lex.c: syntax.l
	$(FLEX) --stdout $< > $@
findFile.o: findFile.c findFile.h
syntax.lex.o: syntax.lex.c findFile.h
main.o: main.c

clean:
	-rm syntax.lex.c findFile.o syntax

