#include <stdlib.h>
#include "sx127x.h"

/*!
 * Sx127x modem configuration choice
 */
 
static const ModemConfig MODEM_CONFIG_TABLE[] =
{
    //  1d,     1e,      26
    { 0x72,   0x74,    0x00}, // Bw125Cr45Sf128 (the chip default)
    { 0x92,   0x74,    0x00}, // Bw500Cr45Sf128
    { 0x48,   0x94,    0x00}, // Bw31_25Cr48Sf512
    { 0x78,   0xc4,    0x00}, // Bw125Cr48Sf4096
    { 0x78,   0xa4,    0x00}, // Bw125Cr48Sf1024
};

/*
 * Private functions prototypes
 */
 
/*!
 * \brief Delay in milisecond
 *
 * \param [IN] Miliseconds
 * 
 * \retval Non-Return
 */
static void DelayMs(uint32_t ms);

/*!
 * \brief Get PA Select
 *
 * \param [IN] Power
 * 
 * \retval Kind of select
 */
static uint8_t Sx127x_GetPaSelect(int8_t power);
/*
 * Sx127x driver functions implementation
 */
 
static void DelayMs(uint32_t ms)
{
#ifdef USING_RTOS
	osDelay(ms);
#else
	HAL_Delay(ms);
#endif
	
}

void Sx127x_Reset(Sx127x_t *sx127x)
{
	HAL_GPIO_WritePin (sx127x->reset_pin.gpio_port,sx127x->reset_pin.gpio_pin,GPIO_PIN_RESET);
	DelayMs(10);
	HAL_GPIO_WritePin (sx127x->reset_pin.gpio_port,sx127x->reset_pin.gpio_pin,GPIO_PIN_SET);
	DelayMs(10);
}

Std_ReturnType Sx127x_Interface(Sx127x_t *sx127x, uint8_t *buffer_tx, uint8_t *buffer_rx, uint16_t size)
{
	Std_ReturnType ret;
	
	if(sx127x_cmd.write_read != NULL)
	{
		HAL_GPIO_WritePin (sx127x->nss_pin.gpio_port,sx127x->nss_pin.gpio_pin,GPIO_PIN_RESET);
		
		if(sx127x_cmd.write_read(sx127x->hspi,buffer_tx,buffer_rx,size,HAL_MAX_DELAY)==HAL_OK)
		{
			ret = STD_OK;
		}
		else
		{
			ret = STD_NOT_OK;
		}
		
		HAL_GPIO_WritePin (sx127x->nss_pin.gpio_port,sx127x->nss_pin.gpio_pin,GPIO_PIN_SET);
		
		if (ret == STD_NOT_OK) 
		{
			return ret;
		}
	}
	
	return ret;
}

Std_ReturnType Sx127x_ReadReg(Sx127x_t *sx127x, uint8_t addr, uint8_t *reg_value)
{
	Std_ReturnType ret = STD_NOT_OK;
	
	uint8_t buf_tx [CMD_BUFFER_LEN];
	uint8_t buf_rx [CMD_BUFFER_LEN];
	
	buf_tx[0] = addr & 0x7F;
	
	ret = Sx127x_Interface(sx127x, buf_tx, buf_rx, CMD_BUFFER_LEN);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	*reg_value = buf_rx[1];
	
	return STD_OK;
}

Std_ReturnType Sx127x_WriteReg(Sx127x_t *sx127x, uint8_t addr, uint8_t reg_value)
{
	Std_ReturnType ret = STD_NOT_OK;
	
	uint8_t buf_tx [CMD_BUFFER_LEN];
	uint8_t buf_rx [CMD_BUFFER_LEN];
	
	buf_tx[0] = addr | 0x80;
	buf_tx[1] = reg_value;
	
	ret = Sx127x_Interface(sx127x, buf_tx, buf_rx, CMD_BUFFER_LEN);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetOpMode(Sx127x_t *sx127x, uint8_t opMode)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t reg_value = 0;
	
	ret = Sx127x_ReadReg(sx127x, SX127X_REG_01_OP_MODE, &reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	reg_value &= SX127X_OPMODE_MASK;
	reg_value |= opMode;
	
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_01_OP_MODE, reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return ret;
}

Std_ReturnType Sx127x_SetModeStdby(Sx127x_t *sx127x)
{
	Std_ReturnType ret = STD_NOT_OK;
	
	if(sx127x->settings.State == RF_STDBY)
	{
		return STD_OK;
	}
	
	ret = Sx127x_SetOpMode(sx127x, SX127X_MODE_STDBY);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	sx127x->settings.State = RF_STDBY;
	
	DelayMs(1);
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetModeSleep(Sx127x_t *sx127x)
{
	Std_ReturnType ret = STD_NOT_OK;
	
	if(sx127x->settings.State == RF_SLEEP)
	{
		return STD_OK;
	}
	
	ret = Sx127x_SetOpMode(sx127x, SX127X_MODE_SLEEP);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	sx127x->settings.State = RF_SLEEP;
	
	DelayMs(1);
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetModeTx(Sx127x_t *sx127x)
{
	Std_ReturnType ret = STD_NOT_OK;
	
	if(sx127x->settings.State == RF_TX_RUNNING)
	{
		return STD_OK;
	}
	
	ret = Sx127x_SetOpMode(sx127x, SX127X_MODE_TX);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_40_DIO_MAPPING1 , 0x40); // Interrupt on TxDone
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	sx127x->settings.State = RF_TX_RUNNING;
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetModeRx(Sx127x_t *sx127x)
{
	Std_ReturnType ret = STD_NOT_OK;
	
	if(sx127x->settings.State == RF_RX_RUNNING)
	{
		return STD_OK;
	}
	
	ret = Sx127x_SetOpMode(sx127x, SX127X_MODE_RXCONTINUOUS);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_40_DIO_MAPPING1 , 0x00); // Interrupt on RxDone
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	sx127x->settings.State = RF_RX_RUNNING;
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetModeCAD(Sx127x_t *sx127x)
{
	Std_ReturnType ret = STD_NOT_OK;
	
	if(sx127x->settings.State == RF_CAD)
	{
		return STD_OK;
	}
	
	ret = Sx127x_SetOpMode(sx127x, SX127X_MODE_CAD);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_40_DIO_MAPPING1 , 0x80); // Interrupt on CadDone
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	sx127x->settings.State = RF_CAD;
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetModemConfig(Sx127x_t *sx127x, ModemConfigChoice index)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t reg_value = 0;
	
	/* Check if index > number of choice */
	if ((index+1) > (signed int)(sizeof(MODEM_CONFIG_TABLE) / sizeof(ModemConfig)))
	{
		return STD_NOT_OK;
	}
	
	reg_value = MODEM_CONFIG_TABLE[index].reg_1d;
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_1D_MODEM_CONFIG1, reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	reg_value = MODEM_CONFIG_TABLE[index].reg_1e;
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_1E_MODEM_CONFIG2, reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	reg_value = MODEM_CONFIG_TABLE[index].reg_26;
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_26_MODEM_CONFIG3, reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetPreambleLength(Sx127x_t *sx127x, uint16_t bytes)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t reg_value = 0;
	
	reg_value = bytes >> 8;
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_20_PREAMBLE_MSB, reg_value);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	reg_value = bytes & 0xff;
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_21_PREAMBLE_LSB, reg_value);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetFrequency(Sx127x_t *sx127x, float centre)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t reg_value = 0;
	
	uint32_t frf = (centre * 1000000.0) / SX127X_FSTEP; // Frf = FRF / FSTEP
	
	reg_value = (frf >> 16) & 0xff;
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_06_FRF_MSB, reg_value);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	reg_value = (frf >> 8) & 0xff;
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_07_FRF_MID, reg_value);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	reg_value =  frf & 0xff;
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_08_FRF_LSB, reg_value);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetTxPower(Sx127x_t *sx127x, int8_t power, Std_SwitchType useRFO)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t reg_value = 0;
	
	if (useRFO == STD_ON )
	{
		if (power > 14) power = 14;
		
		if (power < -1) power = -1;
		
		reg_value = (SX127X_MAX_POWER | (power + 1));
		ret = Sx127x_WriteReg(sx127x,SX127X_REG_09_PA_CONFIG,reg_value);
		if(ret == STD_NOT_OK)
		{
			return ret;
		}
	}
	else
	{
		if (power > 23) power = 23;
		
		if (power < 5) power = 5;
		
		// For RFM95_PA_DAC_ENABLE, manual says '+20dBm on PA_BOOST when OutputPower=0xf'
		// RFM95_PA_DAC_ENABLE actually adds about 3dBm to all power levels. We will us it
		// for 21, 22 and 23dBm
		
		if (power > 20)
		{
			reg_value = SX127X_PA_DAC_ENABLE;
			ret = Sx127x_WriteReg(sx127x,SX127X_REG_4D_PA_DAC,reg_value);
			if(ret == STD_NOT_OK)
			{
				return ret;
			}
			power -= 3;
		}
		else
		{
			reg_value = SX127X_PA_DAC_DISABLE;
			ret = Sx127x_WriteReg(sx127x,SX127X_REG_4D_PA_DAC,reg_value);
			if(ret == STD_NOT_OK)
			{
				return ret;
			}
		}
		
		// RFM95/96/97/98 does not have RFO pins connected to anything. Only PA_BOOST
		// pin is connected, so must use PA_BOOST
		// Pout = 2 + OutputPower.
		// The documentation is pretty confusing on this topic: PaSelect says the max power is 20dBm,
		// but OutputPower claims it would be 17dBm.
		// My measurements show 20dBm is correct
		
		reg_value = (SX127X_PA_SELECT | (power-5));
		ret = Sx127x_WriteReg(sx127x,SX127X_REG_09_PA_CONFIG,reg_value);
		if(ret == STD_NOT_OK)
		{
			return ret;
		}
	}
	
	return STD_OK;
}


	
//Std_ReturnType Sx127x_SetTxPower(Sx127x_t *sx127x, int8_t power)
//{
//	Std_ReturnType ret = STD_NOT_OK;
//	uint8_t reg_value = 0;
//	
//	uint8_t paConfig = 0;
//	uint8_t paDac = 0;

//	ret = Sx127x_ReadReg(sx127x, SX127X_REG_09_PA_CONFIG, &paConfig);
//	if (ret == STD_NOT_OK)
//	{
//		return ret;
//	}
//	
//	ret = Sx127x_ReadReg(sx127x, SX127X_REG_4D_PA_DAC, &paDac);
//	if (ret == STD_NOT_OK)
//	{
//		return ret;
//	}
//	
//  paConfig = ( paConfig & RF_PACONFIG_PASELECT_MASK ) | Sx127x_GetPaSelect( power );

//  if( ( paConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
//  {
//		if( power > 17 )
//		{
//			paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_ON;
//		}
//		else
//		{
//			paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_OFF;
//		}
//		if(( paDac & RF_PADAC_20DBM_ON ) == RF_PADAC_20DBM_ON)
//		{
//			if(power < 5)
//			{
//				power = 5;
//			}
//			if(power > 20)
//			{
//				power = 20;
//			}
//			paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) | (uint8_t)((uint16_t)(power - 5) & 0x0F);
//		}
//		else
//		{
//			if(power < 2)
//			{
//				power = 2;
//			}
//			if(power > 17)
//			{
//				power = 17;
//			}
//			paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) | (uint8_t)( (uint16_t)(power - 2) & 0x0F);
//		}
//	}
//	else
//	{
//		if(power > 0)
//		{
//			if(power > 15)
//			{
//				power = 15;
//			}
//			paConfig = ( paConfig & RF_PACONFIG_MAX_POWER_MASK & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( 7 << 4 ) | ( power );
//		}
//		else
//		{
//			if(power < -4)
//			{
//				power = -4;
//			}
//			paConfig = ( paConfig & RF_PACONFIG_MAX_POWER_MASK & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( 0 << 4 ) | ( power + 4 );
//		}
//	}
//	ret = Sx127x_WriteReg(sx127x,SX127X_REG_09_PA_CONFIG,paConfig);
//	if(ret == STD_NOT_OK)
//	{
//		return ret;
//	}
//	
//	ret = Sx127x_WriteReg(sx127x,SX127X_REG_4D_PA_DAC,paDac);
//	if(ret == STD_NOT_OK)
//	{
//		return ret;
//	}
//}

static uint8_t Sx127x_GetPaSelect(int8_t power)
{
    if( power > 14 )
    {
        return RF_PACONFIG_PASELECT_PABOOST;
    }
    else
    {
        return RF_PACONFIG_PASELECT_RFO;
    }
}


Std_ReturnType Sx127x_ClearFlagsReg(Sx127x_t *sx127x)
{
	Std_ReturnType ret = STD_NOT_OK;
	
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_12_IRQ_FLAGS, 0xff);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetNetworkId(Sx127x_t *sx127x, uint8_t network_id)
{
	Std_ReturnType ret = STD_NOT_OK;
	
	ret = Sx127x_WriteReg(sx127x,SX127X_REG_39_SYNCWORD,network_id);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}

Std_ReturnType Sx127x_IsChannelFree(Sx127x_t *sx127x)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t reg_value = 0;
	
	/* Set channel activity detection mode*/
	ret = Sx127x_SetModeCAD(sx127x);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	ret = Sx127x_ReadReg(sx127x, SX127X_REG_12_IRQ_FLAGS, &reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	while(!(reg_value & SX127X_CAD_DONE))
	{
		//! Should add time out
		
		ret = Sx127x_ReadReg(sx127x, SX127X_REG_12_IRQ_FLAGS, &reg_value);
		if (ret == STD_NOT_OK)
		{
			return ret;
		}	
	}
	
	if ((reg_value & SX127X_CAD_DETECTED))
	{
		sx127x->settings.CadStt = CAD_BUSY;
	}
	else
	{
		sx127x->settings.CadStt = CAD_FREE;
	}
	
	/* Clear CAD_DONE and CAD_DETECTED */ 
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_12_IRQ_FLAGS, (SX127X_CAD_DONE|SX127X_CAD_DETECTED));
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Set standby mode */
	ret = Sx127x_SetModeStdby(sx127x);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}
	
Std_ReturnType Sx127x_Wait4FreeChannel(Sx127x_t *sx127x, uint16_t backoff_time, uint16_t timeout)
{
	Std_ReturnType ret = STD_NOT_OK; 
	unsigned long prev = 0;

	/* Clear full irq Flags */
	ret = Sx127x_ClearFlagsReg(sx127x);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	prev = HAL_GetTick();
	
	/* Check if channel is free */
	ret = Sx127x_IsChannelFree(sx127x);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	while (sx127x->settings.CadStt != CAD_FREE)
	{
		if (HAL_GetTick() < prev)
		{
			prev = HAL_GetTick();
		}
		
		/* Return if timeout */
		if ((HAL_GetTick() - prev) > timeout)
		{
			return STD_NOT_OK;
		}
		
		/* Back off for serveral ms */
		uint16_t tBackOff = (rand()%10) *backoff_time;
		DelayMs (tBackOff);
		
		ret = Sx127x_IsChannelFree(sx127x);
		if (ret == STD_NOT_OK)
		{
			return ret;
		}
	}
	
	return STD_OK;
}

Std_ReturnType Sx127x_SendData(Sx127x_t *sx127x, const uint8_t* data, uint16_t len, uint16_t timeout)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t buff_tx[257]={0};
	uint8_t buff_rx[257]={0};
	uint8_t reg_value = 0;
	unsigned long prev = 0;
	
	if(len > SX127X_MAX_PAYLOAD_LEN)
	{
		len = SX127X_MAX_PAYLOAD_LEN;
	}
	
	ret = Sx127x_SetModeStdby(sx127x);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Set payload length */
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_22_PAYLOAD_LENGTH, len);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Set full FIFO used for TX */
	ret = Sx127x_WriteReg (sx127x, SX127X_REG_0E_FIFO_TX_BASE_ADDR, 0x00);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Set FIFO pointer address 	*/
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_0D_FIFO_ADDR_PTR, 0x00);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Push data to Payload */
	buff_tx[0] = SX127X_REG_00_FIFO | 0x80;
	for(uint8_t i = 0; i < len; i++)
	{
		buff_tx[i+1] = data[i];
	}
	
	ret = Sx127x_Interface(sx127x, buff_tx, buff_rx, (len+1));
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Set mode Tx to transmit */
	ret = Sx127x_SetModeTx(sx127x);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	prev = HAL_GetTick();
	
	ret = Sx127x_ReadReg(sx127x, SX127X_REG_12_IRQ_FLAGS, &reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	while((reg_value & SX127X_TX_DONE) != SX127X_TX_DONE)
	{
		
		if (prev > HAL_GetTick())
		{
			prev = HAL_GetTick();
		}
		
		/* Return if timeout */
		if ((HAL_GetTick() - prev) > timeout)
		{
			return STD_NOT_OK;
		}
		
		ret = Sx127x_ReadReg(sx127x, SX127X_REG_12_IRQ_FLAGS, &reg_value);
	  if (ret == STD_NOT_OK)
	  {
			return ret;
		}
	}
	
	/* Clear IRQ Flags */
	ret = Sx127x_WriteReg(sx127x,SX127X_REG_12_IRQ_FLAGS,SX127X_TX_DONE);
	if(ret == STD_NOT_OK)
	{
			return ret;
	}
	
	ret = Sx127x_SetModeStdby(sx127x);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}

Std_ReturnType Sx127x_ReceiveData(Sx127x_t *sx127x, uint8_t *rx_buffer, uint8_t *len_rx, uint16_t timeout)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t buff_tx[257]={0};
	uint8_t buff_rx[257]={0};
	uint8_t reg_value = 0;
	unsigned long prev = 0;
	uint8_t len = 0;
	
	if (sx127x->settings.State == RF_TX_RUNNING)
	{
		return STD_NOT_OK;
	}
	
	if (sx127x->settings.State != RF_STDBY)
	{
		ret = Sx127x_SetModeStdby(sx127x);
		if(ret == STD_NOT_OK)
	  {
		  return ret;
	  }
	}
	
	/* Clear all flags*/
	ret = Sx127x_ClearFlagsReg(sx127x);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Set full FIFO used for RX */
	ret = Sx127x_WriteReg (sx127x, SX127X_REG_0F_FIFO_RX_BASE_ADDR, 0x00);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Set mode Rx */
	ret = Sx127x_SetModeRx(sx127x);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	prev = HAL_GetTick();
	
	ret = Sx127x_ReadReg(sx127x, SX127X_REG_12_IRQ_FLAGS, &reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	while((reg_value & SX127X_RX_DONE) != SX127X_RX_DONE)
	{
		if(prev > HAL_GetTick())
		{
			prev = HAL_GetTick();
		}
		
		/* Return if timeout */
		if ((HAL_GetTick() - prev) > timeout)
		{
			return STD_NOT_OK;
		}
		
		ret = Sx127x_ReadReg(sx127x, SX127X_REG_12_IRQ_FLAGS, &reg_value);
	  if (ret == STD_NOT_OK)
	  {
		  return ret;
	  }
	}
	
	ret = Sx127x_ReadReg(sx127x, SX127X_REG_13_RX_NB_BYTES, &len);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	*len_rx = len;
	
	ret = Sx127x_ReadReg(sx127x, SX127X_REG_10_FIFO_RX_CURRENT_ADDR, &reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_0D_FIFO_ADDR_PTR, reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Read FIFO rx data */
	buff_tx[0] = SX127X_REG_00_FIFO & 0x7F;
	ret = Sx127x_Interface(sx127x, buff_tx, buff_rx, (uint16_t) (len+1));
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	for(uint8_t i = 0; i < len; i++)
	{
		rx_buffer[i] = buff_rx[i+1];
	}
	
	ret = Sx127x_ClearFlagsReg(sx127x);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	ret = Sx127x_SetModeStdby(sx127x);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}

Std_ReturnType Sx127x_SetLoRa(Sx127x_t *sx127x)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t reg_value = 0;
	
	ret = Sx127x_SetModeSleep(sx127x);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	ret = Sx127x_ReadReg(sx127x, SX127X_REG_01_OP_MODE, &reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	reg_value |= SX127X_LONG_RANGE_MODE;
	
	ret = Sx127x_WriteReg(sx127x, SX127X_REG_01_OP_MODE, reg_value);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	ret = Sx127x_SetModeStdby(sx127x);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}

Std_ReturnType Sx127x_InitModule(Sx127x_t *sx127x)
{
	Std_ReturnType ret = STD_NOT_OK;
	
	/* Reset Module */
	Sx127x_Reset(sx127x);
	
	/* Init LoRa mode */
	ret = Sx127x_SetLoRa(sx127x);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Init Modem configuration */
	ret = Sx127x_SetModemConfig(sx127x, sx127x->settings.CfgChoice);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Init Preamble length*/
	ret = Sx127x_SetPreambleLength(sx127x, sx127x->settings.PreambleLen);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Init center frequency */
	ret = Sx127x_SetFrequency(sx127x, sx127x->settings.CenterFreq);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Init Tx Power */
	ret = Sx127x_SetTxPower(sx127x, sx127x->settings.TxPower,STD_OFF);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	/* Init Network ID */
	ret = Sx127x_SetNetworkId(sx127x, sx127x->settings.NetworkId);
	if (ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}


Std_ReturnType Sx1276_RandomSeed(Sx127x_t *sx127x, uint32_t *rnd_seed)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t i = 0;
	uint8_t rssi_arr[4];
	uint8_t reg_value = 0;
	
	ret = Sx127x_SetModeRx(sx127x);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	for(i = 0; i < 4; i++)
	{
		DelayMs(1);
		
		// Unfiltered RSSI value reading. Only takes the LSB value
		ret = Sx127x_ReadReg(sx127x, SX127X_REG_2C_RSSIWIDEBAND, &reg_value); 
		if(ret == STD_NOT_OK)
		{
			return ret;
		}
		
		rssi_arr[i] = reg_value;
	}
	
  *rnd_seed = ((rssi_arr[0]<<24) | (rssi_arr[1]<<16) | (rssi_arr[2]<<8) | rssi_arr[3]);
	
	ret = Sx127x_SetModeStdby(sx127x);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	return STD_OK;
}

/*
Std_ReturnType Sx127x_ReadRssi(Sx127x_t *sx127x, int16_t * rssi_value)
{
	Std_ReturnType ret = STD_NOT_OK;
	uint8_t reg_value = 0;
	uint32_t frf = (sx127x->settings.CenterFreq * 1000000.0) / SX127X_FSTEP;
	
	ret = Sx127x_ReadReg(sx127x, SX127X_REG_1B_RSSI_VALUE, &reg_value);
	if(ret == STD_NOT_OK)
	{
		return ret;
	}
	
	if(frf > RF_MID_BAND_THRESH)
	{
		*rssi_value = RSSI_OFFSET_HF + reg_value;
	}
	else
	{
		*rssi_value = RSSI_OFFSET_LF + reg_value;
	}
	
	return STD_OK;
}
*/
