server:	server.c thread_client.c thread_game.c thread_client.h server.h global.h thread_game.h
	gcc -g -Wall -pthread -lpthread server.c thread_client.c thread_game.c -o server

clean:
	rm -f server