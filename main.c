#include <stdio.h>
#include <string.h>
#include "xil_printf.h"

/* Core System Infrastructure */
#include "interfaces/message.h"

/* Communication Drivers (Transport Layer) */
#include "interfaces/uart_handler.h"
#include "interfaces/eth_handler.h"

/* Execution Engine & Parsing (Application/Logic Layer) */
#include "user_logic/parser.h"
#include "core_logic/executor.h"

/* Hardware Mapping & AXI Drivers (Hardware Layer) */
#include "axi_regs.h"
#include "core_logic/api_wrapper.h"
#include "core_logic/afe_drivers.h"

/* * THE CORE LOGIC ENGINE 
 * Completely blind to whether the command came from PC or Terminal.
 */
void execute_core_logic(char *payload, char *response_buffer, int is_ethernet) {
    
    /* 1. Parse and Execute */
    parse_and_store(payload);
    executor_poll();

    /* 2. Fetch Hardware Results */
    u16 status = READ_STATUS();
    u8 opcode  = READ_OPCODE();

    /* 3. Generate the Universal Response String */
    if (status == TI_AFE_RET_EXEC_PASS) {
        if (!is_ethernet) xil_printf("[MAIN] SUCCESS: Opcode 0x%02X\r\n", opcode);

        if (opcode == OPCODE_SPI_RAW_READ) { 
            sprintf(response_buffer, "SUCCESS: Read Val = 0x%02X\r\n", HW_RESULT_BASE[0]);
            if (!is_ethernet) xil_printf("   -> Result: 0x%02X\r\n", HW_RESULT_BASE[0]);
        } 
        else if (opcode == OPCODE_SPI_RAW_READ_MULTI) { 
            sprintf(response_buffer, "SUCCESS: Multi Read Complete. First Byte = 0x%02X\r\n", HW_RESULT_BASE[0]);
            if (!is_ethernet) {
                for (int i = 0; i < NUM_SPI; i++) xil_printf("      SPI[%d]: 0x%02X\r\n", i, HW_RESULT_BASE[i]);
            }
        }
        else if (opcode == OPCODE_SPI_BURST_READ) { 
            sprintf(response_buffer, "SUCCESS: Burst Read Complete.\r\n");
            if (!is_ethernet) {
                uint16_t size;
                /* Matches 1 byte afeInst + 2 bytes addr offset */
                memcpy(&size, (const void *)&HW_OPERAND_BASE[3], 2);
                xil_printf("   -> Burst Data:\r\n");
                for (uint16_t i = 0; i < size; i++) xil_printf("      [%d]: 0x%02X\r\n", i, HW_RESULT_BASE[i]);
            }
        }
        else {
            sprintf(response_buffer, "SUCCESS: Command Executed.\r\n");
        }
    } 
    else if (opcode != 0xFF) {
        sprintf(response_buffer, "ERROR: Hardware Execution Failed (Code 0x%04X)\r\n", status);
    } 
    else {
        sprintf(response_buffer, "ERROR: Invalid Command Syntax.\r\n");
        if (!is_ethernet) print_help();
    }
}

int main() {
    /* 1. Initialize Hardware Abstraction Layers */
    uart_init();
    eth_init();

    Message_t active_msg;
    char response_buffer[256];

    xil_printf("\r\nSystem Boot Complete. Dispatcher Ready.\r\nCMD> ");

    /* 2. The Dispatcher Super-Loop */
    while (1) {
        
        /* Keep hardware buffers fed */
        uart_poll();
        eth_poll();

        /* --- PATH 1: PROCESS UART TERMINAL COMMANDS --- */
        if (uart_get_message(&active_msg)) {
            
            /* Ignore empty "Enter" key presses */
            if (active_msg.length > 0) {
                /* Execute and print response to local terminal */
                execute_core_logic(active_msg.payload, response_buffer, 0); /* 0 = not ethernet */
                xil_printf("%s", response_buffer);
            }
            xil_printf("CMD> "); /* Reprint the terminal prompt */
        }

        /* --- PATH 2: PROCESS ETHERNET TCP COMMANDS --- */
        if (eth_get_message(&active_msg)) {
            
            /* Log Ethernet traffic to the local terminal for debugging */
            xil_printf("\r\n[TCP RECV]: %s\r\n", active_msg.payload);
            
            if (active_msg.length > 0) {
                /* Execute and send response over TCP socket */
                execute_core_logic(active_msg.payload, response_buffer, 1); /* 1 = is ethernet */
                eth_send_msg(response_buffer);
            }
            xil_printf("CMD> "); /* Restore the local terminal prompt after TCP event finishes */
        }
        
    }

    return 0;
}