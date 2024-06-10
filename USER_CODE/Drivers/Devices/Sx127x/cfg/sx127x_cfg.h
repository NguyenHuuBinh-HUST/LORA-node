
#ifndef SX127X_CFG_H
#define SX127X_CFG_H

#include "cmsis_os.h"
#include "sx127x_Types.h"

extern Sx127x_CfgType sx127x_cmd;

#define USING_RTOS

/* Define Hardware  */
#define SX127X_RESET_PORT                 GPIOB

#define SX127X_RESET_PIN                  GPIO_PIN_9

#define SX127X_NSS_PORT                   GPIOB

#define SX127X_NSS_PIN                    GPIO_PIN_12

#define SX127X_NODE_HSPI                  hspi2       

/* Define Parameters */
#define SX127X_UPLINK_FREQUENCY           (float)868.1

#define SX127X_DOWNLINK_FREQUENCY         (float)869.525

#define SX127X_PREAMBLE_LENGTH            8

#define SX127X_TX_POWER                   17

#define SX127X_NETWORK_ID_DEFAULT         0x34

#define SX127X_MAX_PAYLOAD_LEN            256

/* Define Timeout */
#define SX127X_RX_TIMEOUT_MS              10000  

#define SX127X_TX_TIMEOUT_MS              10000  

#define SX127X_CAD_TIMEOUT_MS             10000  

#define SX127X_BACKOFF_TIMEOUT_MS         10  

#endif /* SX127X_CFG_H */
