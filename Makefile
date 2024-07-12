CFLAGS=-lraylib

phys: phys.cpp
	g++ $(CFLAGS) phys.cpp -o phys
	./phys
