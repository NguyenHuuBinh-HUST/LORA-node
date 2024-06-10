#ifndef LINKED_LIST_FIFO_TYPES_H
#define LINKED_LIST_FIFO_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h"
#include "stm32f1xx.h"
#include "./cfg/LinkedList_FIFO_Cfg.h"

typedef struct Node_s
{
	uint8_t data[MAX_DATA_BUFFER];
	
	struct Node_s *next;
	struct Node_s *prev;
}Node_t;


#ifdef __cplusplus
}
#endif

#endif /* LINKED_LIST_FIFO_TYPES_H */
