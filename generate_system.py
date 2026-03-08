import json
import os

def generate_system():
    print("Reading api_blueprint.json...")
    with open('api_blueprint.json', 'r') as f:
        data = json.load(f)
    functions = data['functions']

    # 1. GENERATE FPGA DEMAPPER (api.h & api.c)
    with open('api.h', 'w') as f:
        f.write("/* AUTO-GENERATED FPGA DEMAPPER HEADER */\n")
        f.write("#ifndef API_H\n#define API_H\n\n#include \"xil_types.h\"\n\n")
        f.write("typedef u16 (*api_func_ptr)(volatile u8 *operands);\n\n")
        f.write("typedef enum {\n")
        for func in functions: f.write(f"    {func['opcode_enum']},\n")
        f.write("    API_TABLE_SIZE\n} opcode_t;\n\n")
        f.write("extern api_func_ptr api_table[API_TABLE_SIZE];\n\n#endif\n")

    with open('api.c', 'w') as f:
        f.write("/* AUTO-GENERATED FPGA DEMAPPER SOURCE */\n")
        f.write("#include \"api.h\"\n#include \"axi_regs.h\"\n#include \"afe_drivers.h\"\n\n")
        for func in functions:
            f.write(f"u16 api_{func['name']}_wrapper(volatile u8 *operands) {{\n")
            offset = 0
            for p in func['payload']:
                if p['type'] == "u8":
                    f.write(f"    uint8_t  {p['name']} = operands[{offset}];\n")
                    offset += 1
                elif p['type'] == "u16":
                    f.write(f"    uint16_t {p['name']} = (uint16_t)(operands[{offset}] & 0xFF) | (uint16_t)((operands[{offset+1}] & 0xFF) << 8);\n")
                    offset += 2
            call = func['driver_call'].replace("{offset}", str(offset))
            f.write(f"    return (u16){call};\n}}\n\n")
        f.write("api_func_ptr api_table[API_TABLE_SIZE] = {\n")
        f.write(",\n".join([f"    api_{fn['name']}_wrapper" for fn in functions]))
        f.write("\n};\n")

    # 2. GENERATE FPGA MAPPER (mapper.h & mapper.c)
    with open('mapper.h', 'w') as f:
        f.write("/* AUTO-GENERATED FPGA MAPPER HEADER */\n")
        f.write("#ifndef MAPPER_H\n#define MAPPER_H\n\n#include <stdint.h>\n\n")
        f.write("uint8_t get_opcode_from_string(const char *cmd);\n\n#endif\n")

    with open('mapper.c', 'w') as f:
        f.write("/* AUTO-GENERATED FPGA MAPPER SOURCE */\n")
        f.write("#include <string.h>\n#include \"mapper.h\"\n#include \"api.h\"\n\n")
        f.write("uint8_t get_opcode_from_string(const char *cmd) {\n")
        for i, func in enumerate(functions):
            typed_name = func['name'].replace("afeSpi", "spi")
            if i == 0:
                f.write(f"    if (strcmp(cmd, \"{typed_name}\") == 0) return {func['opcode_enum']};\n")
            else:
                f.write(f"    else if (strcmp(cmd, \"{typed_name}\") == 0) return {func['opcode_enum']};\n")
        f.write("    return 0xFF;\n}\n")

    # 3. UPDATE AFE_DRIVERS.H SAFELY
    print("Updating afe_drivers.h...")
    start_marker = "/* AUTO-GENERATED PROTOTYPES START */\n"
    end_marker = "/* AUTO-GENERATED PROTOTYPES END */"
    
    if os.path.exists('afe_drivers.h'):
        with open('afe_drivers.h', 'r') as f:
            header_content = f.read()
            
        if start_marker in header_content and end_marker in header_content:
            new_prototypes = ""
            for func in functions:
                new_prototypes += f"{func['prototype']};\n"
                
            top_part = header_content.split(start_marker)[0]
            bottom_part = header_content.split(end_marker)[1]
            updated_content = top_part + start_marker + new_prototypes + end_marker + bottom_part
            
            with open('afe_drivers.h', 'w') as f:
                f.write(updated_content)
            print("Successfully injected prototypes into afe_drivers.h")
        else:
            print("ERR: Could not find START/END markers in afe_drivers.h.")
    else:
        print("ERR: afe_drivers.h not found. Create it first with the comment markers.")

if __name__ == "__main__":
    generate_system()