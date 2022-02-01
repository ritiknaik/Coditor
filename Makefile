CC=gcc
CF=-c -Wall -Wextra -pedantic -std=c99
F=-Wall -Wextra -pedantic -std=c99

all: editor

force:main.c options.c raw.c rows.c terminal.c syntax.c stack.c
	$(CC) $(F) main.c options.c raw.c rows.c terminal.c syntax.c stack.c -o editorl

editor: main.o options.o raw.o rows.o terminal.o syntax.o stack.o
	$(CC) main.o options.o raw.o rows.o terminal.o syntax.o stack.o -o editorl

main.o: main.c
	$(CC) $(CF) main.c

options.o: options.c
	$(CC) $(CF) options.c

raw.o: raw.c
	$(CC) $(CF) raw.c

rows.o: rows.c
	$(CC) $(CF) rows.c

terminal.o: terminal.c
	$(CC) $(CF) terminal.c

syntax.o: syntax.c
	$(CC) $(CF) syntax.c

stack.o: stack.c
	$(CC) $(CF) stack.c

clean:
	rm -rf *.o editorl
