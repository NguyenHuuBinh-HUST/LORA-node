#ifndef CIRCULAR_BUFFER_TYPES_H
#define CIRCULAR_BUFFER_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "Std_Types.h"
#include "./cfg/Circular_Buffer_Cfg.h"

#define MAX_CB_LENGTH 3

#define MAX_CB_DATA_LENGTH   256
	
typedef enum
{
	IDLE = 0x00, \
	BUFFER_IS_FULL, \
	BUFFER_IS_EMPTY, \
}CircularBuffer_Stt;

typedef struct 
{
	CircularBuffer_Stt buffer_status;
	uint8_t            write_index;
	uint8_t            read_index;
	uint8_t            buffer_length;
	const uint8_t      maxlen;
	uint8_t            buffer[MAX_CB_LENGTH][MAX_CB_DATA_LENGTH];
}CircularBuffer_t;



#ifdef __cplusplus
}
#endif

#endif /* CIRCULAR_BUFFER_TYPES_H */
