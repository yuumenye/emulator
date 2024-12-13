CC := g++
FLAGS := -I ../common

all: assembler

assembler: main.o assembler.o
	$(CC) $^ -o $@ 

main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp

assembler.o: assembler.cpp
	$(CC) $(FLAGS) -c assembler.cpp

clean:
	rm -rf *.o assembler
