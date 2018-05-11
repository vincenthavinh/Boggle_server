#include "../headers/global.h"

void* game_handler(){

    while(1){

        printf("debut SESSION\n");
        game->tour_act = 0;

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
            msg_bilanmots();

            //pause resultats entre 2 tours
            sleep(TEMPS_PAUSE);

            //indice prochain tour
            game->tour_act++;

        }

        printf("fin SESSION\n");
        msg_vainqueur();
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

void msg_bilanmots(){
    char buffer_out[BUF_SIZE] = { 0 };

    //message RFIN
    strcat(buffer_out, "BILANMOTS/motsproposes/scores/\n");

    int i;
    for(i=0; i<MAX_CLIENTS; i++){
        if(clients[i]->is_ready == TRUE){
            send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
        }
    }
}

void msg_vainqueur(){
    char buffer_out[BUF_SIZE] = { 0 };

    //message RFIN
    strcat(buffer_out, "VAINQUEUR/bilan/\n");

    int i;
    for(i=0; i<MAX_CLIENTS; i++){
        if(clients[i]->is_ready == TRUE){
            send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
        }
    }
}


boolean est_valide(char* mot, char* traj, char* raison){
    boolean traj_valide = FALSE;
    boolean mot_valide = FALSE;

    //verification taille trajectoire/mot
    if(strlen(traj) != 2*strlen(mot)){
        sprintf(raison, "POS taille traj != 2* taille mot");
    }

    //verification trajectoire et mot
    else{

        //boucle de verification de la trajectoire
        int x, y, x_prec, y_prec;

        int i;
        for(i=0; i<strlen(mot); i++){

            x = traj[i*2+1] - '1';
            y = traj[i*2] - 'A';
            int index = COTE_GRILLE * y + x;

            if( !(0<=x && x<COTE_GRILLE) || !(0<=y && y<COTE_GRILLE) ){
                sprintf(raison, "POS outofbounds :%c%c",traj[i*2], traj[i*2+1]);
                break;
            }
            if(mot[i] != game->grille_act[index]){
                sprintf(raison, "POS mot[%d]: %c != %c : traj[%c%c]", i, mot[i], 
                    game->grille_act[index], traj[i*2], traj[i*2+1]);
                break;
            }

            if(i > 0){
               if( (!(x_prec-1 <= x && x <= x_prec+1)) || 
                (!(y_prec-1 <= y && y <= y_prec+1)) ){
                sprintf(raison, "POS %c%c et %c%c non adjacents",
                    traj[(i-1)*2], traj[(i-1)*2+1], traj[i*2], traj[i*2+1]);
                printf("x:%d, x_pre:%d, y:%d, y_pre:%d\n", x, x_prec, y, y_prec);
                break;
               }
            }

            x_prec = x;
            y_prec = y;

            if(i == strlen(mot)-1) traj_valide = TRUE;
        }

        //si trajectoire bonne, on verifie le dictionnaire
        if(traj_valide == TRUE){

            //boucle de verification du mot dans le dico
            char ligne[17] = { 0 };
            FILE* dico = fopen(DICO_FILENAME, "r");
            while (!feof(dico) && mot_valide==FALSE){
                fgets(ligne, TAILLE_GRILLE-1, dico);
                char* newline = strchr(ligne, '\n');
                if (newline != NULL) *newline = '\0';

                if(strcmp(mot, ligne) == 0) {
                    mot_valide = TRUE;
                    break;
                }
            }
            sprintf(raison, "DIC mot non trouve dans le dictionnaire");
        }
    }

    return (traj_valide && mot_valide);
}

/*void ajout_prop(propos* prop, char* mot, boolean valide){
    propos* nouv = (propos*) malloc (sizeof(struct propos_struct));
    nouv->valide = valide;
    nouv->mot = strndup(mot, TAILLE_GRILLE);
    nouv->next = prop;
}*/

void* timer_tour(){

    sleep(TEMPS_TOUR);

    pthread_mutex_lock(game->mutex);
    pthread_cond_signal(game->event);
    pthread_mutex_unlock(game->mutex);

    return NULL;
}
