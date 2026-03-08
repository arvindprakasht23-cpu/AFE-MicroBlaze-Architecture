/* AUTO-GENERATED FPGA DEMAPPER SOURCE */
#include "api.h"
#include "axi_regs.h"
#include "afe_drivers.h"

u16 api_afeSpiRawWrite_wrapper(volatile u8 *operands) {
    uint8_t  afeInst = operands[0];
    uint16_t addr = (uint16_t)(operands[1] & 0xFF) | (uint16_t)((operands[2] & 0xFF) << 8);
    uint8_t  data = operands[3];
    return (u16)afeSpiRawWrite(afeInst, addr, data);
}

u16 api_afeSpiRawRead_wrapper(volatile u8 *operands) {
    uint8_t  afeInst = operands[0];
    uint16_t addr = (uint16_t)(operands[1] & 0xFF) | (uint16_t)((operands[2] & 0xFF) << 8);
    return (u16)afeSpiRawRead(afeInst, addr, (uint8_t *)HW_RESULT_BASE);
}

u16 api_afeSpiBurstWrite_wrapper(volatile u8 *operands) {
    uint8_t  afeInst = operands[0];
    uint16_t addr = (uint16_t)(operands[1] & 0xFF) | (uint16_t)((operands[2] & 0xFF) << 8);
    uint16_t size = (uint16_t)(operands[3] & 0xFF) | (uint16_t)((operands[4] & 0xFF) << 8);
    return (u16)afeSpiBurstWrite(afeInst, addr, (uint8_t *)&operands[5], size);
}

u16 api_afeSpiBurstRead_wrapper(volatile u8 *operands) {
    uint8_t  afeInst = operands[0];
    uint16_t addr = (uint16_t)(operands[1] & 0xFF) | (uint16_t)((operands[2] & 0xFF) << 8);
    uint16_t size = (uint16_t)(operands[3] & 0xFF) | (uint16_t)((operands[4] & 0xFF) << 8);
    return (u16)afeSpiBurstRead(afeInst, addr, size, (uint8_t *)HW_RESULT_BASE);
}

u16 api_afeSpiRawWriteMulti_wrapper(volatile u8 *operands) {
    uint8_t  sel = operands[0];
    uint16_t addr = (uint16_t)(operands[1] & 0xFF) | (uint16_t)((operands[2] & 0xFF) << 8);
    uint8_t  data = operands[3];
    return (u16)afeSpiRawWriteMulti(sel, addr, data);
}

u16 api_afeSpiRawReadMulti_wrapper(volatile u8 *operands) {
    uint8_t  sel = operands[0];
    uint16_t addr = (uint16_t)(operands[1] & 0xFF) | (uint16_t)((operands[2] & 0xFF) << 8);
    return (u16)afeSpiRawReadMulti(sel, addr, (uint8_t *)HW_RESULT_BASE);
}

u16 api_afeSpiBurstWriteMulti_wrapper(volatile u8 *operands) {
    uint8_t  sel = operands[0];
    uint16_t addr = (uint16_t)(operands[1] & 0xFF) | (uint16_t)((operands[2] & 0xFF) << 8);
    uint16_t size = (uint16_t)(operands[3] & 0xFF) | (uint16_t)((operands[4] & 0xFF) << 8);
    return (u16)afeSpiBurstWriteMulti(sel, addr, (uint8_t *)&operands[5], size);
}

api_func_ptr api_table[API_TABLE_SIZE] = {
    api_afeSpiRawWrite_wrapper,
    api_afeSpiRawRead_wrapper,
    api_afeSpiBurstWrite_wrapper,
    api_afeSpiBurstRead_wrapper,
    api_afeSpiRawWriteMulti_wrapper,
    api_afeSpiRawReadMulti_wrapper,
    api_afeSpiBurstWriteMulti_wrapper
};
