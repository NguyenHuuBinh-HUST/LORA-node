#ifndef LORA_MAC_NODE_CFG_H
#define LORA_MAC_NODE_CFG_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "LoRaMac_Node_Types.h"

/* Define LoRa MAC version */
#define LORAMAC_VERSION                LORAMAC_VERSION_2_DATA_COLLECTING
	
/* Define network address */
#define NETWORK_ADDR                   ((255<<24) | (255<<16) | (0<<8) | 0)

/* Define device address */
#define DEV_ADDR                       ((NETWORK_ADDR) | 9)

/* Define network broadcast address */
#define BROADCAST_ADDR                 ((NETWORK_ADDR) | 255)
	
/* Define ACK Timeout */
#define ACK_TIMEOUT                    2000

#ifdef __cplusplus
}
#endif

#endif /* LORA_MAC_NODE_CFG_H */


