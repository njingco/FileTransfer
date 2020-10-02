CC=gcc -Wall -g
CLIB = -lpthread

all: server client

server: server.o
	$(CC) -o server server.o $(CLIB)

client: client.o
	$(CC) -o client client.o

clean:
	rm -f *.o core.* server client

server.o:
	$(CC) -c server.c

client.o:
	$(CC) -c client.c
