#include "global.h"
#include "thread_client.h"

void* client_handler(void* slot_client) {
	int slot = *(int*) slot_client;
	
	printf("\n%d: client %d connecte!\n",slot,  slot);

	char buffer_in[BUF_SIZE] = { 0 }; //buffer d'entree
	int bytes_in = 0; //bytes ecrites dans le buffer_in

	while (1)
	{
		/*reception du message*/
		int bytes_in = recv(clients[slot]->sock, buffer_in, BUF_SIZE-1, 0);
		if (bytes_in <=0) break;

		/*affichage du message*/
		printf("TAILLE : %d, MSG: %s\n", bytes_in, buffer_in);

		/*parsing du message*/
		char* reste = buffer_in;
		char* commande = strtok_r(reste, "/", &reste);

		if(strcmp(commande, "CONNEXION") == 0){
			printf("case CONNEXION\n");
			/*while ((token = strtok_r(reste, "/", &reste)))
        	printf("TOKEN: %s\n", token);*/
			char* user = strtok_r(NULL, "/", &reste);
        	comm_connexion(slot, user);

		}
		else if(strcmp(commande, "SORT") == 0){
			printf("case SORT\n");
			comm_sort(slot);

		}
		else if(strcmp(commande, "TROUVE") == 0){
			printf("case TROUVE\n");

		}
		else if(strcmp(commande, "ENVOI") == 0){
			printf("case TROUVE\n");

		}
		else if(strcmp(commande, "PENVOI") == 0){
			printf("case PENVOI\n");

		}else{
			printf("probleme COMMANDE non reconnue: %s\n", commande);
		}
	}

	if (bytes_in == -1) {
		printf("ERREUR recv: %d\n", bytes_in);
		perror("recv");
	}
	else if (bytes_in == 0) {
		/*sur deconnexion propre du client:*/
		printf("CO FINIE recv: %d\n", bytes_in);

		/*on passe le client en deconnecte*/
		clients[slot]->is_co = FALSE;
		clients[slot]->is_ready = FALSE;

		//on incremente la semaphore de slots clients disponibles
		sem_post(slots_clients);
	}

	return NULL;
}

void comm_connexion(int slot, char* user){
	if(clients[slot]->is_ready == TRUE){
		printf("erreur: CONNEXION/ deja recu pour ce client.\n");
		return;
	}

	char buffer_out[BUF_SIZE] = { 0 };

	//on enregistre le pseudo de l'user, et on le passe en etat ready
	clients[slot]->is_ready = TRUE;
	strcpy(clients[slot]->user, user);

	//message BIENVENUE
	strcat(buffer_out, "BIENVENUE/");
	strcat(buffer_out, grilles[game->tour_act]);
	strcat(buffer_out, "/");
	sprintf(buffer_out+strlen(buffer_out), "%d", game->tour_act);

	int i;
	for(i=0; i<MAX_CLIENTS; i++){
		if(clients[i]->is_ready == TRUE){
			strcat(buffer_out, "*");
			strcat(buffer_out, clients[i]->user);
			strcat(buffer_out, "*");
			snprintf(buffer_out+strlen(buffer_out), 12, "%d", clients[i]->score);
		}
	}
	strcat(buffer_out, "/\n");
	send(clients[slot]->sock, buffer_out, strlen(buffer_out), 0);

	//message CONNECTE
	buffer_out[0] = '\0';
	strcat(buffer_out, "CONNECTE/");
	strcat(buffer_out, clients[slot]->user);
	strcat(buffer_out, "/\n");

	for(i=0; i<MAX_CLIENTS; i++){
		if(i != slot && clients[i]->is_ready == TRUE){
			send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
		}
	}
}

void comm_sort(int slot){
	char buffer_out[BUF_SIZE] = { 0 };

	//message DECONNEXION
	buffer_out[0] = '\0';
	strcat(buffer_out, "DECONNEXION/");
	strcat(buffer_out, clients[slot]->user);
	strcat(buffer_out, "/\n");

	int i;
	for(i=0; i<MAX_CLIENTS; i++){
		if(i != slot && clients[i]->is_ready == TRUE){
			send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
		}
	}

	buffer_out[0]='\0';
}
