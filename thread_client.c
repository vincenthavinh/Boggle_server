#include "global.h"
#include "thread_client.h"

int recv_message(int sock, char buffer[]){
	/*Reception d'un message du client dans le buffer.
	 *On remplace le '\n' de terminaison de commande par '\0'*/
	int bytes_in = recv(sock, buffer, BUF_SIZE-1, 0);
	if (bytes_in <=0) return bytes_in;

	char* newline = memchr(buffer, '\n', bytes_in);
	if (newline == NULL) {
		printf("ERREUR memchr : pas de '\\n' en fin de message.");
		printf("msg : [%.*s]", bytes_in, buffer);
		return bytes_in;
	}
	*newline = '\0';

	return bytes_in;
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

	if(strncmp(buffer + i_start, "CONNEXION", cmp_length) == 0)
		return CONNEXION;
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

void comm_bienvenue(char* buffer_in,int i_start, char* buffer_out, int slot){
	int i_slash = index_of_slash(buffer_in + i_start);

	pthread_rwlock_wrlock(clients[slot]->rwlock);
	//LOCK ECRITURE (et lecture)

	if(clients[slot]->is_ready == TRUE){
		printf("erreur: CONNEXION/ deja recu pour ce client.\n");
		pthread_rwlock_unlock(clients[slot]->rwlock);
		return;
	}

	//on enregistre le pseudo de l'user, et on le passe en etat ready
	clients[slot]->is_ready = TRUE;
	memcpy(clients[slot]->user, buffer_in + i_start, i_slash);
	
	//UNLOCK
	pthread_rwlock_unlock(clients[slot]->rwlock);

	/*reponse BIENVENUE*/
	strcat(buffer_out, "BIENVENUE/");

	pthread_rwlock_rdlock(game->rwlock);
	//LOCK LECTURE

	strcat(buffer_out, grilles[game->tour_act]);
	strcat(buffer_out, "/");
	char buffer_int[12];
	snprintf(buffer_int, 12, "%d", game->tour_act);

	//UNLOCK
	pthread_rwlock_unlock(game->rwlock);

	strcat(buffer_out, buffer_int);

	int i;
	for(i=0; i<MAX_CLIENTS; i++){
		pthread_rwlock_rdlock(clients[i]->rwlock);
		//LOCK LECTURE

		if(clients[i]->is_ready == TRUE){
			strcat(buffer_out, "*");
			strcat(buffer_out, clients[i]->user);
			strcat(buffer_out, "*");
			snprintf(buffer_int, 12, "%d", clients[i]->score);
			strcat(buffer_out, buffer_int);
		}

		//UNLOCK
		pthread_rwlock_unlock(clients[i]->rwlock);

	}
	strcat(buffer_out, "/\n");

	send(clients[slot]->sock, buffer_out, strlen(buffer_out), 0);
	buffer_out[0] = '\0';

	/*reponse CONNECTE*/
	strcat(buffer_out, "CONNECTE/");

	pthread_rwlock_rdlock(clients[slot]->rwlock);
	//LOCK LECTURE

	strcat(buffer_out, clients[slot]->user);

	//UNLOCK
	pthread_rwlock_unlock(clients[slot]->rwlock);
	strcat(buffer_out, "/\n");

	for(i=0; i<MAX_CLIENTS; i++){
		pthread_rwlock_rdlock(clients[i]->rwlock);
		//LOCK LECTURE

		if(i != slot && clients[i]->is_ready == TRUE){
			send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
		}
		//UNLOCK
		pthread_rwlock_unlock(clients[i]->rwlock);
	}
	buffer_out[0]='\0';
}

void* client_handler(void* slot_client) {
	int slot = *(int*) slot_client;
	
	printf("\n%d: client %d connecte!\n",slot,  slot);

	char buffer_in[BUF_SIZE] = { 0 }; //buffer d'entree
	char buffer_out[BUF_SIZE] = { 0 }; //buffer de sortie
	int bytes_in = 0; //bytes ecrites dans le buffer_in

	while (1)
	{
		/*reception du message*/
		bytes_in = recv_message(clients[slot]->sock, buffer_in);
		if (bytes_in <=0) break;

		/*affichage du message*/
		printf("TAILLE : %d, MSG: %s\n", bytes_in, buffer_in);

		/*parsing du message*/
		int i_start = 0;
		int i_slash = index_of_slash(buffer_in);
		if((i_slash)==-1) continue;

		comm_client comm = get_command(buffer_in, i_start, i_slash);

		i_start = i_slash +1;
		//printf("%.*s\n", i_slash, buffer + i_start );

		switch(comm){

			case CONNEXION:
				printf("case CONNEXION\n");
				comm_bienvenue(buffer_in, i_start , buffer_out, slot);
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

	if (bytes_in == -1) {
		printf("ERREUR recv: %d\n", bytes_in);
		perror("recv");
	}
	else if (bytes_in == 0) {
		/*sur deconnexion propre du client:*/
		printf("CO FINIE recv: %d\n", bytes_in);

		//on incremente la semaphore de slots clients disponibles
		sem_post(slots_clients);

		//
	}

	return NULL;
}