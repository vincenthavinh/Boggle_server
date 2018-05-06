#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "thread_client.h"

void* client_handler(void* sock_client) {
	int int_client = *(int*)sock_client;
	
	printf("\n%d: client %d connecte!\n",int_client,  int_client);

	char buffer[BUF_SIZE];
	int bytes_rec = 0;
	//char* last_newline;

	while (1)
	{
		/*Reception d'un message du client dans le buffer.
		 *On remplace le '\n' de terminaison de commande par '\0'*/
		bytes_rec = recv(int_client, &buffer, BUF_SIZE-1, 0);
		if (bytes_rec <=0) break;

	  char* last_newline = memchr(buffer, '\n', bytes_rec);
	  if (last_newline == NULL) {
			perror("memchr");
			exit(1);
		}
		*last_newline = '\0';

		/*affichage du message*/
		printf("%s\n", buffer);

		/*parsing du message*/

	}

	if (bytes_rec == -1) {
		perror("recv");
	}
	else if (bytes_rec == 0) {
		// EOS on the socket: close it, exit the thread, etc.
	}

	return NULL;
}