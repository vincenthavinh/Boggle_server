#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

//./server -g 2 ABCDEFGHIJKLMNOP KGROJFUNTZOLKSUE -t 2 -p 2010


void parse_command_line(int argc, char* argv[]){
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
				n_tours = atoi(argv[i]);
				//printf("-t %d\n", n_tours);
			}

			/*OPTION GRILLES*/
			else if (strcmp(argv[i],"-grilles") == 0 || 
				strcmp(argv[i],"--grilles") == 0 ||
				strcmp(argv[i],"-g") == 0)
			{
				opt_grilles = TRUE;

				i++;
				n_grilles = atoi(argv[i]);

				//on alloue le pointeur grilles.
				grilles = (char**) malloc (n_grilles * sizeof(char*));
				
				int j;
				for(j=0; j<n_grilles; j++){
					//on alloue la grille (char*).
					grilles[j] = (char*) malloc (TAILLE_GRILLE * sizeof(char));

					//on copie les n_grilles arguments suivants dans les cases du tableau grilles.
					i++;
					if(strlen(argv[i])!=16){
						printf("\nERREUR: grille %s de taille %zd\n",argv[i], strlen(argv[i]));
						exit(1);
					}
					strncpy(grilles[j], argv[i], TAILLE_GRILLE);
				}

				/*printf("-t %d ", n_grilles);
				int k; 
				for(k=0; k<n_grilles; k++) 
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
	printf("port : %d\ntours : %d\nimmediat: %d\nopt_grilles: %d\ngrilles: %d\n", 
		port, n_tours, immediat, opt_grilles, n_grilles);
	int i;
	for(i=0; i<n_grilles;i++)
		printf("%.*s ", TAILLE_GRILLE, grilles[i]);
	printf("\n");
}

int init_socket(int port){

	//creation socket
	int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	if(socket_desc == -1){
		perror("socket()");
		exit(errno);
	}
	
	//socket address in structure
	struct sockaddr_in sin_server = { 0 };

	sin_server.sin_addr.s_addr = htonl(INADDR_ANY);
	sin_server.sin_port = htons(port);
	sin_server.sin_family = AF_INET;

	//bind
	if(bind(socket_desc,(struct sockaddr*) &sin_server, sizeof(sin_server)) == -1){
		perror("bind()");
		exit(errno);
	}

	//listen
	if(listen(socket_desc, MAX_CLIENTS) == -1){
		perror("listen()");
		exit(errno);
	}

	return socket_desc;
}

int main(int argc, char* argv[]) {
	
	parse_command_line(argc, argv);

	print_args(); //pour l'instant ne marche qu'avec des chaines precisees en arg.

	//creation socket d'ecoute du serveur.
	int socket_desc_server = init_socket(port);

	//settings client
  struct sockaddr_in sin_client = { 0 };
  size_t sin_client_size = sizeof(sin_client);
  pthread_t thread_id;
	int socket_desc_client;
  
  //boucle d'ecoute
  //accept() est bloquant => on execute le corps del a boucle quand un client arrive
  while( (socket_desc_client = accept(socket_desc_server, 
  		(struct sockaddr*) &sin_client, (socklen_t*) &sin_client_size)) ){
  	
  	if(socket_desc_client == -1) {
  		perror("accept()");
  		continue;
  	}

  	//creation du pthread du client
		if( pthread_create( &thread_id , NULL ,  client_handler , 
					(void*) &socket_desc_client) != 0)
        {
            perror("pthread_create()");
            return 1;
        }

  }

	return 0;
}

void* client_handler(void* socket_desc) {
	printf("\nclient connecte!\n");
	return NULL;
}