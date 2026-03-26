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

    # Regex to find functions like: uint32_t afeSpiRawWrite(uint8_t afeInst, uint16_t addr, ...);
    func_pattern = re.compile(r'uint32_t\s+([a-zA-Z0-9_]+)\s*\((.*?)\);')
    opcode_counter = 0

    for line in lines:
        match = func_pattern.search(line)
        if match:
            driver_func = match.group(1) # e.g., "afeSpiRawWrite"
            args_string = match.group(2) # e.g., "uint8_t afeInst, uint16_t addr"

            # Auto-generate names and opcodes
            base_name = driver_func.replace("afeSpi", "spi") 
            opcode_name = "OPCODE_" + re.sub(r'(?<!^)(?=[A-Z])', '_', base_name).upper()

            command_data = {
                "name": base_name,
                "opcode": opcode_name,
                "driver_func": driver_func,
                "args": []
            }

            if args_string.strip() != "void":
                arg_list = args_string.split(',')
                for arg in arg_list:
                    arg = arg.strip()
                    if not arg: continue
                    
                    parts = arg.split()
                    if len(parts) >= 2:
                        c_type = parts[0]
                        arg_name = parts[1].replace("*", "") # Clean pointers
                        
                        # Smart Logic to detect Arrays vs Hardware Result pointers
                        if "*" in arg or c_type == "uint8_t*":
                            if "readVal" in arg_name or ("data" in arg_name and "Read" in driver_func):
                                json_type = "HW_RESULT"
                            else:
                                json_type = "ARRAY_U8"
                        else:
                            json_type = REVERSE_TYPE_MAP.get(c_type, "UNKNOWN")
                        
                        command_data["args"].append({
                            "type": json_type,
                            "name": arg_name
                        })
            
            commands_list.append(command_data)
            opcode_counter += 1

    # Write the JSON file safely
    with open(json_filename, 'w') as outfile:
        json.dump({"commands": commands_list}, outfile, indent=4)
        
    print(f"✅ Scraper Success! Extracted {opcode_counter} functions into {json_filename}.")

if __name__ == "__main__":
    parse_header_to_json()