#include "../headers/global.h"

void* client_handler(void* slot_client) {
	int slot = *(int*) slot_client;
	
	printf("%d: client %d connecte!\n",slot,  slot);

	char buffer_in[BUF_SIZE] = { 0 }; //buffer d'entree
	int bytes_in = 0; //bytes ecrites dans le buffer_in

	while (1)
	{
		/*reception du message*/
		int bytes_in = recv(clients[slot]->sock, buffer_in, BUF_SIZE-1, 0);
		if (bytes_in <=0) break;

		/*affichage du message*/
		printf("%d: %s", slot, buffer_in);
		fflush(stdout);

		/*parsing du message*/
		char* reste = buffer_in;
		char* commande = strtok_r(reste, "/", &reste);

		/*CAS CONNEXION*/
		if(strcmp(commande, "CONNEXION") == 0){
			//printf("%d: case CONNEXION\n", slot);
			char* user = strtok_r(NULL, "/", &reste);
        	comm_connexion(slot, user);

		}
		/*CAS SORT*/
		else if(strcmp(commande, "SORT") == 0){
			//printf("%d: case SORT\n", slot);
			comm_sort(slot);
			bytes_in = 0;
			break;

		}
		/*CAS TROUVE*/
		else if(strcmp(commande, "TROUVE") == 0){
			//printf("%d: case TROUVE\n", slot);
			char* mot = strtok_r(NULL, "/", &reste);
			char* traj = strtok_r(NULL, "/", &reste);
			comm_trouve(slot, mot, traj);
		}
		/*CAS ENVOI*/
		else if(strcmp(commande, "ENVOI") == 0){
			//printf("%d: case TROUVE\n", slot);
			char* message = strtok_r(NULL, "/", &reste);
			comm_envoi(slot, message);

		}
		/*CAS PENVOI*/
		else if(strcmp(commande, "PENVOI") == 0){
			//printf("%d: case PENVOI\n", slot);
			char* user = strtok_r(NULL, "/", &reste);
			char* message = strtok_r(NULL, "/", &reste);
			comm_penvoi(slot, message, user);

		}
		/*COMMANDE NON TROUVEE*/
		else{
			printf("%d: probleme COMMANDE non reconnue: %s\n", slot, commande);
		}

		memset(buffer_in, '\0', BUF_SIZE);
	}

	if(bytes_in == 0) {
		/*sur deconnexion propre du client:*/
		printf("CO FINIE client %d, close socket %d\n", slot, clients[slot]->sock);
		close(clients[slot]->sock);

		/*on supprime sa liste de propositions (desallocation)*/
		supp_all_props(&(clients[slot]->list_prop));

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
		printf("%d: erreur, CONNEXION/ deja recu pour ce client.\n", slot);
		return;
	}

	char buffer_out[BUF_SIZE] = { 0 };

	//on enregistre le pseudo de l'user, et on le passe en etat ready
	clients[slot]->is_ready = TRUE;
	strcpy(clients[slot]->user, user);

	//message BIENVENUE
	strcat(buffer_out, "BIENVENUE/");
	strcat(buffer_out, game->grille_act);
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
	strcat(buffer_out, "DECONNEXION/");
	strcat(buffer_out, clients[slot]->user);
	strcat(buffer_out, "/\n");

	int i;
	for(i=0; i<MAX_CLIENTS; i++){
		if(i != slot && clients[i]->is_ready == TRUE){
			send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
		}
	}
}

void comm_trouve(int slot, char* mot, char* traj){
	char raison[60] = { 0 };

	boolean prop_valide = est_valide(slot, mot, traj, raison);

	char buffer_out[BUF_SIZE] = { 0 };

	//envoi du message conditionnel
	if(prop_valide == TRUE){
		//on stocke la proposition dans la liste des propositions du client.
		ajout_prop(&(clients[slot]->list_prop) , mot);

		//message MVALIDE
		strcat(buffer_out, "MVALIDE/");
		strcat(buffer_out, mot);
		strcat(buffer_out, "/\n");
		send(clients[slot]->sock, buffer_out, strlen(buffer_out), 0);
	}
	else{
		//message MINVALIDE
		strcat(buffer_out, "MINVALIDE/");
		strcat(buffer_out, raison);
		strcat(buffer_out, "/\n");
		send(clients[slot]->sock, buffer_out, strlen(buffer_out), 0);
	}
}

void comm_envoi(int slot, char* message){
	char buffer_out[BUF_SIZE] = { 0 };

	//message RECEPTION
	buffer_out[0] = '\0';
	strcat(buffer_out, "RECEPTION/");
	strcat(buffer_out, message);
	strcat(buffer_out, "/");
	strcat(buffer_out, clients[slot]->user);
	strcat(buffer_out, "/\n");

	int i;
	for(i=0; i<MAX_CLIENTS; i++){
		if(clients[i]->is_ready == TRUE){
			send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
		}
	}
}

void comm_penvoi(int slot, char* message, char* user){
	char buffer_out[BUF_SIZE] = { 0 };

	//message PRECEPTION
	buffer_out[0] = '\0';
	strcat(buffer_out, "PRECEPTION/");
	strcat(buffer_out, message);
	strcat(buffer_out, "/");
	strcat(buffer_out, clients[slot]->user);
	strcat(buffer_out, "/\n");

	int i;
	for(i=0; i<MAX_CLIENTS; i++){
		if(clients[i]->is_ready == TRUE){
			if(strcmp(clients[i]->user, user) == 0)
			send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
		}
	}
}