#ifndef AXI_REGS_H
#define AXI_REGS_H

#include <stdint.h>
#include "xil_types.h"

// Base address of your custom AXI IP
#define REG_BASE_ADDR 0x44A00000

#define HW_OPERAND_BASE ((volatile u8 *)(REG_BASE_ADDR + 0x00)) 
#define HW_RESULT_BASE  ((volatile u8 *)(REG_BASE_ADDR + 0x24)) 

#define REG_OPCODE      ((volatile u8 *)(REG_BASE_ADDR + 0x44))
#define REG_CMD         ((volatile u8 *)(REG_BASE_ADDR + 0x48))
#define REG_STATUS      ((volatile u16 *)(REG_BASE_ADDR + 0x4C))

#define WRITE_OPCODE(val) (*REG_OPCODE = (val))
#define READ_OPCODE()     (*REG_OPCODE)
#define WRITE_CMD(val)    (*REG_CMD = (val))
#define READ_CMD()        (*REG_CMD)
#define WRITE_STATUS(val) (*REG_STATUS = (val))
#define READ_STATUS()     (*REG_STATUS)

#endif