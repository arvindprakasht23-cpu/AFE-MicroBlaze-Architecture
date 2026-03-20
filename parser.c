#include "parser.h"
#include "command_dict.h"
#include "axi_regs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void parse_and_store(char *input) {
    char *cmd_name = strtok(input, "( \r\n");
    if (!cmd_name) return;

    for (int i = 0; i < DICT_SIZE; i++) {
        if (strcmp(cmd_name, cmd_dict[i].cmd_string) == 0) {
            uint32_t hw_offset = 0;
            int arg_idx = 0;
            
            // Keep looping as long as there are comma-separated values
            while (1) {
                char *token = strtok(NULL, ", )");
                if (!token) break; // Exit loop when no more arguments are found

                uint32_t val = (uint32_t)strtoul(token, NULL, 16);
                
                // If we have more tokens than min_args, lock the type to the last argument type (which is ARG_ARRAY_U8 for bursts)
                int format_idx = (arg_idx < cmd_dict[i].min_args) ? arg_idx : (cmd_dict[i].min_args - 1);
                arg_type_t current_type = cmd_dict[i].arg_format[format_idx];

                if (current_type == ARG_U8 || current_type == ARG_ARRAY_U8) {
                    uint8_t temp8 = (uint8_t)(val & 0xFF);
                    memcpy((void*)&HW_OPERAND_BASE[hw_offset], &temp8, 1);
                    hw_offset += 1;
                } 
                else if (current_type == ARG_U16) {
                    uint16_t temp16 = (uint16_t)(val & 0xFFFF);
                    memcpy((void*)&HW_OPERAND_BASE[hw_offset], &temp16, 2);
                    hw_offset += 2;
                }
                
                arg_idx++;
            }
            
            WRITE_OPCODE(cmd_dict[i].opcode);
            WRITE_CMD(1); 
            return;
        }
    }
    WRITE_OPCODE(0xFF); 
}