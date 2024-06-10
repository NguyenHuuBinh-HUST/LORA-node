
#include "usart.h"
#include "serial_pipe_hal.h"





#include "task_LoRa.h" 
extern ExtBoard_t detector_board;
uint8_t Uart5_Rx_indx = 0, Uart5_Rx_data[2], Uart5_Rx_Buffer[100], Transfer_cplt; //Khai bao cac bien de nhan du lieu
uint8_t Data_Detect = 0;
uint8_t Rx_index=0;
uint8_t Rx_buffer[100];
uint8_t Rx_data[1];
extern uint8_t state_init ;
extern LoRaSV_PROCESS_STATE_t lps;
SerialPipe::SerialPipe(int rxSize): 
	_pipeRx(rxSize)
{
}

SerialPipe::~SerialPipe(void)
{
}

// rx channel
int SerialPipe::readable(void)
{
    return _pipeRx.readable();
}

int SerialPipe::getc(void)
{
    if (!_pipeRx.readable())
        return EOF;
    return _pipeRx.getc();
}

int SerialPipe::get(void* buffer, int length, bool blocking)
{
    return _pipeRx.get((char*)buffer,length,blocking);
}

void SerialPipe::rxIRQBuf(char c)
{
	if (_pipeRx.writeable())
        _pipeRx.putc(c);
    else
        /* overflow */;
}

/*******************************************************************************
 * Function Name  : HAL_USART_Handler
 * Description    : This function handles USART global interrupt request.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	__HAL_UART_FLUSH_DRREGISTER(UartHandle); // Clear the buffer to prevent overrun
	
	
if(UartHandle->Instance == detector_board.huart->Instance)
	{
		
		if(detector_board.cpltUartdata != 1)
		{
			if(detector_board.rxTmp[0] != '\n')
		  {
				detector_board.rxBuffer[detector_board.rxIndex++] = detector_board.rxTmp[0];
				Uart5_Rx_Buffer[Uart5_Rx_indx++] = detector_board.rxTmp[0];
			}
			else
			{
				
				detector_board.cpltUartdata = 1;
				detector_board.dataLen = detector_board.rxIndex;
				detector_board.rxIndex = 0;
				if(lps != LoRaSV_STATE_TX)
				{
					//state_init = 1;
					lps = LoRaSV_STATE_TX;
		
				}
			}
		}
		HAL_UART_Receive_IT(detector_board.huart,(uint8_t *)detector_board.rxTmp,1);
	}
//		if(Rx_index==0)
//			{
//				for(int i=0;i<100;i++){
//					 Rx_buffer[i] = 0;
//				}
//			}
//		if(Rx_data[0]!='\n')
//			{
//				Rx_buffer[Rx_index++]=Rx_data[0];
//			}
//		else
//			{
//				if(Transfer_cplt != 1)
//				{	
//					Transfer_cplt = 1;
//					memcpy(Uart5_Rx_Buffer,Rx_buffer,Rx_index);
//				}
//				if(detector_board.cpltUartdata != 1)
//				{
//					detector_board.cpltUartdata = 1;
//					memcpy(detector_board.rxBuffer,Rx_buffer,Rx_index);
//					detector_board.dataLen = Rx_index+1;
//				}
//					Rx_index = 0;
//			}
//			HAL_UART_Receive_IT(detector_board.huart,Rx_data,1);
		//HAL_UART_Receive_IT(detector_board.huart,(uint8_t *)detector_board.rxTmp,1); // Enable uart receive interupt
		//HAL_UART_Receive_IT(detector_board.huart,detector_board.rxBuffer,sizeof(detector_board.rxBuffer));
	
	
}
/* Get data from the board that connect to UART 5 */
Std_ReturnType UART6_GetData(uint8_t * buffer, uint8_t * len, uint32_t Timeout)
{
//	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3); //Request for data
	
	uint32_t TimeStamp = HAL_GetTick();
	
	while(Transfer_cplt == 0)
	{
		/* Condition to avoid an overflow (DO NOT REMOVE) */
		if(HAL_GetTick() < TimeStamp)
		{
			TimeStamp = HAL_GetTick();
		}
		
		if((HAL_GetTick() - TimeStamp) > Timeout)
		{
			break;
		}
	}
	
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //end Request
	
	if(Transfer_cplt == 1)
	{
		memcpy(buffer, Uart5_Rx_Buffer, Uart5_Rx_indx+1);
		memset(Uart5_Rx_Buffer, 0, Uart5_Rx_indx+1);
		*len = Uart5_Rx_indx+1;
		Uart5_Rx_indx=0;
		Transfer_cplt = 0;
	}
		
	return STD_OK;
}