#include "../headers/global.h"

void* game_handler(){

    while(1){

        printf("debut SESSION\n");

        //message SESSION
        msg_session();

        //boucle des tours
        while(game->tour_act < nb_tours){

            //setup grille du tour actuel.
            if(opt_grilles == TRUE){
                game->grille_act = grilles[game->tour_act % nb_grilles];
            }else{
                printf("GENERER GRILLE ALEA A FAIRE\n");
                exit(1);
            }

            //debut du tour
            printf("DEBUT TOUR\n");

            //message TOUR
            msg_tour();

            //lancement du thread timer
            pthread_t pthread_id;
            pthread_create( &pthread_id , NULL ,  timer_tour, NULL);

            //en Attente du signal du timer (bloquant)
            pthread_mutex_lock(game->mutex);
            pthread_cond_wait(game->event, game->mutex);
            pthread_mutex_unlock(game->mutex);

            //Fin du tour
            printf("TOUR FINI\n");

            //message RFIN
            msg_rfin();

            //msg BILANMOTS
            printf("BILANMOTS a faire\n");

            //pause resultats entre 2 tours
            sleep(TEMPS_PAUSE);

            //indice prochain tour
            game->tour_act++;

        }

        printf("VAINQUEUR a faire\n");
        sleep(TEMPS_PAUSE);
    }

    return NULL;
}

void msg_session(){
    char buffer_out[BUF_SIZE] = { 0 };

    //message RFIN
    strcat(buffer_out, "SESSION/\n");

    int i;
    for(i=0; i<MAX_CLIENTS; i++){
        if(clients[i]->is_ready == TRUE){
            send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
        }
    }
}

void msg_tour(){
    char buffer_out[BUF_SIZE] = { 0 };

    //message TOUR
    strcat(buffer_out, "TOUR/");
    strcat(buffer_out, game->grille_act);
    strcat(buffer_out, "/\n");

    int i;
    for(i=0; i<MAX_CLIENTS; i++){
        if(clients[i]->is_ready == TRUE){
            send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
        }
    }
}

void msg_rfin(){
    char buffer_out[BUF_SIZE] = { 0 };

    //message RFIN
    strcat(buffer_out, "RFIN/\n");

    int i;
    for(i=0; i<MAX_CLIENTS; i++){
        if(clients[i]->is_ready == TRUE){
            send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
        }
    }
}

void* timer_tour(){

    sleep(TEMPS_TOUR);

    game->tour_fini = TRUE;

    pthread_mutex_lock(game->mutex);
    pthread_cond_signal(game->event);
    pthread_mutex_unlock(game->mutex);

    return NULL;
}
