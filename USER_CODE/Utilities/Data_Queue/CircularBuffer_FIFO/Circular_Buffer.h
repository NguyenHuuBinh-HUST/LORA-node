#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Circular_Buffer_Types.h"

Std_ReturnType Circular_Buffer_Push(CircularBuffer_t *cirbuf, uint8_t * data);

Std_ReturnType Circular_Buffer_Pop(CircularBuffer_t *cirbuf, uint8_t *data);


#ifdef __cplusplus
}
#endif

#endif /* CIRCULAR_BUFFER_H */
