#ifndef SERVER_H
#define SERVER_H

#define TAILLE_GRILLE 16
#define MAX_CLIENTS 3


typedef struct {
	pthread_t clients[MAX_CLIENTS];
	boolean busy[MAX_CLIENTS];
	int clients_co;
	pthread_t server;

} threads_pool;

/*options ligne de commande*/
int port = 2018;
int nb_grilles = 1;
int n_tours = 1;
char** grilles;
boolean immediat = FALSE;
boolean opt_grilles = FALSE;

/*variables globales (dont semaphores)*/
threads_pool threads = { {0}, {0}, 0, 0};

/*========== Fonctions ==========*/

void parse_command_line(int argc, char* argv[]);
void print_args();
int init_socket(int port);
void handling_clients_loop(int socket_desc_server);

#endif