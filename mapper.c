/* AUTO-GENERATED FPGA MAPPER SOURCE */
#include <string.h>
#include "mapper.h"
#include "api.h"

uint8_t get_opcode_from_string(const char *cmd) {
    if (strcmp(cmd, "spiRawWrite") == 0) return OPCODE_RAW_WRITE;
    else if (strcmp(cmd, "spiRawRead") == 0) return OPCODE_RAW_READ;
    else if (strcmp(cmd, "spiBurstWrite") == 0) return OPCODE_BURST_WRITE;
    else if (strcmp(cmd, "spiBurstRead") == 0) return OPCODE_BURST_READ;
    else if (strcmp(cmd, "spiRawWriteMulti") == 0) return OPCODE_RAW_WRITE_MULTI;
    else if (strcmp(cmd, "spiRawReadMulti") == 0) return OPCODE_RAW_READ_MULTI;
    else if (strcmp(cmd, "spiBurstWriteMulti") == 0) return OPCODE_BURST_WRITE_MULTI;
    return 0xFF;
}
