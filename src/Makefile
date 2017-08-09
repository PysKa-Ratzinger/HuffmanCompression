CC=gcc
CFLAGS=-W -Wall -Wextra -Werror -Wfatal-errors -g
LDLIBS=
OBJECTS=main.o

main: $(OBJECTS)

%.o: %.c %.h

run: main
	./main

clean:
	rm -fv *.o *.gch main
