#include "./cfg/LoRaMac_Node_Cfg.h"

LoRaMacMessageData_t tx_msg = {
	{NULL}, \
	NULL, \
	((NULL<<5)|(LORAMAC_VERSION)), \
	{
		DEV_ADDR, \
		NULL, \
		NULL, \
		NULL, \
	}, \
	NULL, \
	{NULL}, \
	NULL, \
};
