#include "Circular_Buffer_Cfg.h"

/* Circular buffer for rx lora */
CircularBuffer_t rx_lora_cirbuf =
{
	IDLE, \
  0, \
  0, \
  0, \
  MAX_CB_LENGTH, \
  {0},
};

//CircularBuffer_t Cell_backup =
//{
//	IDLE, \
//  0, \
//  0, \
//  0, \
//  MAX_CB_LENGTH, \
//  {0},
//};