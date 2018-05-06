#ifndef STRUCTURES_H
#define STRUCTURES_H

/*========== Definitions de Structures ==========*/

typedef enum { FALSE, TRUE } boolean;

typedef enum { 
	CONNECTION,
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

#endif