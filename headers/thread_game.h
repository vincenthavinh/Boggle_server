#ifndef THREAD_GAME_H
#define THREAD_GAME_H

void* game_handler();
void* timer_tour();

void msg_session();
void msg_tour();
void msg_rfin();
void msg_bilanmots();
void msg_vainqueur();

void init_grille();
boolean est_valide(int slot, char* mot, char* traj, char* raison);
void ajout_prop(propos** prop, char* mot);
void supp_all_props(propos** ptr_list_prop);

void calcul_scores();

#endif