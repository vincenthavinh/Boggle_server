server:	server.c server.h
	gcc -g -Wall -pthread -lpthread server.c -o server

clean:
	rm -f server