#include "../headers/global.h"

void* game_handler(){

    while(1){

        printf("debut SESSION\n");
        game->tour_act = 0;

        //message SESSION
        msg_session();

        //boucle des tours
        while(game->tour_act < nb_tours){
            //on empeche les clients de lire les donnees du jeu.
            pthread_mutex_lock(game->mutex_clients);

            //setup grille du tour actuel.
            init_grille();

            //debut du tour
            printf("DEBUT TOUR\n");

            //message TOUR
            msg_tour();

            //lancement du thread timer
            pthread_t pthread_id;
            pthread_create( &pthread_id , NULL ,  timer_tour, NULL);

            //les clients peuvent lire les informations du jeu
            pthread_mutex_unlock(game->mutex_clients);

            //en Attente du signal du timer (bloquant)
            pthread_mutex_lock(game->mutex_timer);
            pthread_cond_wait(game->event, game->mutex_timer);
            pthread_mutex_unlock(game->mutex_timer);

            //Fin du tour
            printf("TOUR FINI\n");

            //message RFIN
            msg_rfin();

            //lock des clients sur les ressources
            pthread_mutex_lock(game->mutex_clients);

            //calcul et attribution des scores du tour
            calcul_scores();

            //msg BILANMOTS
            msg_bilanmots();

            //remise a zero des motsproposes pour chaque client
            int i;
            for(i=0; i<MAX_CLIENTS; i++){
                if(clients[i]->is_ready == TRUE){
                    supp_all_props(&(clients[i]->list_prop));
                }
            }

            pthread_mutex_unlock(game->mutex_clients);

            //pause resultats entre 2 tours
            sleep(TEMPS_PAUSE);

            //indice prochain tour
            game->tour_act++;

        }

        printf("fin SESSION\n");

        //msg VAINQUEUR
        msg_vainqueur();

        sleep(TEMPS_PAUSE);

        pthread_mutex_lock(game->mutex_clients);
        
        //remise a zero des scores
        int i;
        for(i=0; i<MAX_CLIENTS; i++)
            if(clients[i]->is_ready == TRUE)
                clients[i]->score = 0;

        pthread_mutex_unlock(game->mutex_clients);
    }

    return NULL;
}

void init_grille(){
    if(opt_grilles == TRUE){
        game->grille_act = grilles[game->tour_act % nb_grilles];
    }else{
        char des[16][6] = {{'E','T','U','K','N','O'},
                       {'E','V','G','T','I','N'},
                       {'D','E','C','A','M','P'},
                       {'I','E','L','R','U','W'},
                       {'E','H','I','F','S','E'},
                       {'R','E','C','A','L','S'},
                       {'E','N','T','D','O','S'},
                       {'O','F','X','R','I','A'},
                       {'N','A','V','E','D','Z'},
                       {'E','I','O','A','T','A'},
                       {'G','L','E','N','Y','U'},
                       {'B','M','A','Q','J','O'},
                       {'T','L','I','B','R','A'},
                       {'S','P','U','L','T','E'},
                       {'A','I','M','S','O','R'},
                       {'E','N','H','R','I','S'}};

        int choix[16] = { -1 };
        srand(time(NULL));     

        //ordre ede selection des des;
        int i;
        for(i=0; i<16; i++){
            int random = rand()%16;
            boolean deja_choisi = FALSE;

            int j;
            for(j=0; j<i; j++){
                if(choix[j] ==  random){
                    deja_choisi = TRUE;
                    break;
                }
            }

            if(deja_choisi == FALSE){
                choix[i] = random;
            }else{
                i--;
            }
        }

        //for(i=0; i<16; i++) printf("%d, ", choix[i]);
        //printf("\n");    

        for(i=0; i<16; i++){
            int random = rand()%6;
            game->grille_act[i] = des[choix[i]][random];

            //printf("%d, %c, %d\n", choix[i], game->grille_act[i], random);
        }

        //printf("grille_act : %s, last:%d \n", game->grille_act, game->grille_act[16]);

    }
}

void calcul_scores(){
    int i;
    for(i=0; i<MAX_CLIENTS; i++){
        if(clients[i]->is_ready == TRUE){
            
            //boucle sur tous les mots proposes du client, ajout de points si valide.
            propos* temp = clients[i]->list_prop;
            while(temp != NULL){

                //si l'option -immediat n'est pas active, on zappe les mots non uniques
                boolean est_unique = TRUE;
                if(immediat == FALSE){
                    int j;
                    for(j=0; j<MAX_CLIENTS; j++){
                        if( (j != i) && (clients[j]->is_ready == TRUE) ) {
                            propos* temp2 = clients[j]->list_prop;
                            while(temp2 != NULL){
                                if(strcmp(temp->mot, temp2->mot) == 0){
                                    est_unique = FALSE;
                                    break;
                                }
                                temp2 = temp2->next;
                            }
                        }
                        if(est_unique == FALSE) break;
                    }
                }
         
                if(est_unique == TRUE){
                    if(strlen(temp->mot) == 3) clients[i]->score += 1;
                    else if(strlen(temp->mot) == 4) clients[i]->score += 1;
                    else if(strlen(temp->mot) == 5) clients[i]->score += 2;
                    else if(strlen(temp->mot) == 6) clients[i]->score += 3;
                    else if(strlen(temp->mot) == 7) clients[i]->score += 5;
                    else if(strlen(temp->mot) >= 8) clients[i]->score += 11;
                    else printf("ERREUR SCORE [%s] taille < 3 ?\n", temp->mot);
                }

                temp = temp->next;
            }
        }
    }
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
    //construction chaine scores
    char scores[BUF_SIZE] = { 0 };

    sprintf(scores+strlen(scores), "%d", game->tour_act);
    int i;
    for(i=0; i<MAX_CLIENTS; i++){
        if(clients[i]->is_ready == TRUE){
            strcat(scores, "*");
            strcat(scores, clients[i]->user);
            strcat(scores, "*");
            snprintf(scores+strlen(scores), 12, "%d", clients[i]->score);
        }
    }

    //message BILANMOTS
    char buffer_out[BUF_SIZE] = { 0 };

    for(i=0; i<MAX_CLIENTS; i++){
        if(clients[i]->is_ready == TRUE){
            strcat(buffer_out, "BILANMOTS/");

            //ajout chaine motsproposes
            propos* temp = clients[i]->list_prop;
            while(temp != NULL){
                strcat(buffer_out, temp->mot);
                strcat(buffer_out, "*");
                temp = temp->next;
            }
            buffer_out[strlen(buffer_out)] = '\0';

            //ajout chaine scores/
            strcat(buffer_out, "/");
            strcat(buffer_out, scores);
            strcat(buffer_out, "/\n");

            send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
            buffer_out[0] = '\0';
        }
    }
}

void msg_vainqueur(){
    //construction chaine scores
    char scores[BUF_SIZE] = { 0 };

    sprintf(scores+strlen(scores), "%d", game->tour_act);
    int i;
    for(i=0; i<MAX_CLIENTS; i++){
        if(clients[i]->is_ready == TRUE){
            strcat(scores, "*");
            strcat(scores, clients[i]->user);
            strcat(scores, "*");
            snprintf(scores+strlen(scores), 12, "%d", clients[i]->score);
            //strcat(scores, "*");
        }
    }
    //scores[strlen(scores)] = '\0';


    //message RFIN
    char buffer_out[BUF_SIZE] = { 0 };

    strcat(buffer_out, "VAINQUEUR/");
    strcat(buffer_out, scores);
    strcat(buffer_out, "/\n");

    for(i=0; i<MAX_CLIENTS; i++){
        if(clients[i]->is_ready == TRUE){
            send(clients[i]->sock, buffer_out, strlen(buffer_out), 0);
        }
    }
}

boolean est_valide(int slot, char* mot, char* traj, char* raison){

    //verification taille trajectoire/mot
    if(strlen(traj) != 2*strlen(mot)){
        sprintf(raison, "POS taille traj != 2* taille mot");
        return FALSE;
    }

    //boucle de verification de la trajectoire
    int x, y, x_prec, y_prec;
    int i;
    for(i=0; i<strlen(mot); i++){

        x = traj[i*2+1] - '1';
        y = traj[i*2] - 'A';
        int index = COTE_GRILLE * y + x;

        if( !(0<=x && x<COTE_GRILLE) || !(0<=y && y<COTE_GRILLE) ){
            sprintf(raison, "POS outofbounds :%c%c",traj[i*2], traj[i*2+1]);
            return FALSE;
        }
        if(mot[i] != game->grille_act[index]){
            sprintf(raison, "POS mot[%d]: %c != %c : traj[%c%c]", i, mot[i], 
                game->grille_act[index], traj[i*2], traj[i*2+1]);
            return FALSE;
        }

        if(i > 0){
           if( (!(x_prec-1 <= x && x <= x_prec+1)) || 
            (!(y_prec-1 <= y && y <= y_prec+1)) ){
            sprintf(raison, "POS %c%c et %c%c non adjacents",
                traj[(i-1)*2], traj[(i-1)*2+1], traj[i*2], traj[i*2+1]);
            return FALSE;
           }
        }

        x_prec = x;
        y_prec = y;
    }

    //boucle de verification du mot dans le dictionnaire
    boolean dans_le_dico = FALSE;
    char ligne[50] = { 0 };
    FILE* dico = fopen(DICO_FILENAME, "r");
    while (!feof(dico)){
        memset(ligne, '\0', 50);
        fgets(ligne, 49, dico);
        char* newline = strchr(ligne, '\n');
        char* retchar = strchr(ligne, '\r');
        if (newline != NULL) *newline = '\0';
        if (retchar != NULL) *retchar = '\0';

        //printf("%s | %s | %zd\n", mot, ligne, strlen(ligne));

        if(strcmp(mot, ligne) == 0) {
            dans_le_dico = TRUE;
            break;
        }
    }
    if(dans_le_dico == FALSE){
            sprintf(raison, "DIC mot non trouve dans le dictionnaire");
            return FALSE;
    }

    //boucle de verification mot deja pris par soi meme
    propos* temp = clients[slot]->list_prop;
    while(temp != NULL){
        if(strcmp(mot, temp->mot) == 0){
            sprintf(raison, "PRI vous avez deja pris ce mot");
            return FALSE;
        }
        temp = temp->next;
    }

    //boucle de verification mot deja pris par un autre joueur
    //(option -immediat active pour le serveur)
    //(si -immediat non active, voir calcul_scores() qui zappera les mots non uniques)
    if(immediat == TRUE){
        for(i=0; i<MAX_CLIENTS; i++){
            if( (i != slot) && (clients[i]->is_ready == TRUE) ){
                temp = clients[i]->list_prop;

                while(temp != NULL){
                    if(strcmp(mot, temp->mot) == 0){
                        sprintf(raison, "PRI %s a deja pris ce mot", clients[i]->user);
                        return FALSE;
                    }
                    temp = temp->next;
                }
            }
        }
    }



    return TRUE;
}

void ajout_prop(propos** ptr_list_prop, char* mot){
    propos* nouv = (propos*) malloc (sizeof(propos));
    nouv->mot = strndup(mot, TAILLE_GRILLE);
    nouv->next = *ptr_list_prop;

    *ptr_list_prop = nouv;
}

void supp_all_props(propos* * ptr_list_prop){
    propos* act = *ptr_list_prop;

    while(act != NULL){
        propos* suiv = act->next;
        free(act->mot);
        act->mot = NULL;
        free(act);
        act = suiv;
    }

    *ptr_list_prop = NULL;
}

void* timer_tour(){

    sleep(TEMPS_TOUR);

    pthread_mutex_lock(game->mutex_timer);
    pthread_cond_signal(game->event);
    pthread_mutex_unlock(game->mutex_timer);

    return NULL;
}
