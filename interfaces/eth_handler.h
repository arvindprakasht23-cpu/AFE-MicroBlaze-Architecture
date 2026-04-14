#ifndef ETH_HANDLER_H
#define ETH_HANDLER_H

#include "message.h"

void eth_init(void);
void eth_poll(void);
int  eth_get_message(Message_t *msg); // Returns 1 if a message is ready
void eth_send_msg(char *msg);         // Used by main to send responses

#endif