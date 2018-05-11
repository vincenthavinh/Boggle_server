#include "../headers/global.h"

void* client_handler(void* slot_client) {
	int slot = *(int*) slot_client;
	
	printf("%d: client %d connecte!\n",slot,  slot);

	char buffer_in[BUF_SIZE] = { 0 }; //buffer d'entree
	int bytes_in = 0; //bytes ecrites dans le buffer_in
	FILE* dico = fopen(DICO_FILENAME, "r"); //acces au dico pour verifier les mots.

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
			//comm_trouve(slot, mot, traj, dico);
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

	if (bytes_in == -1) {
		printf("ERREUR recv: %d\n", bytes_in);
		perror("recv");
	}
	else if (bytes_in == 0) {
		/*sur deconnexion propre du client:*/
		printf("CO FINIE client %d, close socket %d\n", slot, clients[slot]->sock);
		close(clients[slot]->sock);

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

void comm_trouve(int slot, char* mot, char* traj, FILE* dico){
	char ligne[17] = { 0 };

	//boucle de verification de la trajectoire
	boolean traj_valide = FALSE;
	boolean mot_valide = FALSE;

	//check bonne taille trajectoire/mot
	if(strlen(traj) != 2*strlen(mot)){
		printf("POS taille traj\n");
		return;
	}
	else{

		//boucle de verification de la trajectoire
		int i;
		for(i=0; i<strlen(mot); i++){
			//printf("char: %c, traj: %c%c,\n", mot[i], traj[i*2], traj[i*2+1]);
			int x = traj[i*2+1] - '1';
			int y = traj[i*2] - 'A';
			int index = COTE_GRILLE * y + x;
			if( !(0<=x && x<COTE_GRILLE) || !(0<=y && y<COTE_GRILLE) ){
				printf("POS outofbounds :%c%c, ",traj[i*2], traj[i*2+1]);
				printf("index: %d, x: %d, y: %d\n", index, x, y);
				return;
			}
			if( mot[index] != game->grille_act[index]){
				printf("POS %c%c: %c != %c\n", traj[i*2], 
					traj[i*2+1], mot[index], game->grille_act[index] );
			}
		}

		//TROUVE/TRIDENT/C2B1A2A3B2C1D2/
		//TROUVE/TRIDENTDZZD/C2B1A2A3B2C3D2/
		//TROUVE/TRIDENT/C2B1A2A3B2CZDOZJF3D2/

		//boucle de verification du mot dans le dico
		rewind(dico);
		while (!feof(dico) && mot_valide==FALSE){
			fgets(ligne, TAILLE_GRILLE-1, dico);
			char* newline = strchr(ligne, '\n');
			if (newline != NULL) *newline = '\0';

			if(strcmp(mot, ligne) == 0)	mot_valide = TRUE;
		}
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