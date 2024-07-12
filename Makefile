CFLAGS=-lraylib -std=c++20

phys: phys.cpp
	g++ $(CFLAGS) phys.cpp -o phys
	./phys
