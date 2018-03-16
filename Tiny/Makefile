tiny: main.o scan.o parse.o util.o symtab.o analyze.o cgen.o
	gcc main.o scan.o parse.o util.o symtab.o analyze.o cgen.o -o tiny
main.o: main.c scan.h globals.h
	gcc -c main.c
scan.o: scan.c scan.h globals.h
	gcc -c scan.c
parse.o: parse.c parse.h globals.h
	gcc -c parse.c
util.o: util.c util.h globals.h
	gcc -c util.c
symtab.o: symtab.c symtab.h globals.h
	gcc -c symtab.c
analyze.o: analyze.c analyze.h globals.h
	gcc -c analyze.c
cgen.o: cgen.c cgen.h globals.h
	gcc -c cgen.c
