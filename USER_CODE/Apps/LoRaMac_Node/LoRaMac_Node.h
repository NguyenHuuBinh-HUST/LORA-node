#ifndef LORA_MAC_NODE_H
#define LORA_MAC_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"
#include "LoRaMac_Node_Types.h"
#include "./cfg/LoRaMac_Node_Cfg.h"

LoRaMacParserStatus_t LoRaMac_ParserData(LoRaMacMessageData_t* mac_msg);
	
LoRaMacSerializerStatus_t LoRaMac_SetUpDataMessage
(
  LoRaMacMessageData_t *data_msg,
  Std_SwitchType ack_switch,
  uint8_t frame_port,
	uint16_t *cur_frame_cnt,
  uint8_t *data,
  uint16_t frame_length
);

//LoRaMacSerializerStatus_t LoRaMacSetJoinRequest(LoRaMacMessageData_t* macMsg);

LoRaMacSerializerStatus_t LoRaMac_SetUpACKMessage(LoRaMacMessageData_t *ack_msg, uint16_t cnt_reply);

#ifdef __cplusplus
}
#endif

#endif /* LORA_MAC_NODE_H */
