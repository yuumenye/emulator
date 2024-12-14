CC := g++
FLAGS := -I ../common -D DEBUG

all: processor

processor: main.o processor.o stack.o
	$(CC) $^ -o $@ 

main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp

processor.o: processor.cpp
	$(CC) $(FLAGS) -c processor.cpp

stack.o: stack.cpp
	$(CC) $(FLAGS) -c stack.cpp

clean:
	rm -rf *.o processor
