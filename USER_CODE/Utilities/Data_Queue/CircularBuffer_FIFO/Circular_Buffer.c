#include <string.h>
#include "Circular_Buffer.h"


Std_ReturnType Circular_Buffer_Push(CircularBuffer_t *cirbuf, uint8_t * data)
{
	if(cirbuf->buffer_length == cirbuf->maxlen)
	{
		cirbuf->buffer_status = BUFFER_IS_FULL;
		
		return STD_NOT_OK;
	}
	
	memcpy(cirbuf->buffer[cirbuf->write_index],data,sizeof((unsigned char *)data));
	
	cirbuf->write_index++;
	
	if(cirbuf->write_index == cirbuf->maxlen)
	{
		cirbuf->write_index = 0;
	}
	
	cirbuf->buffer_length++;
	
	cirbuf->buffer_status = IDLE;
	
	return STD_OK;
}

Std_ReturnType Circular_Buffer_Pop(CircularBuffer_t *cirbuf, uint8_t * data)
{
	if(cirbuf->buffer_length == 0)
	{
		cirbuf->buffer_status = BUFFER_IS_EMPTY;
		return STD_NOT_OK;
	}
	
	memcpy(data,cirbuf->buffer[cirbuf->read_index],sizeof(cirbuf->buffer[cirbuf->read_index]));
	
	memset(cirbuf->buffer[cirbuf->read_index],0,sizeof(cirbuf->buffer[cirbuf->read_index]));
	
	cirbuf->read_index++;
	
	if(cirbuf->read_index == cirbuf->maxlen)
	{
		cirbuf->read_index = 0;
	}
	
	cirbuf->buffer_length--;
	
	cirbuf->buffer_status = IDLE;
	
	return STD_OK;
}
