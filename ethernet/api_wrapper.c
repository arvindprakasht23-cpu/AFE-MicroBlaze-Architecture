#include <string.h>
#include "api_wrapper.h"
#include "axi_regs.h"
#include "afe_drivers.h"

#define MAX_BURST_SIZE 64

u16 api_afeSpiRawWrite_wrapper(volatile u8 *operands) {
    Cmd_spiRawWrite_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(Cmd_spiRawWrite_Args_t));

    return (u16)afeSpiRawWrite(args.afeInst, args.addr, args.data);
}

u16 api_afeSpiRawRead_wrapper(volatile u8 *operands) {
    Cmd_spiRawRead_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(Cmd_spiRawRead_Args_t));

    return (u16)afeSpiRawRead(args.afeInst, args.addr, (uint8_t *)HW_RESULT_BASE);
}

u16 api_afeSpiBurstWrite_wrapper(volatile u8 *operands) {
    Cmd_spiBurstWrite_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(Cmd_spiBurstWrite_Args_t));

    if (args.dataArraySize > MAX_BURST_SIZE) {
        args.dataArraySize = MAX_BURST_SIZE;
    }
    uint8_t data[MAX_BURST_SIZE];
    memcpy(data, (const void *)&operands[sizeof(Cmd_spiBurstWrite_Args_t)], args.dataArraySize);

    return (u16)afeSpiBurstWrite(args.afeInst, args.addr, args.dataArraySize, data);
}

u16 api_afeSpiBurstRead_wrapper(volatile u8 *operands) {
    Cmd_spiBurstRead_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(Cmd_spiBurstRead_Args_t));

    return (u16)afeSpiBurstRead(args.afeInst, args.addr, args.dataArraySize, (uint8_t *)HW_RESULT_BASE);
}

u16 api_afeSpiRawWriteMulti_wrapper(volatile u8 *operands) {
    Cmd_spiRawWriteMulti_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(Cmd_spiRawWriteMulti_Args_t));

    return (u16)afeSpiRawWriteMulti(args.afeInstSel, args.addr, args.data);
}

u16 api_afeSpiRawReadMulti_wrapper(volatile u8 *operands) {
    Cmd_spiRawReadMulti_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(Cmd_spiRawReadMulti_Args_t));

    return (u16)afeSpiRawReadMulti(args.afeInstSel, args.addr, (uint8_t *)HW_RESULT_BASE);
}

u16 api_afeSpiBurstWriteMulti_wrapper(volatile u8 *operands) {
    Cmd_spiBurstWriteMulti_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(Cmd_spiBurstWriteMulti_Args_t));

    if (args.dataArraySize > MAX_BURST_SIZE) {
        args.dataArraySize = MAX_BURST_SIZE;
    }
    uint8_t data[MAX_BURST_SIZE];
    memcpy(data, (const void *)&operands[sizeof(Cmd_spiBurstWriteMulti_Args_t)], args.dataArraySize);

    return (u16)afeSpiBurstWriteMulti(args.afeInstSel, args.addr, args.dataArraySize, data);
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
