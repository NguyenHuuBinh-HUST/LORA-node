#ifndef SERIAL_PIPE_H
#define SERIAL_PIPE_H

#include "pipe_hal.h"
#include "main.h"
#include "Std_Types.h"

#ifndef EOF
    #define EOF (-1)
#endif

class SerialPipe
{
public:
	SerialPipe(int rxSize = 256);
	~SerialPipe(void);
	// rx channel
    //----------------------------------------------------

    /** check if readable
        \return the size available in the buffer.
    */
    int readable(void);

    /** receive one character from the serial port (blocking)
        \param the character received
    */
    int getc(void);

    /** read a buffer from the serial port
        \param pointer to the buffer to read.
        \param length number of bytes to read
        \param blocking true if all bytes shall be read. false if only the available bytes.
        \return the number of bytes read.
    */
		
    int get(void* buffer, int length, bool blocking);
		
	void rxIRQBuf(char c);

protected:
    Pipe<char> _pipeRx; //!< receive pipe
};

extern char UART1_RX_Data;
extern char UART2_RX_Data;
extern char UART3_RX_Data;

Std_ReturnType UART6_GetData(uint8_t * buffer, uint8_t * len, uint32_t Timeout);

#endif
