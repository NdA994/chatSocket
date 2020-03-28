CC=gcc
CFLAGS=-o
LIBS=-pthread

all: server.out 

server.out:
	gcc -o server.out server.c -pthread

clean:
	rm *.out
