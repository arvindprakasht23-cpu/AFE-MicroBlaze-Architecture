import json

TYPE_MAP = {
    "U8":       {"c_type": "uint8_t",   "size": 1, "arg_enum": "ARG_U8"},
    "U16":      {"c_type": "uint16_t",  "size": 2, "arg_enum": "ARG_U16"},
    "U32":      {"c_type": "uint32_t",  "size": 4, "arg_enum": "ARG_U32"},
    "ARRAY_U8": {"c_type": "uint8_t *", "size": 0, "arg_enum": "ARG_ARRAY_U8"}
}

def generate_firmware():
    with open('commands.json', 'r') as f:
        data = json.load(f)
    commands = data['commands']

    # 1. Generate api_wrapper.h
    with open('api_wrapper.h', 'w') as f:
        f.write('#ifndef API_WRAPPER_H\n#define API_WRAPPER_H\n\n')
        f.write('#include "xil_types.h"\n\n')
        f.write('typedef u16 (*api_func_ptr)(volatile u8 *operands);\n\n')
        f.write('typedef enum {\n')
        for i, cmd in enumerate(commands):
            f.write(f'    {cmd["opcode"]} = {i},\n')
        f.write('    API_TABLE_SIZE\n} opcode_t;\n\n')
        f.write('extern api_func_ptr api_table[API_TABLE_SIZE];\n\n')
        f.write('#endif\n')

    # 2. Generate command_dict.c
    with open('command_dict.c', 'w') as f:
        f.write('#include "command_dict.h"\n')
        f.write('#include "api_wrapper.h"\n\n')
        f.write('const command_meta_t cmd_dict[] = {\n')
        for cmd in commands:
            user_args = [arg for arg in cmd['args'] if arg["type"] != "HW_RESULT"]
            enum_args = [TYPE_MAP[arg["type"]]["arg_enum"] for arg in user_args]
            while len(enum_args) < 5:
                enum_args.append("ARG_NONE")
            arg_str = ", ".join(enum_args)
            f.write(f'    {{"{cmd["name"]}", {cmd["opcode"]}, {len(user_args)}, {{{arg_str}}}}},\n')
        f.write('};\n\n')
        f.write('const int DICT_SIZE = sizeof(cmd_dict) / sizeof(cmd_dict[0]);\n')

    # 3. Generate afe_drivers.h
    with open('afe_drivers.h', 'w') as f:
        f.write('#ifndef AFE_DRIVERS_H\n#define AFE_DRIVERS_H\n\n')
        f.write('#include <stdint.h>\n\n')
        f.write('typedef enum RET_TYPE\n{\n')
        f.write('    TI_AFE_RET_EXEC_PASS = 0,\n')
        f.write('    TI_AFE_RET_EXEC_FAIL = 1\n')
        f.write('} RetType_e;\n\n')
        f.write('#define NUM_SPI 4\n\n')
        for cmd in commands:
            proto_args = []
            for i, arg in enumerate(cmd['args']):
                a_type = arg["type"]
                a_name = arg["name"]
                if a_type == "HW_RESULT":
                    proto_args.append(f"uint8_t *{a_name}")
                elif a_type == "ARRAY_U8":
                    size_var = cmd['args'][i-1]["name"]
                    proto_args.pop()
                    proto_args.append(f"uint8_t *{a_name}")
                    proto_args.append(f"uint16_t {size_var}")
                else:
                    proto_args.append(f'{TYPE_MAP[a_type]["c_type"]} {a_name}')
            arg_str = ", ".join(proto_args) if proto_args else "void"
            f.write(f'uint32_t {cmd["driver_func"]}({arg_str});\n')
        f.write('\n#endif\n')

    # 4. Generate api_wrapper.c
    with open('api_wrapper.c', 'w') as f:
        f.write('#include <string.h>\n')
        f.write('#include "api_wrapper.h"\n')
        f.write('#include "axi_regs.h"\n')
        f.write('#include "afe_drivers.h"\n\n')
        f.write('#define MAX_BURST_SIZE 64\n\n')

        api_table_entries = []
        for cmd in commands:
            func_name = f"api_{cmd['driver_func']}_wrapper"
            api_table_entries.append(func_name)
            f.write(f'u16 {func_name}(volatile u8 *operands) {{\n')
            offset = 0
            params = []
            for i, arg in enumerate(cmd['args']):
                a_type = arg["type"]
                a_name = arg["name"]
                if a_type == "HW_RESULT":
                    params.append("(uint8_t *)HW_RESULT_BASE")
                    continue
                c_type = TYPE_MAP[a_type]["c_type"]
                size = TYPE_MAP[a_type]["size"]
                if a_type != "ARRAY_U8":
                    f.write(f'    {c_type} {a_name};\n')
                    f.write(f'    memcpy(&{a_name}, (const void *)&operands[{offset}], {size});\n')
                    params.append(a_name)
                    offset += size
                else:
                    size_var = cmd['args'][i-1]["name"]
                    f.write(f'    uint8_t {a_name}[MAX_BURST_SIZE];\n')
                    f.write(f'    if ({size_var} > MAX_BURST_SIZE) {{\n')
                    f.write(f'        {size_var} = MAX_BURST_SIZE;\n')
                    f.write(f'    }}\n')
                    f.write(f'    memcpy({a_name}, (const void *)&operands[{offset}], {size_var});\n')
                    params.pop() 
                    params.append(a_name)
                    params.append(size_var)
            param_str = ", ".join(params)
            f.write(f'\n    return (u16){cmd["driver_func"]}({param_str});\n')
            f.write('}\n\n')

        f.write('api_func_ptr api_table[API_TABLE_SIZE] = {\n')
        for entry in api_table_entries:
            f.write(f'    {entry},\n')
        f.write('};\n')
    print("Code Generated Successfully!")

if __name__ == "__main__":
    generate_firmware()