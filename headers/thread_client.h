#ifndef THREAD_CLIENT_H
#define THREAD_CLIENT_H

void* client_handler(void* slot_client);

void comm_connexion(int slot, char* user);
void comm_sort(int slot);
void comm_trouve(int slot, char* mot, char* traj, FILE* dico);
void comm_envoi(int slot, char* message);
void comm_penvoi(int slot, char* message, char* user);

#endif