#include <string.h>
#include "api_wrapper.h"
#include "axi_regs.h"
#include "afe_drivers.h"

#define MAX_BURST_SIZE 64

u16 api_afeSpiRawWrite_wrapper(volatile u8 *operands) {
    uint8_t afeInst;
    memcpy(&afeInst, (const void *)&operands[0], 1);
    uint16_t addr;
    memcpy(&addr, (const void *)&operands[1], 2);
    uint8_t data;
    memcpy(&data, (const void *)&operands[3], 1);
    return (u16)afeSpiRawWrite(afeInst, addr, data);
}

u16 api_afeSpiRawRead_wrapper(volatile u8 *operands) {
    uint8_t afeInst;
    memcpy(&afeInst, (const void *)&operands[0], 1);
    uint16_t addr;
    memcpy(&addr, (const void *)&operands[1], 2);
    return (u16)afeSpiRawRead(afeInst, addr, (uint8_t *)HW_RESULT_BASE);
}

u16 api_afeSpiBurstWrite_wrapper(volatile u8 *operands) {
    uint8_t afeInst;
    memcpy(&afeInst, (const void *)&operands[0], 1);
    uint16_t addr;
    memcpy(&addr, (const void *)&operands[1], 2);
    uint16_t dataArraySize;
    memcpy(&dataArraySize, (const void *)&operands[3], 2);
    uint8_t data[MAX_BURST_SIZE];
    if (dataArraySize > MAX_BURST_SIZE) {
        dataArraySize = MAX_BURST_SIZE;
    }
    memcpy(data, (const void *)&operands[5], dataArraySize);
    return (u16)afeSpiBurstWrite(afeInst, addr, data, dataArraySize);
}

u16 api_afeSpiBurstRead_wrapper(volatile u8 *operands) {
    uint8_t afeInst;
    memcpy(&afeInst, (const void *)&operands[0], 1);
    uint16_t addr;
    memcpy(&addr, (const void *)&operands[1], 2);
    uint16_t dataArraySize;
    memcpy(&dataArraySize, (const void *)&operands[3], 2);
    return (u16)afeSpiBurstRead(afeInst, addr, dataArraySize, (uint8_t *)HW_RESULT_BASE);
}

u16 api_afeSpiRawWriteMulti_wrapper(volatile u8 *operands) {
    uint8_t afeInstSel;
    memcpy(&afeInstSel, (const void *)&operands[0], 1);
    uint16_t addr;
    memcpy(&addr, (const void *)&operands[1], 2);
    uint8_t data;
    memcpy(&data, (const void *)&operands[3], 1);
    return (u16)afeSpiRawWriteMulti(afeInstSel, addr, data);
}

u16 api_afeSpiRawReadMulti_wrapper(volatile u8 *operands) {
    uint8_t afeInstSel;
    memcpy(&afeInstSel, (const void *)&operands[0], 1);
    uint16_t addr;
    memcpy(&addr, (const void *)&operands[1], 2);
    return (u16)afeSpiRawReadMulti(afeInstSel, addr, (uint8_t *)HW_RESULT_BASE);
}

u16 api_afeSpiBurstWriteMulti_wrapper(volatile u8 *operands) {
    uint8_t afeInstSel;
    memcpy(&afeInstSel, (const void *)&operands[0], 1);
    uint16_t addr;
    memcpy(&addr, (const void *)&operands[1], 2);
    uint16_t dataArraySize;
    memcpy(&dataArraySize, (const void *)&operands[3], 2);
    uint8_t data[MAX_BURST_SIZE];
    if (dataArraySize > MAX_BURST_SIZE) {
        dataArraySize = MAX_BURST_SIZE;
    }
    memcpy(data, (const void *)&operands[5], dataArraySize);
    return (u16)afeSpiBurstWriteMulti(afeInstSel, addr, data, dataArraySize);
}

api_func_ptr api_table[API_TABLE_SIZE] = {
    api_afeSpiRawWrite_wrapper,
    api_afeSpiRawRead_wrapper,
    api_afeSpiBurstWrite_wrapper,
    api_afeSpiBurstRead_wrapper,
    api_afeSpiRawWriteMulti_wrapper,
    api_afeSpiRawReadMulti_wrapper,
    api_afeSpiBurstWriteMulti_wrapper,
};