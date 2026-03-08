#include "afe_drivers.h"
#include "xil_printf.h"

u32 afeSpiRawWrite(u8 afeInst, u16 addr, u8 data) {
    xil_printf("\r\n[DRIVER] afeSpiRawWrite: Inst=0x%02X, Addr=0x%04X, Data=0x%02X\r\n", afeInst, addr, data);
    return TI_AFE_RET_EXEC_PASS;
}

u32 afeSpiRawRead(u8 afeInst, u16 addr, u8 *readVal) {
    xil_printf("\r\n[DRIVER] afeSpiRawRead: Inst=0x%02X, Addr=0x%04X\r\n", afeInst, addr);
    *readVal = 0xAA; // Mock return value
    return TI_AFE_RET_EXEC_PASS;
}

u32 afeSpiBurstWrite(u8 afeInst, u16 addr, u8 *data, u16 size) {
    xil_printf("\r\n[DRIVER] afeSpiBurstWrite: Inst=0x%02X, Addr=0x%04X, Size=%d\r\n", afeInst, addr, size);
    for(int i=0; i<size; i++) xil_printf("   Data[%d]=0x%02X\r\n", i, data[i]);
    return TI_AFE_RET_EXEC_PASS;
}

u32 afeSpiBurstRead(u8 afeInst, u16 addr, u16 size, u8 *data) {
    xil_printf("\r\n[DRIVER] afeSpiBurstRead: Inst=0x%02X, Addr=0x%04X, Size=%d\r\n", afeInst, addr, size);
    for(int i=0; i<size; i++) data[i] = 0xBB + i; // Mock return array
    return TI_AFE_RET_EXEC_PASS;
}

u32 afeSpiRawWriteMulti(u8 sel, u16 addr, u8 data) {
    xil_printf("\r\n[DRIVER] afeSpiRawWriteMulti: Sel=0x%02X, Addr=0x%04X, Data=0x%02X\r\n", sel, addr, data);
    return TI_AFE_RET_EXEC_PASS;
}

u32 afeSpiRawReadMulti(u8 sel, u16 addr, u8 *readVal) {
    xil_printf("\r\n[DRIVER] afeSpiRawReadMulti: Sel=0x%02X, Addr=0x%04X\r\n", sel, addr);
    for(int i=0; i<NUM_SPI; i++) readVal[i] = 0x10 + i; // Mock returns for 4 SPIs
    return TI_AFE_RET_EXEC_PASS;
}

u32 afeSpiBurstWriteMulti(u8 sel, u16 addr, u8 *data, u16 size) {
    xil_printf("\r\n[DRIVER] afeSpiBurstWriteMulti: Sel=0x%02X, Addr=0x%04X, Size=%d\r\n", sel, addr, size);
    for(int i=0; i<size; i++) xil_printf("   Data[%d]=0x%02X\r\n", i, data[i]);
    return TI_AFE_RET_EXEC_PASS;
}