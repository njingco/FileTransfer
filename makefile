CC=gcc -Wall -g
CLIB = -lpthread

all: server client

server: server.o child.o utilities.o
	$(CC) -o server server.o child.o utilities.o $(CLIB)

client: client.o
	$(CC) -o client client.o

clean:
	rm -f *.o core.* server client

server.o:
	$(CC) -c server.c

client.o:
	$(CC) -c client.c

child.o:
	$(CC) -c child.c

utilities.o:
	$(CC) -c utilities.c