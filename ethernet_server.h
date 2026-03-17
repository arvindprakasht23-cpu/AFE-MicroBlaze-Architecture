#ifndef ETHERNET_SERVER_H
#define ETHERNET_SERVER_H

int start_application(void);
int eth_has_data(void);
void eth_get_data(char *buffer);
void eth_send_response(char *msg);
void print_app_header(void);
int transfer_data(void);

#endif