#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "server.h"

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
				printf("-p %d\n", port);
			}

			/*OPTION TOURS*/
			else if (strcmp(argv[i],"-tours") == 0 || 
				strcmp(argv[i],"--tours") == 0 ||
				strcmp(argv[i],"-t") == 0)
			{
				i++;
				n_tours = atoi(argv[i]);
				printf("-t %d\n", n_tours);
			}

			/*OPTION GRILLES*/
			else if (strcmp(argv[i],"-grilles") == 0 || 
				strcmp(argv[i],"--grilles") == 0 ||
				strcmp(argv[i],"-g") == 0)
			{
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

				printf("-t %d ", n_grilles);
				int k; 
				for(k=0; k<n_grilles; k++) 
					printf("%.*s ", TAILLE_GRILLE, grilles[k]);
				printf("\n");
			}

			/*OPTION IMMEDIAT*/
			else if (strcmp(argv[i],"-immediat") == 0 || 
				strcmp(argv[i],"--immediat") == 0 ||
				strcmp(argv[i],"-i") == 0)
			{
				printf("-i\n");
			}

			/*ERREUR*/
			else
			{
				printf("ERREUR : %s non reconnu", argv[i]);
			}

		}
	}
}

int main(int argc, char* argv[]) {
	
	parse_command_line(argc, argv);

/*while (strcmp(check,input) != 0)*/
	//int socket = create_socket();

	return 0;
}