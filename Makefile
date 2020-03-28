CC=gcc
CFLAGS=-o
LIBS=-pthread

all: server.out 

server.out:
	$(CC) $(CFLAGS) $(LIBS) -o server.out server.o

client.out: 
	$(CC) $(CFLAGS) $(LIBS) -o client.out client.o string.o

clean:
	rm *.o *.out
