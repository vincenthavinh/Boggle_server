#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <pthread.h>
#include <semaphore.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*========== macros (constantes) ==========*/

#define DICO_FILENAME "./dico_full_format.txt"

#define TAILLE_GRILLE 17
#define COTE_GRILLE 4
#define MAX_CLIENTS 3

#define TAILLE_USER 50
#define TAILLE_TRAJ 33

#define BUF_SIZE 1024

#define TEMPS_TOUR 180
#define TEMPS_PAUSE 10

/*========== Definitions de Structures ==========*/

typedef enum { FALSE, TRUE } boolean;

typedef struct propos_struct propos; 
struct propos_struct{
	char* mot;
	propos* next;
};

typedef struct {
	boolean is_co;
	boolean is_ready;	
	int sock;
	char* user;
	int score;
	propos* list_prop;
} client;

typedef struct {
	int tour_act;
	char* grille_act;
	pthread_cond_t* event;
	pthread_mutex_t* mutex_timer;
	pthread_mutex_t* mutex_clients;
} boggle_game;

/*========== include headers custom ==========*/

#include "../headers/server.h"
#include "../headers/thread_client.h"
#include "../headers/thread_game.h"


/*========== Var Globales Partagees ==========*/

client* clients[MAX_CLIENTS];
sem_t* slots_clients;

boggle_game* game;




/*========== options ligne de commande =========*/

/*ces variables ne sont affectees qu'une fois, ensuite elles seront
 *seulement accedees en lecture: pas de probleme de concurrence*/
int port;
int nb_grilles;
int nb_tours;
char** grilles;
boolean immediat;
boolean opt_grilles;

#endif