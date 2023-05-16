client.o: src/client/client.cpp
	g++ -c src/client/client.cpp -o client.o

server.o: src/server/server.cpp
	g++ -c src/server/server.cpp -o server.o

client.exe: client.o
	g++ client.o -o client.exe
	rm -f client.o

server.exe: server.o
	g++ server.o -o server.exe
	rm -f server.o

clean:
	rm -f client.o server.o client.exe server.exe

all: client.exe server.exe

compile-all: client.exe server.exe

compile-all-and-run-client: client.exe server.exe
	./client.exe

compile-all-and-run-server: client.exe server.exe
	./server.exe

.PHONY: clean