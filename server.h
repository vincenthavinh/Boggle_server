#ifndef SERVER_H
#define SERVER_H

#define TAILLE_GRILLE 16

#define MAX_CLIENTS 3
#define BUF_SIZE	1024

/*========== Definitions de Structure ==========*/

typedef enum { FALSE, TRUE } boolean;

/*options ligne de commande*/
int port = 2018;
int nb_grilles = 1;
int n_tours = 1;
char** grilles;
boolean immediat = FALSE;
boolean opt_grilles = FALSE;

/*========== Fonctions ==========*/

void parse_command_line(int argc, char* argv[]);

void print_args();

int init_socket(int port);

void handling_clients_loop(int socket_desc_server);

void* client_handler(void* socket_desc_client);

#endif