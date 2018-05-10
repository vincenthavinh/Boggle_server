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

#define TAILLE_GRILLE 16
#define MAX_CLIENTS 3

#define TAILLE_USER 50
#define TAILLE_MOT 17
#define TAILLE_TRAJ 35

#define BUF_SIZE 1024

/*========== Definitions de Structures ==========*/

typedef enum { FALSE, TRUE } boolean;

typedef enum { 
	CONNEXION,
	SORT,
	TROUVE,
	ENVOI,
	PENVOI,
	non_reconnu
} comm_client;

typedef enum {
	BIENVENUE,
	CONNECTE,
	DECONNEXION,
	SESSION,
	VAINQUEUR,
	TOUR,
	MVALIDE,
	MINVALIDE,
	RFIN,
	BILANMOTS,
	RECEPTION,
	PRECEPTION
} mess_server;

typedef struct {
	boolean is_co;
	boolean is_ready;	
	int sock;
	char* user;
	int score ;
	char* mot;
	char* traj;
} client;

typedef struct {
	int tour_act;
	boolean tour_fini;
	int client;
	pthread_cond_t* event;
	pthread_mutex_t* mutex;
} boggle_game;

/*========== options ligne de commande =========*/

/*ces variables ne sont affectees qu'une fois, ensuite elles seront
 *seulement accedees en lecture*/
int port;
int nb_grilles;
int nb_tours;
char** grilles;
boolean immediat;
boolean opt_grilles;

/*========== autres ==========*/

client* clients[MAX_CLIENTS];
sem_t* slots_clients;

boggle_game* game;

#endif