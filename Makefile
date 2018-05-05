server:	server.c server.h
	gcc -g -Wall -lpthread server.c -o server

clean:
	rm -f server