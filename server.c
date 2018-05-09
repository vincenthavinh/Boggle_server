#include "global.h"
#include "server.h"
#include "thread_client.h"

//./server -g 2 ABCDEFGHIJKLMNOP KGROJFUNTZOLKSUE -t 2 -p 2010


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

				/*printf("-t %d ", nb_grilles);
				int k; 
				for(k=0; k<nb_grilles; k++) 
					printf("%.*s ", TAILLE_GRILLE, grilles[k]);
				printf("\n");*/
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
	printf("port : %d\ntours : %d\nimmediat: %s\ngrilles: %s\nnb_grilles: %d\n", 
		port, nb_tours, (immediat?"oui":"non"), (opt_grilles?"fixes":"aleatoires"), nb_grilles);
	int i;
	for(i=0; i<nb_grilles;i++)
		printf("%.*s ", TAILLE_GRILLE, grilles[i]);
	printf("\n========================================\n\n");
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

void init_clients(){
	slots_clients = (sem_t*) malloc (sizeof(sem_t));
	sem_init(slots_clients, 0, MAX_CLIENTS);

	int i;
	for(i=0; i<MAX_CLIENTS; i++){
		clients[i] = (client*) malloc (sizeof(client));

		clients[i]->is_co = FALSE;
		clients[i]->sock = -1;
		clients[i]->score = 0;

		clients[i]->user = (char*) malloc (TAILLE_USER * sizeof(char));
		clients[i]->mot = (char*) malloc (TAILLE_MOT * sizeof(char));
		memset(clients[i]->user, '\0', TAILLE_USER);
		memset(clients[i]->mot, '\0', TAILLE_MOT);

		clients[i]->mutex = (pthread_mutex_t*) malloc (sizeof(pthread_mutex_t));
		pthread_mutex_init(clients[i]->mutex, NULL);
	}
}

void handling_clients_loop(int sock_server){
	//settings client
	struct sockaddr_in sin_client = { 0 };
	size_t sin_client_size = sizeof(sin_client);
	pthread_t thread_id = NULL;

	/*initialisation du tableau de clients et sa semaphore d'acces*/
	init_clients();

	int a;
	sem_getvalue(slots_clients, &a);
	printf("CAPACITE : %d\n", a);
	/*pthread_mutex_lock(clients[0]->mutex);
	pthread_mutex_unlock(clients[0]->mutex);*/

	//boucle d'ecoute
	while(1){
		
		/*si un slot client est disponible (bloquant)*/
		sem_wait(slots_clients);

		//accept() est bloquant => on eexecute la suite quand un client arrive
		int sock_client = accept(sock_server, 
			(struct sockaddr*) &sin_client, (socklen_t*) &sin_client_size);

		if(sock_client == -1) {
			perror("accept() returned -1");
			continue;
		}

		int i;
		for(i=0; i<MAX_CLIENTS; i++){
			pthread_mutex_lock(clients[i]->mutex);
			if(clients[i]->is_co == FALSE){
				clients[i]->sock = sock_client;
			}
			pthread_mutex_unlock(clients[i]->mutex);
		}

		//creation du pthread du client
		if( pthread_create( &thread_id , NULL ,  client_handler , 
					(void*) &sock_client) != 0){
			perror("pthread_create()");
			continue;
		}


	}
}

int main(int argc, char* argv[]) {
	
	parse_command_line(argc, argv);

	print_args(); //pour l'instant ne marche qu'avec des chaines precisees en arg.

	//creation socket d'ecoute du serveur.
	int sock_server = init_socket(port);

	handling_clients_loop(sock_server);

	return 0;
}