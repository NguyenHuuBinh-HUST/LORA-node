#include<string.h>
#include "LoRaMac_Node.h"

LoRaMacParserStatus_t LoRaMac_ParserData(LoRaMacMessageData_t* mac_msg)
{
	uint16_t bufItr = 0;
	
	mac_msg->MHDR.Value = mac_msg->Buffer[bufItr++];
	mac_msg->FHDR.DevAddr = mac_msg->Buffer[bufItr++];
	mac_msg->FHDR.DevAddr |= ((uint32_t) mac_msg->Buffer[bufItr++] << 8);
	mac_msg->FHDR.DevAddr |= ((uint32_t) mac_msg->Buffer[bufItr++] << 16);
	mac_msg->FHDR.DevAddr |= ((uint32_t) mac_msg->Buffer[bufItr++] << 24);
	
	mac_msg->FHDR.FCtrl.Value = mac_msg->Buffer[bufItr++];
	
	mac_msg->FHDR.FCnt = mac_msg->Buffer[bufItr++];
	mac_msg->FHDR.FCnt |= mac_msg->Buffer[bufItr++] << 8;
	
	mac_msg->FHDR.FLen = mac_msg->Buffer[bufItr++];
	
	mac_msg->FPort = 0;
	mac_msg->FRMPayloadSize = 0;
	
	if((mac_msg->BufSize - bufItr) > 0)
	{
		mac_msg->FPort = mac_msg->Buffer[bufItr++];
		mac_msg->FRMPayloadSize = (mac_msg->BufSize - bufItr);
    if(mac_msg->FRMPayloadSize != mac_msg->FHDR.FLen)
		{
			return LORAMAC_PARSER_FAIL;
		}
		
		for(uint8_t i = 0; i < mac_msg->FRMPayloadSize; i++)
		{
			mac_msg->FRMPayload[i] = mac_msg->Buffer[bufItr++];
		}
	}
	
	return LORAMAC_PARSER_SUCCESS;
}

LoRaMacSerializerStatus_t LoRaMac_SerializePacket(LoRaMacMessageData_t* mac_msg)
{
	
	uint16_t bufItr = 0;
	
	/* Calculate buffer size*/
	uint16_t computedBufSize = LORAMAC_MHDR_FIELD_SIZE
                           + LORAMAC_FHDR_DEV_ADD_FIELD_SIZE
                           + LORAMAC_FHDR_F_CTRL_FIELD_SIZE
                           + LORAMAC_FHDR_F_CNT_FIELD_SIZE
	                         + LORAMAC_FHDR_F_LEN_FIELD_SIZE;
	
	if(mac_msg->FRMPayloadSize > 0)
	{
		computedBufSize += LORAMAC_F_PORT_FIELD_SIZE;
	}
	
	computedBufSize += mac_msg->FRMPayloadSize;
	
	/* Check macMsg->BufSize */
	if(mac_msg->BufSize < computedBufSize)
	{
		return LORAMAC_SERIALIZER_ERROR_BUF_SIZE;
	}
	
	mac_msg->Buffer[bufItr++] = mac_msg->MHDR.Value;
	mac_msg->Buffer[bufItr++] = (mac_msg->FHDR.DevAddr) & 0xFF;
	mac_msg->Buffer[bufItr++] = (mac_msg->FHDR.DevAddr >> 8) & 0xFF;
	mac_msg->Buffer[bufItr++] = (mac_msg->FHDR.DevAddr >> 16) & 0xFF;
	mac_msg->Buffer[bufItr++] = (mac_msg->FHDR.DevAddr >> 24) & 0xFF;
	
	mac_msg->Buffer[bufItr++] = mac_msg->FHDR.FCtrl.Value;
	mac_msg->Buffer[bufItr++] = mac_msg->FHDR.FCnt & 0xFF;
	mac_msg->Buffer[bufItr++] = (mac_msg->FHDR.FCnt >> 8) & 0xFF;
	mac_msg->Buffer[bufItr++] = mac_msg->FHDR.FLen;
	
	if(mac_msg->FRMPayloadSize > 0)
	{
		mac_msg->Buffer[bufItr++] = mac_msg->FPort;
	}
	else
	{
		return LORAMAC_SERIALIZER_SUCCESS;
	}
	
	for(uint8_t i = 0; i < mac_msg->FRMPayloadSize; i++)
	{
		mac_msg->Buffer[bufItr++] = mac_msg->FRMPayload[i];
	}

	mac_msg->BufSize = bufItr;
	
	return LORAMAC_SERIALIZER_SUCCESS;
}

LoRaMacSerializerStatus_t LoRaMac_SetUpDataMessage
(
  LoRaMacMessageData_t *data_msg,
  Std_SwitchType ack_switch,
  uint8_t frame_port,
  uint16_t *cur_frame_cnt,
  uint8_t *data,
  uint16_t frame_length
)
{
	LoRaMacSerializerStatus_t serialize_ret = LORAMAC_SERIALIZER_ERROR;
	
	if(ack_switch == STD_ON)
	{
		data_msg->MHDR.Bits.MType = FRAME_TYPE_DATA_CONFIRMED;
	}
	else
	{
		data_msg->MHDR.Bits.MType = FRAME_TYPE_DATA_UNCONFIRMED;
	}
	
	data_msg->BufSize = 255;
	
  data_msg->FHDR.DevAddr = DEV_ADDR;
	
//	*cur_frame_cnt = data_msg->FHDR.FCnt;
//	
//	data_msg->FHDR.FCnt++;
	
	data_msg->FHDR.FCnt = *cur_frame_cnt;
	
	data_msg->FHDR.FLen = frame_length;
	
	data_msg->FPort = frame_port;
	
	memcpy(data_msg->FRMPayload,data,frame_length);
	
	data_msg->FRMPayloadSize = frame_length;
	
	serialize_ret = LoRaMac_SerializePacket(data_msg);
	
	return serialize_ret;
}

LoRaMacSerializerStatus_t LoRaMac_SetUpACKMessage
(
  LoRaMacMessageData_t *ack_msg,
  uint16_t cnt_reply
)
{

	LoRaMacSerializerStatus_t serialize_ret = LORAMAC_SERIALIZER_ERROR;

	ack_msg->MHDR.Bits.MType = FRAME_TYPE_DATA_UNCONFIRMED;
	
	ack_msg->BufSize = 255;
	
  ack_msg->FHDR.DevAddr = DEV_ADDR;
	
	ack_msg->FHDR.FCtrl.Bits.Ack = 1;
	
	ack_msg->FHDR.FCnt = cnt_reply;
	
	ack_msg->FHDR.FLen = 0;
	
	ack_msg->FPort = 0;
	
	ack_msg->FRMPayloadSize = 0;
	
	serialize_ret = LoRaMac_SerializePacket(ack_msg);
	
	return serialize_ret;
}