import json

TYPE_MAP = {
    "U8":       {"c_type": "uint8_t",   "size": 1, "arg_enum": "ARG_U8"},
    "U16":      {"c_type": "uint16_t",  "size": 2, "arg_enum": "ARG_U16"},
    "U32":      {"c_type": "uint32_t",  "size": 4, "arg_enum": "ARG_U32"},
    "ARRAY_U8": {"c_type": "uint8_t *", "size": 0, "arg_enum": "ARG_ARRAY_U8"}
}

def generate_firmware():
    try:
        with open('commands.json', 'r') as f:
            data = json.load(f)
    except FileNotFoundError:
        print("Error: commands.json not found! Run header_to_json.py first.")
        return

    commands = data['commands']

    # 1. Generate api_wrapper.h (The Packed Structs)
    with open('api_wrapper.h', 'w') as f:
        f.write('#ifndef API_WRAPPER_H\n#define API_WRAPPER_H\n\n')
        f.write('#include "xil_types.h"\n')
        f.write('#include <stdint.h>\n\n')
        f.write('typedef u16 (*api_func_ptr)(volatile u8 *operands);\n\n')
        
        f.write('typedef enum {\n')
        for i, cmd in enumerate(commands):
            f.write(f'    {cmd["opcode"]} = {i},\n')
        f.write('    API_TABLE_SIZE\n} opcode_t;\n\n')

        for cmd in commands:
            f.write(f'typedef struct __attribute__((packed)) {{\n')
            for arg in cmd['args']:
                if arg["type"] not in ["HW_RESULT", "ARRAY_U8"]:
                    c_type = TYPE_MAP[arg["type"]]["c_type"]
                    f.write(f'    {c_type}  {arg["name"]};\n')
            f.write(f'}} Cmd_{cmd["name"]}_Args_t;\n\n')

        f.write('extern api_func_ptr api_table[API_TABLE_SIZE];\n\n')
        f.write('#endif // API_WRAPPER_H\n')

    # 2. Generate command_dict.c (The Parser Array)
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

    # 3. Generate api_wrapper.c (The Clean memcpy Action)
    with open('api_wrapper.c', 'w') as f:
        f.write('#include <string.h>\n')
        f.write('#include "api_wrapper.h"\n')
        f.write('#include "axi_regs.h"\n')
        f.write('#include "afe_drivers.h"\n\n')
        f.write('#define MAX_BURST_SIZE 64\n\n')

        api_table_entries = []
        for cmd in commands:
            func_name = f"api_{cmd['driver_func']}_wrapper"
            struct_name = f"Cmd_{cmd['name']}_Args_t"
            api_table_entries.append(func_name)
            
            f.write(f'u16 {func_name}(volatile u8 *operands) {{\n')
            
            f.write(f'    {struct_name} args;\n')
            f.write(f'    memcpy(&args, (const void *)operands, sizeof({struct_name}));\n')

            params = []
            has_array = False
            array_name, size_var = "", ""

            for i, arg in enumerate(cmd['args']):
                a_type = arg["type"]
                a_name = arg["name"]
                
                if a_type == "HW_RESULT":
                    params.append("(uint8_t *)HW_RESULT_BASE")
                elif a_type == "ARRAY_U8":
                    has_array = True
                    array_name = a_name
                    size_var = cmd['args'][i-1]["name"]
                    params.pop() 
                    params.append(f"args.{size_var}")
                    params.append(array_name)
                else:
                    params.append(f"args.{a_name}")

            if has_array:
                f.write(f'\n    if (args.{size_var} > MAX_BURST_SIZE) {{\n')
                f.write(f'        args.{size_var} = MAX_BURST_SIZE;\n')
                f.write(f'    }}\n')
                f.write(f'    uint8_t {array_name}[MAX_BURST_SIZE];\n')
                f.write(f'    memcpy({array_name}, (const void *)&operands[sizeof({struct_name})], args.{size_var});\n')

            param_str = ", ".join(params)
            f.write(f'\n    return (u16){cmd["driver_func"]}({param_str});\n')
            f.write('}\n\n')

        f.write('api_func_ptr api_table[API_TABLE_SIZE] = {\n')
        for entry in api_table_entries:
            f.write(f'    {entry},\n')
        f.write('};\n')
        
    print("✅ Generator Success! Enterprise Firmware written to C files.")

if __name__ == "__main__":
    generate_firmware()