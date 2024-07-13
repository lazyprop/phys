CFLAGS=-lraylib -std=c++20

main: main.cpp
	g++ $(CFLAGS) main.cpp -o main
	./main
