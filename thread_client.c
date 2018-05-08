#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "global.h"
#include "thread_client.h"

int recv_message(int sock, char buffer[]){
	/*Reception d'un message du client dans le buffer.
	 *On remplace le '\n' de terminaison de commande par '\0'*/
	int bytes_rec = recv(sock, buffer, BUF_SIZE-1, 0);
	if (bytes_rec <=0) return bytes_rec;

	char* newline = memchr(buffer, '\n', bytes_rec);
	if (newline == NULL) {
		perror("memchr on recv_message");
		exit(1);
	}
	*newline = '\0';

	return bytes_rec;
}

int index_of_slash(char buffer[]) {
		char* slash = strchr(buffer, '/');
		if (slash == NULL) {
			printf("strchr renvoie pointeur sur null: pas de '/' trouve\n");
			return -1;
		}
		int i_slash = (int)(slash - buffer);
		return i_slash;
}

/*i_start inclus, i_end exclu dans la comparaison de chaine*/
comm_client get_command(char buffer[], int i_start, int i_end) {
	int cmp_length = i_end - i_start;

	if(strncmp(buffer + i_start, "CONNECTION", cmp_length) == 0)
		return CONNECTION;
	else if(strncmp(buffer + i_start, "SORT", cmp_length) == 0)
		return SORT;
	else if(strncmp(buffer + i_start, "TROUVE", cmp_length) == 0)
		return TROUVE;
	else if(strncmp(buffer + i_start, "ENVOI", cmp_length) == 0)
		return ENVOI;
	else if(strncmp(buffer + i_start, "PENVOI", cmp_length) == 0)
		return PENVOI;

	return non_reconnu;
}

void add_to_message(char buffer[], int len) {

}

void send_message(int sock, char* comm, char* arg1, char* arg2) {

}

void* client_handler(void* sock_client) {
	int int_client = *(int*)sock_client;
	
	printf("\n%d: client %d connecte!\n",int_client,  int_client);

	char buffer_in[BUF_SIZE] = { 0 };
	char buffer_out[BUF_SIZE] = { 0 };
	int bytes_rec;

	while (1)
	{
		/*reception du message*/
		bytes_rec = recv_message(int_client, buffer_in);
		if (bytes_rec <=0) break;

		/*affichage du message*/
		printf("TAILLE : %d, MSG: %s\n", bytes_rec, buffer_in);

		/*parsing du message*/
		int i_start = 0;
		int i_slash = index_of_slash(buffer_in);
		if((i_slash)==-1) continue; 

		comm_client comm = get_command(buffer_in, i_start, i_slash);

		i_start = i_slash +1;
		i_slash = index_of_slash(buffer_in + i_start);
		//printf("%.*s\n", i_slash, buffer + i_start );

		switch(comm){

			case CONNECTION:
				printf("case CONNECTION\n");
				break;

			case SORT:
				printf("case SORT\n");
				break;

			case TROUVE:
				printf("case TROUVE\n");
				break;

			case ENVOI:
				printf("case ENVOI\n");
				break;

			case PENVOI:
				printf("case PENVOI\n");
				break;

			case non_reconnu:
				printf("probleme COMMANDE non reconnue: %.*s\n", i_slash, buffer_in + i_start );
				break;
		}
	}

	if (bytes_rec == -1) {
		printf("ERREUR recv: %d\n", bytes_rec);
		perror("recv");
	}
	else if (bytes_rec == 0) {
		printf("CO FINIE recv: %d\n", bytes_rec);
	}

	return NULL;
}