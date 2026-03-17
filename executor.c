#include "executor.h"
#include "axi_regs.h"
#include "api_wrapper.h"
#include "afe_drivers.h" 

void executor_poll(void) {
    if (READ_CMD() == 1) {
        WRITE_CMD(2); 
        
        u8 opcode = READ_OPCODE();
        u16 status = TI_AFE_RET_EXEC_FAIL; 

        if (opcode < API_TABLE_SIZE && api_table[opcode]) {
            status = api_table[opcode](HW_OPERAND_BASE); 
        }

        WRITE_STATUS(status);
        WRITE_CMD(0); 
    }
}