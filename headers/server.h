#ifndef SERVER_H
#define SERVER_H

/*========== Fonctions ==========*/

void parse_command_line(int argc, char* argv[]);
void print_args();
int init_socket(int port);
void init_client();

#endif