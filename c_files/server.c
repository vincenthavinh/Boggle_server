#include "../headers/global.h"

int main(int argc, char* argv[]) {
	parse_command_line(argc, argv);

	print_args(); //pour l'instant ne marche qu'avec des chaines precisees en arg.

	//creation socket d'ecoute du serveur.
	int sock_server = init_socket(port);

	//settings client
	struct sockaddr_in sin_client = { 0 };
	size_t sin_client_size = sizeof(sin_client);
	pthread_t thread_id;

	/*initialisation du tableau de clients et sa semaphore d'acces*/
	init_clients();

	/*initialisation des donnees du jeu*/
	init_game();

	//creation du pthread du jeu
	if( pthread_create( &thread_id , NULL ,  game_handler , NULL) != 0){
		perror("pthread_create()");
	}

	//boucle d'ecoute
	while(1){
		
		/*si un slot client est disponible (bloquant)*/
		sem_wait(slots_clients);

		//accept() est bloquant => on execute la suite quand un client arrive
		int sock_client = accept(sock_server, 
			(struct sockaddr*) &sin_client, (socklen_t*) &sin_client_size);

		if(sock_client == -1) {
			perror("accept() returned -1");
			continue;
		}

		//stockage et reinitialisation du client dans le tableau de clients* */
		int num_client = ajout_client(sock_client);

		//creation du pthread du client
		if( pthread_create( &thread_id , NULL ,  client_handler , 
					(void*) &num_client) != 0){
			perror("pthread_create()");
			continue;
		}


	}

	return 0;
}

void init_clients(){
	slots_clients = (sem_t*) malloc (sizeof(sem_t));
	sem_init(slots_clients, 0, MAX_CLIENTS);

	int i;
	for(i=0; i<MAX_CLIENTS; i++){
		clients[i] = (client*) malloc (sizeof(client));

		clients[i]->is_co = FALSE;
		clients[i]->is_ready = FALSE;
		clients[i]->sock = -1;
		clients[i]->score = 0;
		clients[i]->list_prop = NULL;

		clients[i]->user = (char*) calloc (TAILLE_USER, sizeof(char));
	}
}

int ajout_client(int sock_client){
	int i;
	for(i=0; i<MAX_CLIENTS; i++){

		//pthread_rwlock_rdlock(game->rwlock);
		//pthread_rwlock_wrlock(clients[i]->rwlock);

		if(clients[i]->is_co == FALSE){
			clients[i]->sock = sock_client;
			clients[i]->is_co = TRUE;
			clients[i]->is_ready = FALSE;
			clients[i]->score = 0;
			memset(clients[i]->user, '\0', TAILLE_USER);
			break;
		}

		//pthread_rwlock_unlock(clients[i]->rwlock);
		//pthread_rwlock_unlock(game->rwlock);

	}
	return i;
}

void init_game(){
	game = (boggle_game*) malloc (sizeof(boggle_game));
	game->tour_act = 0;

	if(opt_grilles == FALSE){
		game->grille_act = (char*) calloc (TAILLE_GRILLE, sizeof(char));
	}

	game->event = (pthread_cond_t*) malloc (sizeof(pthread_cond_t));
	pthread_cond_init(game->event, NULL);

	game->mutex_timer = (pthread_mutex_t*) malloc (sizeof(pthread_mutex_t));
	pthread_mutex_init(game->mutex_timer, NULL);

	game->mutex_clients = (pthread_mutex_t*) malloc (sizeof(pthread_mutex_t));
	pthread_mutex_init(game->mutex_clients, NULL);


}

int init_socket(int port){

	//creation socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock == -1){
		perror("socket()");
		exit(errno);
	}
	
	//socket address in structure
	struct sockaddr_in sin_server = { 0 };

	sin_server.sin_addr.s_addr = htonl(INADDR_ANY);
	sin_server.sin_port = htons(port);
	sin_server.sin_family = AF_INET;

	//bind
	if(bind(sock,(struct sockaddr*) &sin_server, sizeof(sin_server)) == -1){
		perror("bind()");
		exit(errno);
	}

	//listen
	if(listen(sock, MAX_CLIENTS) == -1){
		perror("listen()");
		exit(errno);
	}

	return sock;
}

void parse_command_line(int argc, char* argv[]){
	/*initialisiation par defaut*/
	port = 2018;
	nb_grilles = 1;
	nb_tours = 1;

	immediat = FALSE;
	opt_grilles = FALSE;

	/*initialisation selon ligne de commande*/
	if(argc > 1){

		int i;
		for (i = 1; i < argc; i++) {
			//printf("%d %s\n", i, argv[i]);

			/*OPTION PORT*/
			if (strcmp(argv[i],"-port") == 0 || 
				strcmp(argv[i],"--port") == 0 ||
				strcmp(argv[i],"-p") == 0)
			{
				i++;
				port = atoi(argv[i]);
				//printf("-p %d\n", port);
			}

			/*OPTION TOURS*/
			else if (strcmp(argv[i],"-tours") == 0 || 
				strcmp(argv[i],"--tours") == 0 ||
				strcmp(argv[i],"-t") == 0)
			{
				i++;
				nb_tours = atoi(argv[i]);
				//printf("-t %d\n", nb_tours);
			}

			/*OPTION GRILLES*/
			else if (strcmp(argv[i],"-grilles") == 0 || 
				strcmp(argv[i],"--grilles") == 0 ||
				strcmp(argv[i],"-g") == 0)
			{
				opt_grilles = TRUE;

				i++;
				nb_grilles = atoi(argv[i]);

				//on alloue le pointeur grilles.
				grilles = (char**) malloc (nb_grilles * sizeof(char*));
				
				int j;
				for(j=0; j<nb_grilles; j++){
					i++;
					if(strlen(argv[i])!=16){
						printf("\nERREUR: grille %s de taille %zd\n",argv[i], strlen(argv[i]));
						exit(1);
					}
					//on copie la chaine passee dans argv[i] dans grilles[j].
					grilles[j] = strndup(argv[i], TAILLE_GRILLE);
				}
			}

			/*OPTION IMMEDIAT*/
			else if (strcmp(argv[i],"-immediat") == 0 || 
				strcmp(argv[i],"--immediat") == 0 ||
				strcmp(argv[i],"-i") == 0)
			{
				//printf("-i\n");
				immediat = TRUE;
			}

			/*ERREUR*/
			else
			{
				printf("ERREUR : %s non reconnu", argv[i]);
			}
		}
	}
}

void print_args(){
	printf("========== PARAMETRES SERVEUR ==========\n");
	printf("port : %d\ntours : %d\nimmediat: %s\ngrilles: %s\n", 
		port, nb_tours, ((immediat==TRUE)?"oui":"non"), (opt_grilles?"fixes":"aleatoires"));
	int i;
	if(opt_grilles){
		printf("nb_grilles: %d\n", nb_grilles);
		for(i=0; i<nb_grilles;i++)
			printf("%.*s ", TAILLE_GRILLE, grilles[i]);
	}
	printf("temps par tour: %d sec\ntemps des pauses: %d sec\n", TEMPS_TOUR, TEMPS_PAUSE);

	printf("\n========================================\n\n");
}