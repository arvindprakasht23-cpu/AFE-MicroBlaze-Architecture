/* AUTO-GENERATED FPGA DEMAPPER HEADER */
#ifndef API_H
#define API_H

#include "xil_types.h"

typedef u16 (*api_func_ptr)(volatile u8 *operands);

typedef enum {
    OPCODE_RAW_WRITE,
    OPCODE_RAW_READ,
    OPCODE_BURST_WRITE,
    OPCODE_BURST_READ,
    OPCODE_RAW_WRITE_MULTI,
    OPCODE_RAW_READ_MULTI,
    OPCODE_BURST_WRITE_MULTI,
    API_TABLE_SIZE
} opcode_t;

extern api_func_ptr api_table[API_TABLE_SIZE];

#endif
