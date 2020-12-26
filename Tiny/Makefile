CC = gcc
CFLAGS += -Wall -O2

tiny: main.o scan.o parse.o util.o symtab.o analyze.o cgen.o
	$(CC) $(CFLAGS) main.o scan.o parse.o util.o symtab.o analyze.o cgen.o -o tiny
main.o: main.c scan.h globals.h
	$(CC) $(CFLAGS) -c main.c
scan.o: scan.c scan.h globals.h
	$(CC) $(CFLAGS) -c scan.c
parse.o: parse.c parse.h globals.h
	$(CC) $(CFLAGS) -c parse.c
util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c
symtab.o: symtab.c symtab.h globals.h
	$(CC) $(CFLAGS) -c symtab.c
analyze.o: analyze.c analyze.h globals.h
	$(CC) $(CFLAGS) -c analyze.c
cgen.o: cgen.c cgen.h globals.h
	$(CC) $(CFLAGS) -c cgen.c

clean:
	rm -f *.o tiny

.PHONY: clean
