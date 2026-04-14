#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include "message.h"

void uart_init(void);
void uart_poll(void);
int  uart_get_message(Message_t *msg); // Returns 1 if a message is ready
void print_help(void);

#endif