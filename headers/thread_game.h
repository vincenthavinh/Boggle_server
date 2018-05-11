#ifndef THREAD_GAME_H
#define THREAD_GAME_H

void* game_handler();
void* timer_tour();

void msg_session();
void msg_tour();
void msg_rfin();
void msg_bilanmots();
void msg_vainqueur();

boolean est_valide(char* mot, char* traj, char* raison);
void ajout_prop(propos** prop, char* mot, boolean valide);
void supp_all_props(propos** ptr_list_prop);

#endif