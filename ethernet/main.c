#include <stdio.h>
#include <string.h>
#include "xparameters.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "lwip/tcp.h"
#include "xil_cache.h"

#include "ethernet_server.h"
#include "parser.h"
#include "executor.h"
#include "axi_regs.h"
#include "api_wrapper.h" // For enum

void lwip_init();
extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
void tcp_fasttmr(void);
void tcp_slowtmr(void);

static struct netif server_netif;
struct netif *echo_netif;

int main() {
    ip_addr_t ipaddr, netmask, gw;
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

    echo_netif = &server_netif;
    init_platform();

    IP4_ADDR(&ipaddr,  192, 168,   1, 10);
    IP4_ADDR(&netmask, 255, 255, 255,  0);
    IP4_ADDR(&gw,      192, 168,   1,  1);

    print_app_header();
    lwip_init();

    if (!xemac_add(echo_netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
        xil_printf("Error adding N/W interface\n\r");
        return -1;
    }

    netif_set_default(echo_netif);
    platform_enable_interrupts();
    netif_set_up(echo_netif);

    start_application(); 

    char buffer[256];
    char reply[128];

    while (1) {
        if (TcpFastTmrFlag) { tcp_fasttmr(); TcpFastTmrFlag = 0; }
        if (TcpSlowTmrFlag) { tcp_slowtmr(); TcpSlowTmrFlag = 0; }
        
        xemacif_input(echo_netif); 
        transfer_data();

        if (eth_has_data()) {
            eth_get_data(buffer);
            
            parse_and_store(buffer);
            executor_poll();
            
            u16 status = READ_STATUS();
            u8 opcode  = READ_OPCODE();

            if (status == TI_AFE_RET_EXEC_PASS) {
                if (opcode == OPCODE_RAW_READ) { 
                    sprintf(reply, "SUCCESS: Read Value = 0x%02X\r\n", HW_RESULT_BASE[0]);
                    eth_send_response(reply);
                } 
                else if (opcode == OPCODE_RAW_READ_MULTI) { 
                    sprintf(reply, "SUCCESS: Multi Read Complete. First byte: 0x%02X\r\n", HW_RESULT_BASE[0]);
                    eth_send_response(reply);
                }
                else if (opcode == OPCODE_BURST_READ) { 
                    sprintf(reply, "SUCCESS: Burst Read Complete\r\n");
                    eth_send_response(reply);
                }
                else {
                    eth_send_response("SUCCESS\r\n");
                }
            } 
            else if (opcode != 0xFF) {
                eth_send_response("ERROR: Hardware Execution Failed\r\n");
            } 
            else {
                eth_send_response("ERROR: Invalid Command Syntax\r\n");
            }
        }
    }

    cleanup_platform();
    return 0;
}