CC := g++
FLAGS := -I ../common -D DEBUG

all: assembler

assembler: main.o assembler.o labels.o
	$(CC) $^ -o $@ 

main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp

assembler.o: assembler.cpp
	$(CC) $(FLAGS) -c assembler.cpp

labels.o: labels.cpp
	$(CC) $(FLAGS) -c labels.cpp

clean:
	rm -rf *.o assembler
