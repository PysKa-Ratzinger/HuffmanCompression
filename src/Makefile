CC=g++
CFLAGS=-W -Wall -Wextra -Werror -Wfatal-errors -g
CXXFLAGS=-W -Wall -Wextra -Werror -Wfatal-errors -g
LDLIBS=
OBJECTS=main.o huffman_algo.o bitstream.o character_encoding.o

main: $(OBJECTS)

%.o: %.cpp %.hpp

run: main
	./main

debug: main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main

clean:
	rm -fv *.o *.gch main
