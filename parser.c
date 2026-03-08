#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "mapper.h"
#include "axi_regs.h"
#include "api.h"

void parse_and_store(char *input) {
    if (READ_CMD() != 0) return;
    for (int i = 0; i < 32; i++) HW_OPERAND_BASE[i] = 0x00;

    char *cmd = strtok(input, "(");
    if (!cmd) return;

    uint8_t opcode = get_opcode_from_string(cmd);

    if (opcode == 0xFF) {
        WRITE_OPCODE(0xFF);                  
        WRITE_STATUS(0x0001);  
        WRITE_CMD(0);                        
        return;                              
    }

    char *arg_string = strtok(NULL, ")");
    if (!arg_string) return;

    char *token = strtok(arg_string, ",");
    uint32_t hw_offset = 0;
    int arg_index = 0;

    while (token != NULL) {
        if (hw_offset >= 31) break; 
        while (*token == ' ') token++; 
        uint32_t val = (uint32_t)strtoul(token, NULL, 16);

        if (arg_index == 0) {
            HW_OPERAND_BASE[hw_offset++] = (uint8_t)(val & 0xFF);
        } else if (arg_index == 1) {
            HW_OPERAND_BASE[hw_offset++] = (uint8_t)(val & 0xFF);        
            HW_OPERAND_BASE[hw_offset++] = (uint8_t)((val >> 8) & 0xFF); 
        } else if (arg_index == 2) {
            if (opcode == OPCODE_BURST_WRITE || opcode == OPCODE_BURST_READ || opcode == OPCODE_BURST_WRITE_MULTI) {
                HW_OPERAND_BASE[hw_offset++] = (uint8_t)(val & 0xFF);        
                HW_OPERAND_BASE[hw_offset++] = (uint8_t)((val >> 8) & 0xFF); 
            } else {
                HW_OPERAND_BASE[hw_offset++] = (uint8_t)(val & 0xFF);
            }
        } else {
            HW_OPERAND_BASE[hw_offset++] = (uint8_t)(val & 0xFF);
        }
        
        token = strtok(NULL, ",");
        arg_index++;
    }

    WRITE_OPCODE(opcode);
    WRITE_CMD(1); 
}