#ifndef SERVER_H
#define SERVER_H

#define TAILLE_GRILLE 16

/*options ligne de commande*/

int port = 2018;
int n_grilles = 1;
int n_tours = 1;
char** grilles;

/*========== Fonctions ==========*/

void parse_command_line(int argc, char* argv[]);

#endif