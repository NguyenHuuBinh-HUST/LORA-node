#include "spi.h"
#include "./cfg/sx127x_cfg.h"

extern SPI_HandleTypeDef SX127X_NODE_HSPI;

Sx127x_CfgType sx127x_cmd ={
	
	(void *) HAL_SPI_TransmitReceive,
	
};

Sx127x_t sx127x_node ={
	&SX127X_NODE_HSPI, \
  {SX127X_RESET_PORT,SX127X_RESET_PIN}, \
	{SX127X_NSS_PORT,SX127X_NSS_PIN}, \
	{
		LoRa, \
		RF_STDBY, \
		Bw125Cr48Sf1024,\
		CAD_BUSY, \
		SX127X_UPLINK_FREQUENCY, \
		SX127X_PREAMBLE_LENGTH, \
		SX127X_TX_POWER, \
		SX127X_NETWORK_ID_DEFAULT
	}, \
};


