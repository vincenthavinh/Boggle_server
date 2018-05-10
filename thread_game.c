#include "global.h"
#include "thread_game.h"

void* game_handler(){

	printf("before wait\n");
	pthread_mutex_lock(game->mutex);
	pthread_cond_wait(game->event, game->mutex);
	printf("after wait\n");


	while(1)
    {
        /*en Attente de tour_fini OU commande TROUVE*/
        pthread_mutex_lock(game->mutex);
        while (game->tour_fini == FALSE && game->client == -1)
            pthread_cond_wait(game->event, game->mutex);
        
        if(game->tour_fini == TRUE){
        	printf("TOUR FINI\n");
        }else{
        	printf("mot trouve: %s\n", clients[game->client]->mot);
        }

        game->client = -1;
        pthread_mutex_unlock(game->mutex);
    }

	return NULL;
}