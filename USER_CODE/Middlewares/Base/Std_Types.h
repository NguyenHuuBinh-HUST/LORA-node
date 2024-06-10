#ifndef STD_TYPES_H
#define STD_TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

#include "cmsis_os.h"

#define STD_OK			  0x00
#define STD_NOT_OK		0x01

#define STD_OFF       0x00
#define STD_ON        0x01

typedef uint8_t Std_ReturnType;
	
typedef uint8_t Std_SwitchType;

#define BIT_CLEAR8(value, mask)     ((value) &= (~(mask)))
#define BIT_CLEAR16(value, mask)    ((value) &= (~(mask)))
#define BIT_CLEAR32(value, mask)    ((value) &= (~(mask)))


#define BIT_GET8(value, mask)       (((uint8) value) & (mask))
#define BIT_GET16(value, mask)      (((uint16) value) & (mask))
#define BIT_GET32(value, mask)      (((uint32) value) & (mask))


#define BIT_SET8(value, mask)       ((value) |= (mask))
#define BIT_SET16(value, mask)      ((value) |= (mask))
#define BIT_SET32(value, mask)      ((value) |= (mask))

#define BIT_IS_SET8(value, mask)       ((value & mask) == mask)
#define BIT_IS_SET16(value, mask)      ((value & mask) == mask)
#define BIT_IS_SET32(value, mask)      ((value & mask) == mask)

#ifdef __cplusplus
}
#endif


#endif /* STD_TYPES_H */
