CC=g++
CFLAGS=-W -Wall -Wextra -Werror -Wfatal-errors -g
LDLIBS=
OBJECTS=main.o huffman_algo.o

main: $(OBJECTS)

%.o: %.cpp %.hpp

run: main
	./main

clean:
	rm -fv *.o *.gch main
