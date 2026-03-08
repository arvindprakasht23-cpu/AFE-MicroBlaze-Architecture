#ifndef TCP_SERVER_H
#define TCP_SERVER_H

// Expose the global flag and buffer so main.c can read them
extern volatile int eth_string_ready;
extern char eth_rx_buffer[256];

// Expose the initialization function
void start_tcp_server(void);

#endif