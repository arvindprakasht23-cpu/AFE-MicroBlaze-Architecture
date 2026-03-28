#include <stdio.h>
#include <string.h>
#include "xil_printf.h"

// Core System
#include "message.h"

// Communication Drivers
#include "uart_handler.h"
#include "eth_handler.h"

// Execution Engine
#include "parser.h"
#include "executor.h"
#include "axi_regs.h"
#include "api_wrapper.h"
#include "afe_drivers.h"

// --- THE CORE LOGIC ENGINE ---
// Completely blind to whether the command came from PC or Terminal
void execute_core_logic(char *payload, char *response_buffer, int is_ethernet) {
    
    // 1. Parse and Execute
    parse_and_store(payload);
    executor_poll();

    // 2. Fetch Hardware Results
    u16 status = READ_STATUS();
    u8 opcode  = READ_OPCODE();

    // 3. Generate the Universal Response String
    if (status == TI_AFE_RET_EXEC_PASS) {
        if (!is_ethernet) xil_printf("[MAIN] SUCCESS: Opcode 0x%02X\r\n", opcode);

        if (opcode == OPCODE_RAW_READ) { 
            sprintf(response_buffer, "SUCCESS: Read Val = 0x%02X\r\n", HW_RESULT_BASE[0]);
            if (!is_ethernet) xil_printf("   -> Result: 0x%02X\r\n", HW_RESULT_BASE[0]);
        } 
        else if (opcode == OPCODE_RAW_READ_MULTI) { 
            sprintf(response_buffer, "SUCCESS: Multi Read Complete. First Byte = 0x%02X\r\n", HW_RESULT_BASE[0]);
            if (!is_ethernet) {
                for (int i = 0; i < NUM_SPI; i++) xil_printf("      SPI[%d]: 0x%02X\r\n", i, HW_RESULT_BASE[i]);
            }
        }
        else if (opcode == OPCODE_BURST_READ) { 
            sprintf(response_buffer, "SUCCESS: Burst Read Complete.\r\n");
            if (!is_ethernet) {
                uint16_t size;
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
    // 1. Initialize Hardware Abstraction Layers
    uart_init();
    eth_init();

    Message_t active_msg;
    char response_buffer[256];

    xil_printf("\r\nSystem Boot Complete. Dispatcher Ready.\r\nCMD> ");

    // 2. The Dispatcher Super-Loop
    while (1) {
        
        // Keep hardware buffers fed
        uart_poll();
        eth_poll();

        // --- THE DISPATCHER / ROUTER ---
        if (uart_get_message(&active_msg) || eth_get_message(&active_msg)) {
            
            // Log Ethernet traffic to the local terminal for debugging
            if (active_msg.source_id == SRC_ETHERNET) {
                xil_printf("\r\n[TCP RECV]: %s\r\n", active_msg.payload);
            }

            // Ignore empty "Enter" key presses
            if (active_msg.length > 0) {
                // 1. Execute the agnostic payload
                int is_eth = (active_msg.source_id == SRC_ETHERNET);
                execute_core_logic(active_msg.payload, response_buffer, is_eth);
            }

            // 2. Route the response back to the correct source
            if (active_msg.source_id == SRC_UART) {
                if (active_msg.length > 0) xil_printf("%s", response_buffer);
                xil_printf("CMD> "); // Reprint the terminal prompt
            } 
            else if (active_msg.source_id == SRC_ETHERNET) {
                eth_send_msg(response_buffer);
                xil_printf("CMD> "); // Restore the local terminal prompt after TCP event
            }
        }
    }

    return 0;
}