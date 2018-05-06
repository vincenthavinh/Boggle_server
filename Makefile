server:	server.c thread_client.c thread_client.h server.h
	gcc -g -Wall -pthread -lpthread server.c thread_client.c -o server

clean:
	rm -f server