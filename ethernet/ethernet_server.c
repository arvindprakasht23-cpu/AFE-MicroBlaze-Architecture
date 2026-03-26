#include <string.h>
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "xil_printf.h"
#include "ethernet_server.h"

volatile int eth_data_ready = 0;
char global_eth_buffer[256];
struct tcp_pcb *active_pcb = NULL;

int eth_has_data(void) { return eth_data_ready; }

void eth_get_data(char *buffer) {
    strcpy(buffer, global_eth_buffer);
    eth_data_ready = 0; 
}

void eth_send_response(char *msg) {
    if (active_pcb != NULL) {
        tcp_write(active_pcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
        tcp_output(active_pcb);
    }
}

int transfer_data() { return 0; }

void print_app_header() {
    xil_printf("\n\r----- LwIP TCP Server (Agnostic Parser) -----\n\r");
    xil_printf("Send string commands directly to Port 7\n\r");
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }
    active_pcb = tpcb;
    tcp_recved(tpcb, p->len);

    int copy_len = (p->len < 255) ? p->len : 255;
    memcpy(global_eth_buffer, p->payload, copy_len);
    global_eth_buffer[copy_len] = '\0'; 
    
    eth_data_ready = 1;

    pbuf_free(p);
    return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    static int connection = 1;
    tcp_recv(newpcb, recv_callback);
    tcp_arg(newpcb, (void*)(UINTPTR)connection);
    connection++;
    return ERR_OK;
}

int start_application() {
    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb) return -1;
    
    if (tcp_bind(pcb, IP_ANY_TYPE, 7) != ERR_OK) return -2;
    
    tcp_arg(pcb, NULL);
    pcb = tcp_listen(pcb);
    if (!pcb) return -3;
    
    tcp_accept(pcb, accept_callback);
    xil_printf("TCP Server listening on Port 7\n\r");
    return 0;
}