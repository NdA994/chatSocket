all: server.out 

server.out:
	gcc -o server.out server.c -pthread

client.out:
	gcc -o client.out client.c -pthread

clean:
	rm *.out
