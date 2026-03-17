#ifndef AFE_DRIVERS_H
#define AFE_DRIVERS_H

#include <stdint.h>

typedef enum RET_TYPE
{
    TI_AFE_RET_EXEC_PASS = 0,
    TI_AFE_RET_EXEC_FAIL = 1
} RetType_e;

#define NUM_SPI 4

// --- OFFICIAL DRIVER PROTOTYPES ---
uint32_t afeSpiRawWrite(uint8_t afeInst, uint16_t addr, uint8_t data);
uint32_t afeSpiRawRead(uint8_t afeInst, uint16_t addr, uint8_t *readVal);
uint32_t afeSpiBurstWrite(uint8_t afeInst, uint16_t addr, uint8_t *data, uint16_t dataArraySize);
uint32_t afeSpiBurstRead(uint8_t afeInst, uint16_t addr, uint16_t dataArraySize, uint8_t *data);
uint32_t afeSpiRawWriteMulti(uint8_t afeInstSel, uint16_t addr, uint8_t data);
uint32_t afeSpiRawReadMulti(uint8_t afeInstSel, uint16_t addr, uint8_t *readVal);
uint32_t afeSpiBurstWriteMulti(uint8_t afeInstSel, uint16_t addr, uint8_t *data, uint16_t dataArraySize);

#endif