#include <stdio.h>
#include <string.h>
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "xil_printf.h"

#define TCP_PORT 5001

volatile int eth_string_ready = 0;
char eth_rx_buffer[256];

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb); 
        tcp_recv(tpcb, NULL); 
        return ERR_OK;
    }

    if (eth_string_ready == 0) {
        int copy_len = p->tot_len;
        if (copy_len > 255) copy_len = 255; 

        memcpy(eth_rx_buffer, p->payload, copy_len);
        eth_rx_buffer[copy_len] = '\0'; 

        eth_string_ready = 1; 

        char *response = "ACK: String Received.\n";
        tcp_write(tpcb, response, strlen(response), 1);
        
        // --- SDK 2018 ADDITION: Force the packet out instantly! ---
        tcp_output(tpcb); 

    } else {
        char *response = "ERR: FPGA Busy.\n";
        tcp_write(tpcb, response, strlen(response), 1);
        tcp_output(tpcb);
    }
    
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    xil_printf("Connected to PC Client!\r\n");
    tcp_recv(newpcb, recv_callback);
    return ERR_OK;
}

void start_tcp_server() {
    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, TCP_PORT);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, accept_callback);
    xil_printf("Server Listening on Port %d...\r\n", TCP_PORT);
}