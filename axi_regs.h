#ifndef AXI_REGS_H
#define AXI_REGS_H

#include "xparameters.h"
#include "xil_types.h"
#include "xil_io.h"

#define REG_BASE_ADDR  XPAR_REG_32IP_0_SREG_AXI_BASEADDR
#define HW_OPERAND_BASE ((volatile u8 *)(REG_BASE_ADDR + 0x00)) 
#define HW_RESULT_BASE  ((volatile u8 *)(REG_BASE_ADDR + 0x24)) 
#define REG_OPCODE      0x20 
#define REG_COMMAND     0x21 
#define REG_STATUS      0x22 

#define WRITE_OPCODE(val)  Xil_Out8(REG_BASE_ADDR + REG_OPCODE, (u8)(val))
#define READ_OPCODE()      Xil_In8(REG_BASE_ADDR + REG_OPCODE)
#define WRITE_CMD(val)     Xil_Out8(REG_BASE_ADDR + REG_COMMAND, (u8)(val))
#define READ_CMD()         Xil_In8(REG_BASE_ADDR + REG_COMMAND)
#define WRITE_STATUS(val)  Xil_Out16(REG_BASE_ADDR + REG_STATUS, (u16)(val))
#define READ_STATUS()      Xil_In16(REG_BASE_ADDR + REG_STATUS)

#endif