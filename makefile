CC=gcc
serverObj = handler.o server.o 
clientObj = client.o
serverH = ./src/handler.h

all: client server

client : $(clientObj)
	$(CC) -o ./bin/client $(clientObj)

server : $(serverObj)
	$(CC) -o ./bin/server $(serverObj) -lpthread

client.o : ./src/client.c
	$(CC) -c ./src/client.c 

server.o : ./src/server.c $(serverH)
	$(CC) -c ./src/server.c $(serverH) 

handler.o : ./src/handler.c
	$(CC) -c ./src/handler.c 
	
cleanAll :
	rm $(serverObj) $(clientObj) ./bin/client ./bin/server

cleanObj:
	rm $(serverObj) $(clientObj)