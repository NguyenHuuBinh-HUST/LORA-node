#include<string.h>
#include "LinkedList_FIFO.h"

void InitFIFO(Node_t **p2head, Node_t **p2tail)
{
	*p2head = NULL;
	*p2tail = NULL;
}

Node_t * CreateNode(uint8_t * data)
{
    Node_t * temp;
    temp = (Node_t *)malloc(sizeof(Node_t));
    
    temp->next = NULL;
    
    memcpy(temp->data,data,strlen((char *)data));
    
    return temp;
}

void PushHead(Node_t **p2head, uint8_t * data)
{
	Node_t *temp = CreateNode(data);
	
	if(*p2head == NULL)
	{
		*p2head = temp;
	}
	else
	{
		temp->next = *p2head;
		*p2head = temp;
	}
}

Node_t * PopTail(Node_t *head, uint8_t * data)
{
	if(head == NULL)
	return NULL;
	
	if (head->next == NULL) 
	{
		free(head);
		return NULL;
	} 
	
	Node_t *p = head;
	
	while(p->next->next != NULL)
	{
	   p = p->next;   
	}
	
	memcpy(data,p->next->data,sizeof(p->next->data));
	
	free(p->next);
	
	p->next = NULL;
	
	return head;
}

uint8_t IsEmpty(Node_t *head)
{
	return (head == NULL);
}
