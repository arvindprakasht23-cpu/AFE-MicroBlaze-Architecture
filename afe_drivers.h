#ifndef AFE_DRIVERS_H
#define AFE_DRIVERS_H

#include "xil_types.h"

#define TI_AFE_RET_EXEC_PASS 0x0000
#define TI_AFE_RET_EXEC_FAIL 0x0001
#define NUM_SPI 4

/* AUTO-GENERATED PROTOTYPES START */
u32 afeSpiRawWrite(u8 afeInst, u16 addr, u8 data);
u32 afeSpiRawRead(u8 afeInst, u16 addr, u8 *readVal);
u32 afeSpiBurstWrite(u8 afeInst, u16 addr, u8 *data, u16 size);
u32 afeSpiBurstRead(u8 afeInst, u16 addr, u16 size, u8 *data);
u32 afeSpiRawWriteMulti(u8 sel, u16 addr, u8 data);
u32 afeSpiRawReadMulti(u8 sel, u16 addr, u8 *readVal);
u32 afeSpiBurstWriteMulti(u8 sel, u16 addr, u8 *data, u16 size);
/* AUTO-GENERATED PROTOTYPES END */

#endif