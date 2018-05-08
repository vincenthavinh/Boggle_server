#ifndef THREAD_CLIENT_H
#define THREAD_CLIENT_H

void* client_handler(void* sock_client);
int recv_message(int sock, char buffer[]);
int index_of_slash(char buffer[]);
comm_client get_command(char buffer[], int i_start, int i_end);

#endif