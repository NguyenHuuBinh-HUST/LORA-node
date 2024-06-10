#ifndef LINKED_LIST_FIFO_H
#define LINKED_LIST_FIFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "LinkedList_FIFO_Types.h"

void InitFIFO(Node_t **p2head, Node_t **p2tail);

Node_t *CreateNode(uint8_t *buffer);

void PushHead(Node_t **p2head, uint8_t * data);

Node_t * PopTail(Node_t *head, uint8_t * data);

uint8_t IsEmpty(Node_t *head);
	

#ifdef __cplusplus
}
#endif

#endif /* LINKED_LIST_FIFO_H */
