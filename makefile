# include paths
INC = -I ./include

# shared
command.o: src/shared/command.h src/shared/command.cpp
	g++ -c src/shared/command.cpp -o command.o $(INC)

# client

client.o: src/client/client.h src/client/client.cpp
	g++ -c src/client/client.cpp -o client.o $(INC)

main_client.o: src/client/main.cpp
	g++ -c src/client/main.cpp -o main_client.o $(INC)

client.exe: client.o main_client.o command.o
	g++ client.o main_client.o command.o -o client.exe -ljsoncpp
	rm -f client.o main_client.o

# server

server.o: src/server/server.h src/server/server.cpp
	g++ -c src/server/server.cpp -o server.o $(INC)

main_server.o: src/server/main.cpp
	g++ -c src/server/main.cpp -o main_server.o $(INC)

server.exe: server.o main_server.o command.o
	g++ server.o main_server.o command.o -o server.exe -ljsoncpp
	rm -f server.o main_server.o


# general

clean:
	rm -f client.o server.o main_client.o main_server.o client.exe server.exe

all: client.exe server.exe

compile-all: client.exe server.exe

compile-all-and-run-client: client.exe server.exe
	./client.exe

compile-all-and-run-server: client.exe server.exe
	./server.exe

.PHONY: clean