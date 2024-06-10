#ifndef TASK_INTERBOARD_H
#define TASK_INTERBOARD_H

#include "Std_Types.h"

void Task_Send_BoardInfo(void *params);

void Board_Rx_Init(void);

Std_ReturnType Get_Board_Data(uint8_t * rx_buffer, uint8_t rx_len, uint32_t timeout);

#endif /* TASK_INTERBOARD_H */
