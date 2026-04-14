import re
import json

# Reverse dictionary: Translates C data types back into your JSON tags
REVERSE_TYPE_MAP = {
    "uint8_t": "U8",
    "uint16_t": "U16",
    "uint32_t": "U32"
}

def parse_header_to_json(header_filename="afe_drivers.h", json_filename="commands.json"):
    commands_list = []
    
    try:
        with open(header_filename, 'r') as file:
            lines = file.readlines()
    except FileNotFoundError:
        print(f"Error: Could not find {header_filename}. Make sure the vendor header is in the same folder.")
        return

    func_pattern = re.compile(r'uint32_t\s+([a-zA-Z0-9_]+)\s*\((.*?)\);')
    opcode_counter = 0

    for line in lines:
        match = func_pattern.search(line)
        if match:
            driver_func = match.group(1) 
            args_string = match.group(2) 

            base_name = driver_func.replace("afeSpi", "spi") 
            opcode_name = "OPCODE_" + re.sub(r'(?<!^)(?=[A-Z])', '_', base_name).upper()

            command_data = {
                "name": base_name,
                "opcode": opcode_name,
                "driver_func": driver_func,
                "args": [],
                "has_result_struct": "Read" in driver_func
            }

            if args_string.strip() != "void":
                arg_list = args_string.split(',')
                for arg in arg_list:
                    arg = arg.strip()
                    if not arg: continue
                    
                    parts = arg.split()
                    if len(parts) >= 2:
                        c_type = parts[0]
                        arg_name = parts[1].replace("*", "") 
                        
                        # --- ADVANCED MEMORY DETECTION ---
                        if "*" in arg or c_type == "uint8_t*":
                            if "ReadMulti" in driver_func:
                                json_type = "OUTPUT_ARRAY_MULTI"
                            elif "BurstRead" in driver_func:
                                json_type = "OUTPUT_ARRAY_BURST"
                            elif "Read" in driver_func:
                                json_type = "OUTPUT_VAL"
                            else:
                                json_type = "INPUT_ARRAY"
                        else:
                            json_type = REVERSE_TYPE_MAP.get(c_type, "UNKNOWN")
                        
                        command_data["args"].append({
                            "type": json_type,
                            "name": arg_name,
                            "c_type": c_type.replace("*", "")
                        })
            
            commands_list.append(command_data)
            opcode_counter += 1

    with open(json_filename, 'w') as outfile:
        json.dump({"commands": commands_list}, outfile, indent=4)
        
    print(f"✅ Scraper Success! Extracted {opcode_counter} APIs with Struct Memory Mapping.")

if __name__ == "__main__":
    parse_header_to_json()