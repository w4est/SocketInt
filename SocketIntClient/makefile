all: socketclient.o main.o client

socketclient.o: socketclient.cpp socketclient.h
	g++ -Wall -c socketclient.cpp

main.o: main.cpp
	g++ -Wall -c main.cpp

client : socketclient.o main.o
	g++ -Wall socketclient.o main.o -o client

clean:
	\rm *.o *~ client
