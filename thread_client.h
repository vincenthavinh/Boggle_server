#ifndef THREAD_CLIENT_H
#define THREAD_CLIENT_H

void* client_handler(void* slot_client);

void comm_connexion(int slot, char* user);
void comm_sort(int slot);
/*void comm_trouve(char* buffer_in,int i_start, char* buffer_out, int slot);
*/

#endif