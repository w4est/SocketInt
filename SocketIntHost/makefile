all: sockethost.o main.o host

sockethost.o: sockethost.cpp sockethost.h
	g++ -Wall -c sockethost.cpp

main.o: main.cpp
	g++ -Wall -c main.cpp

host : sockethost.o main.o
	g++ -Wall sockethost.o main.o -o host

clean:
	\rm *.o *~ host
