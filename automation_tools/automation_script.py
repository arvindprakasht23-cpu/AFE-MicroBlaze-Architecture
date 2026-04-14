import json
import os

TYPE_MAP = {
    "U8": {"c_type": "uint8_t", "arg_enum": "ARG_U8"},
    "U16": {"c_type": "uint16_t", "arg_enum": "ARG_U16"},
    "U32": {"c_type": "uint32_t", "arg_enum": "ARG_U32"},
    "INPUT_ARRAY": {"c_type": "uint8_t", "arg_enum": "ARG_ARRAY_U8"}
}

def generate_firmware():
    try:
        with open('commands.json', 'r') as f:
            data = json.load(f)
    except FileNotFoundError:
        print("Error: commands.json not found! Run header_to_json.py first.")
        return

    commands = data['commands']
    max_args = 5 

    # --- 1. GENERATE command_dict.h ---
    with open('command_dict.h', 'w') as f:
        f.write("""/*
 * command_dict.h
 * Author: PSG_TI_TEAM
 *
 * Command dictionary structures for the UART parser.
 */

#ifndef COMMAND_DICT_H
#define COMMAND_DICT_H

#include "xil_types.h"

/* Allowed data types for parsed command arguments */
typedef enum {
    ARG_NONE = 0,    /* No argument / End of argument list */
    ARG_U8,          /* 8-bit unsigned integer (1 byte) */
    ARG_U16,         /* 16-bit unsigned integer (2 bytes) */
    ARG_U32,         /* 32-bit unsigned integer (4 bytes) */
    ARG_ARRAY_U8     /* Variable-length array of 8-bit integers (Burst data) */
} arg_type_t;

#define MAX_ARGS 5

/* Dictionary entry mapping a string command to its physical execution profile */
typedef struct {
    const char *cmd_string;
    uint8_t opcode;
    uint8_t min_args;
    arg_type_t arg_format[MAX_ARGS];
} command_meta_t;

/* Externally Linked Auto-Generated Data */
extern const command_meta_t cmd_dict[];
extern const int DICT_SIZE;

#endif // COMMAND_DICT_H
""")

    # --- 2. GENERATE api_wrapper.h ---
    with open('api_wrapper.h', 'w') as f:
        f.write("""/*
 * api_wrapper.h
 * Author: PSG_TI_TEAM
 *
 * API wrapper definitions and memory-mapped struct layouts.
 */

#ifndef API_WRAPPER_H
#define API_WRAPPER_H

#include "xil_types.h"
#include <stdint.h>

#define NUM_SPI 8
#define MAX_BURST_SIZE 64

typedef u16 (*api_func_ptr)(volatile u8 *operands);

typedef enum {\n""")
        
        for i, cmd in enumerate(commands):
            f.write(f'    {cmd["opcode"]} = {i},\n')
        f.write('    API_TABLE_SIZE\n} opcode_t;\n\n')
        
        f.write('/* ========================================================================= */\n')
        f.write('/* COMMAND ARGUMENT STRUCTS (Host -> Firmware via HW_OPERAND_BASE)           */\n')
        f.write('/* ========================================================================= */\n\n')

        for cmd in commands:
            f.write(f'typedef struct __attribute__((packed)) {{\n')
            for arg in cmd['args']:
                if "OUTPUT" not in arg["type"] and arg["type"] != "INPUT_ARRAY":
                    f.write(f'    {TYPE_MAP[arg["type"]]["c_type"]}  {arg["name"]};\n')
            f.write(f'}} Cmd_{cmd["name"]}_Args_t;\n\n')

        f.write('/* ========================================================================= */\n')
        f.write('/* RESULT REGISTER STRUCTS (Firmware -> Host via HW_RESULT_BASE)             */\n')
        f.write('/* ========================================================================= */\n\n')

        for cmd in commands:
            if cmd["has_result_struct"]:
                f.write(f'typedef struct __attribute__((packed)) {{\n')
                for arg in cmd['args']:
                    if arg["type"] == "OUTPUT_VAL":
                        f.write(f'    uint8_t {arg["name"]};\n')
                    elif arg["type"] == "OUTPUT_ARRAY_MULTI":
                        f.write(f'    uint8_t {arg["name"]}[NUM_SPI];\n')
                    elif arg["type"] == "OUTPUT_ARRAY_BURST":
                        f.write(f'    uint16_t dataArraySize;\n')
                        f.write(f'    uint8_t {arg["name"]}[MAX_BURST_SIZE];\n')
                f.write(f'}} Result_{cmd["name"]}_t;\n\n')

        f.write('extern api_func_ptr api_table[API_TABLE_SIZE];\n\n')
        f.write('#endif // API_WRAPPER_H\n')

    # --- 3. GENERATE command_dict.c (FIXED ARRAY LOCK) ---
    with open('command_dict.c', 'w') as f:
        f.write("""/*
 * command_dict.c
 * Author: PSG_TI_TEAM
 *
 * Command dictionary data layer for the UART string parser.
 */

#include "command_dict.h"
#include "../core_logic/api_wrapper.h"

const command_meta_t cmd_dict[] = {\n""")
        
        for cmd in commands:
            user_args = [arg for arg in cmd['args'] if "OUTPUT" not in arg["type"]]
            
            # ARCHITECTURE FIX: Force ARG_ARRAY_U8 to the end of the struct mapping
            enum_args = []
            array_tail = None
            for arg in user_args:
                arg_enum = TYPE_MAP[arg["type"]]["arg_enum"]
                if arg_enum == "ARG_ARRAY_U8":
                    array_tail = arg_enum
                else:
                    enum_args.append(arg_enum)
            
            if array_tail:
                enum_args.append(array_tail)

            while len(enum_args) < max_args:
                enum_args.append("ARG_NONE")
            arg_str = ", ".join(enum_args)
            f.write(f'    {{"{cmd["name"]}", {cmd["opcode"]}, {len(user_args)}, {{{arg_str}}}}},\n')
            
        f.write("};\n\n")
        f.write("const int DICT_SIZE = sizeof(cmd_dict) / sizeof(cmd_dict[0]);\n")

    # --- 4. GENERATE api_wrapper.c ---
    with open('api_wrapper.c', 'w') as f:
        f.write("""/*
 * api_wrapper.c
 * Author: PSG_TI_TEAM
 */

#include <string.h>
#include "api_wrapper.h"
#include "../axi_regs.h"
#include "afe_drivers.h"

#define MAX_BURST_SIZE 64\n\n""")

        f.write("/* ------------------------------------------------------------------ */\n")
        f.write("/* Write wrappers — no result register written                        */\n")
        f.write("/* ------------------------------------------------------------------ */\n\n")
        
        for cmd in commands:
            if cmd["has_result_struct"]: continue
            
            func_name = f"api_{cmd['driver_func']}_wrapper"
            struct_name = f"Cmd_{cmd['name']}_Args_t"
            
            f.write(f'u16 {func_name}(volatile u8 *operands) {{\n')
            f.write(f'    {struct_name} args;\n')
            f.write(f'    memcpy(&args, (const void *)operands, sizeof(args));\n')

            params = []
            has_input_array = any(a["type"] == "INPUT_ARRAY" for a in cmd['args'])
            
            if has_input_array:
                f.write('    if (args.dataArraySize > MAX_BURST_SIZE) {\n')
                f.write('        args.dataArraySize = MAX_BURST_SIZE;\n')
                f.write('    }\n')
                f.write('    uint8_t data[MAX_BURST_SIZE];\n')
                f.write('    memcpy(data, (const void *)&operands[sizeof(args)], args.dataArraySize);\n')

            for arg in cmd['args']:
                if arg["type"] == "INPUT_ARRAY":
                    params.append("data")
                else:
                    params.append(f"args.{arg['name']}")

            param_str = ", ".join(params)
            f.write(f'    return (u16){cmd["driver_func"]}({param_str});\n')
            f.write('}\n\n')

        f.write("/* ------------------------------------------------------------------ */\n")
        f.write("/* Read wrappers — result register layout matching                    */\n")
        f.write("/* ------------------------------------------------------------------ */\n\n")

        for cmd in commands:
            if not cmd["has_result_struct"]: continue
            
            func_name = f"api_{cmd['driver_func']}_wrapper"
            struct_name = f"Cmd_{cmd['name']}_Args_t"
            
            f.write(f'u16 {func_name}(volatile u8 *operands) {{\n')
            f.write(f'    {struct_name} args;\n')
            f.write(f'    memcpy(&args, (const void *)operands, sizeof(args));\n\n')

            is_burst = "BurstRead" in cmd["name"]
            if is_burst:
                f.write('    if (args.dataArraySize > MAX_BURST_SIZE) {\n')
                f.write('        args.dataArraySize = MAX_BURST_SIZE;\n')
                f.write('    }\n\n')

            f.write(f'    Result_{cmd["name"]}_t *result = (Result_{cmd["name"]}_t *)HW_RESULT_BASE;\n')
            
            if is_burst:
                f.write(f'    result->dataArraySize = args.dataArraySize;\n')

            params = []
            for arg in cmd['args']:
                if "OUTPUT" in arg["type"]:
                    if arg["type"] == "OUTPUT_VAL":
                        params.append(f"&result->{arg['name']}")
                    else:
                        params.append(f"result->{arg['name']}") 
                else:
                    params.append(f"args.{arg['name']}")

            param_str = ", ".join(params)
            
            f.write(f'    return (u16){cmd["driver_func"]}({param_str});\n')
            f.write('}\n\n')

        f.write("/* ------------------------------------------------------------------ */\n")
        f.write("/* API dispatch table — order perfectly matches opcode_t enum         */\n")
        f.write("/* ------------------------------------------------------------------ */\n\n")
        f.write('api_func_ptr api_table[API_TABLE_SIZE] = {\n')
        
        # FIXED: Iterate exactly in the order the JSON/enum was built
        for cmd in commands:
            func_name = f"api_{cmd['driver_func']}_wrapper"
            f.write(f'    {func_name},  /* Matches {cmd["opcode"]} */\n')
            
        f.write('};\n')

    print("\n✅ Generator Success! The following C wrappers have been built:")
    print("  -> command_dict.h")
    print("  -> command_dict.c")
    print("  -> api_wrapper.h")
    print("  -> api_wrapper.c\n")

if __name__ == "__main__":
    generate_firmware()