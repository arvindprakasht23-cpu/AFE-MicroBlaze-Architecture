#include "eth_handler.h"
#include "xparameters.h"
#include "netif/xadapter.h"
#include "lwip/tcp.h"
#include "xil_printf.h"
#include <string.h>

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
void tcp_fasttmr(void);
void tcp_slowtmr(void);

static struct netif server_netif;
static struct tcp_pcb *active_pcb = NULL;

static char eth_rx_buffer[256];
static volatile int eth_flag = 0;

void eth_send_msg(char *msg) {
    if (active_pcb != NULL) {
        tcp_write(active_pcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
        tcp_output(active_pcb);
    }
}

static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) { 
        tcp_close(tpcb); 
        return ERR_OK; 
    }

    // -------------------------------------------------------------
    // SECURITY LOCK: If the main loop hasn't read the last packet, 
    // -------------------------------------------------------------
    if (eth_flag == 1) {
        return ERR_OK; 
    }

    active_pcb = tpcb;
    tcp_recved(tpcb, p->tot_len); 

    int copy_len = (p->tot_len < 255) ? p->tot_len : 255;
    memcpy(eth_rx_buffer, p->payload, copy_len);
    eth_rx_buffer[copy_len] = '\0';
    
    eth_flag = 1; 
    pbuf_free(p);
    return ERR_OK;
}

static err_t tcp_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    xil_printf("\r\n[ETH] New PC Client Connected!\r\nCMD> ");
    tcp_recv(newpcb, tcp_recv_callback); 
    return ERR_OK;
}

void eth_init(void) {
    ip_addr_t ipaddr, netmask, gw;
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };
    
    IP4_ADDR(&ipaddr, 192, 168, 1, 10);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 1, 1);

    lwip_init();
    xemac_add(&server_netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR);
    netif_set_default(&server_netif);
    platform_enable_interrupts(); 
    netif_set_up(&server_netif);

    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, 7);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, tcp_accept_callback);
    xil_printf("\r\n--- Ethernet LwIP Server Started (192.168.1.10:7) ---\r\n");
}

void eth_poll(void) {
    if (TcpFastTmrFlag) { tcp_fasttmr(); TcpFastTmrFlag = 0; }
    if (TcpSlowTmrFlag) { tcp_slowtmr(); TcpSlowTmrFlag = 0; }
    xemacif_input(&server_netif); 
}

int eth_get_message(Message_t *msg) {
    if (eth_flag) {
        msg->source_id = SRC_ETHERNET;
        msg->length = strlen(eth_rx_buffer);
        strcpy(msg->payload, eth_rx_buffer);
        
        eth_flag = 0; // Unlock the buffer for the next network packet
        return 1;
    }
    return 0;
}
