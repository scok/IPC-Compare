CC = gcc
CFLAGS = -Wall

all: server client msqserver msqclient mserver mclient

server : server.c
	$(CC) $(CFLAGS) server.c -o server -lpthread

client : client.c
	$(CC) $(CFLAGS) client.c -o client -lpthread

msqserver : msqserver.c
	$(CC) $(CFLAGS) msqserver.c -o msqserver -lpthread

msqclient : msqclient.c
	$(CC) $(CFLAGS) msqclient.c -o msqclient -lpthread

mserver : mserver.c
	$(CC) $(CFLAGS) mserver.c -o mserver -lpthread

mclient : mclient.c
	$(CC) $(CFLAGS) mclient.c -o mclient -lpthread

clean:
	rm -f server
	rm -f client
	rm -f msqserver
	rm -f msqclient
	rm -f mserver
	rm -f mclient
