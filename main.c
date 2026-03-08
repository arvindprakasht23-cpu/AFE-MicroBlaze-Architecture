#include <stdio.h>
#include "xparameters.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "lwip/tcp.h"
#include "xil_printf.h"

#include "executor.h"
#include "parser.h" 
#include "tcp_server.h" 
#include "axi_regs.h"     
#include "afe_drivers.h"  

// --- SDK 2018 LwIP TIMER FLAGS ---
// These are managed by the interrupt handler in Xilinx's platform.c
extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;

static struct netif server_netif;

int main() {
    ip_addr_t ipaddr, netmask, gw;
    // The MAC address for your Nexys 4 DDR (you can change the last byte if needed)
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

    init_platform(); // This crucially starts the AXI Timer and Interrupts!
    lwip_init();

    IP4_ADDR(&ipaddr,  192, 168,   1,  10);
    IP4_ADDR(&netmask, 255, 255, 255,   0);
    IP4_ADDR(&gw,      192, 168,   1,   1);

    xemac_add(&server_netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR);
    netif_set_default(&server_netif);
    netif_set_up(&server_netif);

    start_tcp_server();

    int cmd_pending = 0; 

    while (1) {
        // --- SDK 2018 TCP TIMER MANAGEMENT ---
        if (TcpFastTmrFlag) {
            tcp_fasttmr();
            TcpFastTmrFlag = 0;
        }
        if (TcpSlowTmrFlag) {
            tcp_slowtmr();
            TcpSlowTmrFlag = 0;
        }

        // Process any incoming packets from the Nexys 4 DDR EthernetLite MAC
        xemacif_input(&server_netif); 
        
        // 1. CHECK ETHERNET FOR STRINGS
        if (eth_string_ready) {
            xil_printf("\r\nCMD Received: %s\r\n", eth_rx_buffer);
            parse_and_store(eth_rx_buffer); 
            eth_string_ready = 0;           
            
            if (READ_CMD() != 0) {
                cmd_pending = 1; 
            } else {
                xil_printf("FAIL: Parser rejected string.\r\n");
            }
        }

        // 2. RUN THE HARDWARE EXECUTOR
        executor_poll();              

        // 3. CHECK EXECUTION RESULT
        if (cmd_pending && READ_CMD() == 0) {
            u16 final_status = READ_STATUS();
            if (final_status == TI_AFE_RET_EXEC_PASS) {
                xil_printf("SUCCESS: Hardware passed! (0x%04X)\r\n", final_status);
            } else {
                xil_printf("FAIL: Hardware failed! (0x%04X)\r\n", final_status);
            }
            cmd_pending = 0; 
        }
    }

    cleanup_platform();
    return 0;
}