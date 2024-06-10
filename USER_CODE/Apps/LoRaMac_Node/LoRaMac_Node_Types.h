#ifndef LORA_MAC_NODE_TYPES_H
#define LORA_MAC_NODE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sx127x.h" 
#include "./cfg/LoRaMac_Node_Cfg.h"

#define FRM_PAYLOAD_MAX_LEN            245
#define MAX_BUFFER_LEN                 256
	
/*! Frame header (FHDR) maximum field size */
#define LORAMAC_FHDR_MAX_FIELD_SIZE             9

/*! FHDR Device address field size */
#define LORAMAC_FHDR_DEV_ADD_FIELD_SIZE         4

/*! FHDR Frame control field size */
#define LORAMAC_FHDR_F_CTRL_FIELD_SIZE          1

/*! FHDR Frame control field size */
#define LORAMAC_FHDR_F_CNT_FIELD_SIZE           2
	
/*! FHDR Frame length field size */
#define LORAMAC_FHDR_F_LEN_FIELD_SIZE           2


/*! MAC header field size */
#define LORAMAC_MHDR_FIELD_SIZE             1

/*! Port field size */
#define LORAMAC_F_PORT_FIELD_SIZE           1

/*!
 * LoRaMac Version
 */
typedef enum eLoRaMacVersion
{
	/*!
     * Version 1.0
     */
	LORAMAC_VERSION_1_SMART_HOME = 0,
	/*!
     * Version 2.0
     */
	LORAMAC_VERSION_2_DATA_COLLECTING,
	/*!
     * Version 3.0
     */
	LORAMAC_VERSION_3_RFU,
	/*!
     * Version 4.0
     */
	LORAMAC_VERSION_4_RFU,
	
}LoRaMacVersion_t;

/*!
 * LoRaMac Serializer Status
 */
typedef enum eLoRaMacSerializerStatus
{
    /*!
     * No error occurred
     */
    LORAMAC_SERIALIZER_SUCCESS = 0,
    /*!
     * Null pointer exception
     */
    LORAMAC_SERIALIZER_ERROR_NPE,
    /*!
     * Incompatible buffer size
     */
    LORAMAC_SERIALIZER_ERROR_BUF_SIZE,
    /*!
     * Undefined Error occurred
     */
    LORAMAC_SERIALIZER_ERROR,
	
}LoRaMacSerializerStatus_t;

/*!
 * LoRaMac Parser Status
 */
typedef enum eLoRaMacParserStatus
{
    /*!
     * No error occurred
     */
    LORAMAC_PARSER_SUCCESS = 0,
    /*!
     * Failure during parsing occurred
     */
    LORAMAC_PARSER_FAIL,
    /*!
     * Null pointer exception
     */
    LORAMAC_PARSER_ERROR_NPE,
    /*!
     * Undefined Error occurred
     */
    LORAMAC_PARSER_ERROR,
	
}LoRaMacParserStatus_t;

/*!
 * LoRaMac message type enumerator
 */
typedef enum eLoRaMacMessageType
{
    /*!
     * Join-request message
     */
    LORAMAC_MSG_TYPE_JOIN_REQUEST,
    /*!
     * Join-accept message
     */
    LORAMAC_MSG_TYPE_JOIN_ACCEPT,
    /*!
     * Data MAC messages
     */
    LORAMAC_MSG_TYPE_DATA,
    /*!
     * Undefined message type
     */
    LORAMAC_MSG_TYPE_UNDEF,
	
}LoRaMacMessageType_t;

/* MAC Header struct */
typedef union uLoRaMacHeader
{
    /*!
     * Byte-access to the bits
     */
    uint8_t Value;
    /*!
     * Structure containing single access to header bits
     */
    struct sMacHeaderBits
    {
        /*!
         * Major version
         */
        uint8_t Major           : 2;
        /*!
         * RFU
         */
        uint8_t RFU             : 3;
        /*!
         * Message type
         */
        uint8_t MType           : 3;
    }Bits;
	
}LoRaMacHeader_t;

typedef union uLoRaMacFrameCtrl
{
    /*!
     * Byte-access to the bits
     */
    uint8_t Value;
    /*!
     * Structure containing single access to bits
     */
    struct sCtrlBits
    {
        /*!
         * RFU
         */
        uint8_t RFU             : 5;
		/*!
         * Message urgent bit
         */
        uint8_t FUrgent         : 1;
        /*!
         * Frame pending bit
         */
        uint8_t FPending        : 1;
        /*!
         * Message acknowledge bit
         */
        uint8_t Ack             : 1;
    }Bits;
	
}LoRaMacFrameCtrl_t;

typedef struct sLoRaMacFrameHeader
{
    /*!
     * Device address
     */
    uint32_t DevAddr;
    /*!
     * Frame control field
     */
    LoRaMacFrameCtrl_t FCtrl;
    /*!
     * Frame counter
     */
    uint16_t FCnt;
    /*!
     * Frame length
     */
    uint16_t FLen;
	
}LoRaMacFrameHeader_t;

/*!
 * LoRaMac type for Join-request message
 */
typedef struct sLoRaMacMessageJoinRequest
{
    /*!
     * Serialized message buffer
     */
    uint8_t* Buffer;
    /*!
     * Size of serialized message buffer
     */
    uint8_t BufSize;
    /*!
     * MAC header
     */
    LoRaMacHeader_t MHDR;
	
}LoRaMacMessageJoinRequest_t;

/*!
 * LoRaMac type for Join-accept message
 */
typedef struct sLoRaMacMessageJoinAccept
{
    /*!
     * Serialized message buffer
     */
    uint8_t* Buffer;
    /*!
     * Size of serialized message buffer
     */
    uint8_t BufSize;
    /*!
     * MAC header
     */
    LoRaMacHeader_t MHDR;
	
}LoRaMacMessageJoinAccept_t;

/*!
 * LoRaMac type for Data MAC messages
 * (Unconfirmed Data, Confirmed Data, Unconfirmed Command, Confirmed Command)
 */
typedef struct sLoRaMacMessageData
{
    /*!
     * Serialized message buffer
     */
    uint8_t Buffer[MAX_BUFFER_LEN];
    /*!
     * Size of serialized message buffer
     */
    uint8_t BufSize;
    /*!
     * MAC header
     */
    LoRaMacHeader_t MHDR;
    /*!
     * Frame header (FHDR)
     */
    LoRaMacFrameHeader_t FHDR;
    /*!
     * Port field (opt.)
     */
    uint8_t FPort;
    /*!
     * Frame payload may contain MAC commands or data (opt.)
     */
    uint8_t FRMPayload[FRM_PAYLOAD_MAX_LEN];
    /*!
     * Size of frame payload (not included in LoRaMac messages) 
     */
    uint8_t FRMPayloadSize;
	
}LoRaMacMessageData_t;

/*!
 * LoRaMac general message type
 */
typedef struct sLoRaMacMessage
{
    LoRaMacMessageType_t Type;
    union uMessage
    {
        LoRaMacMessageJoinRequest_t JoinReq;
        LoRaMacMessageJoinAccept_t JoinAccept;
        LoRaMacMessageData_t Data;
    }Message;
	
}LoRaMacMessage_t;

typedef enum eLoRaMacFrameType
{
    /*!
     * LoRaMAC join request frame
     */
    FRAME_TYPE_JOIN_REQ              = 0x00,
    /*!
     * LoRaMAC join accept frame
     */
    FRAME_TYPE_JOIN_ACCEPT           = 0x01,
    /*!
     * LoRaMAC unconfirmed data frame
     */
    FRAME_TYPE_DATA_UNCONFIRMED      = 0x02,
	  /*!
     * LoRaMAC confirmed data frame
     */
    FRAME_TYPE_DATA_CONFIRMED        = 0x03,
	  /*!
     * LoRaMAC unconfirmed command frame
     */
    FRAME_TYPE_COMMAND_UNCONFIRMED   = 0x04,
	  /*!
     * LoRaMAC confirmed command frame
     */
    FRAME_TYPE_COMMAND_CONFIRMED     = 0x05,
	
}LoRaMacFrameType_t;


#ifdef __cplusplus
}
#endif

#endif /* LORA_MAC_NODE_TYPES_H */
