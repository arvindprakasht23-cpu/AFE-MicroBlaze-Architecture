#ifndef API_WRAPPER_H
#define API_WRAPPER_H

#include "xil_types.h"

typedef u16 (*api_func_ptr)(volatile u8 *operands);

typedef enum {
    OPCODE_RAW_WRITE = 0,
    OPCODE_RAW_READ = 1,
    OPCODE_BURST_WRITE = 2,
    OPCODE_BURST_READ = 3,
    OPCODE_RAW_WRITE_MULTI = 4,
    OPCODE_RAW_READ_MULTI = 5,
    OPCODE_BURST_WRITE_MULTI = 6,
    API_TABLE_SIZE
} opcode_t;

extern api_func_ptr api_table[API_TABLE_SIZE];

#endif