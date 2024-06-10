#ifndef SENSOR_SERIAL_H
#define SENSOR_SERIAL_H

#include <stdint.h>
#include "serial_pipe_hal.h"

#define MAXSIZE_SENSOR_DIGITAL_PIPE	256
//#define DEBUG_SENSOR_DIGITAL

class SensorDigital : public SerialPipe
{
public:
	/** Constructor
        \param rxSize the size of the serial rx buffer
    */
	SensorDigital( int rxSize = MAXSIZE_SENSOR_DIGITAL_PIPE);
	//! Destructor
	virtual ~SensorDigital(void);

	virtual void start(void);

	virtual void purge(void);

	virtual int waitFinalResp(uint32_t timeout_ms = 1000 /* ms */);
	virtual int getLine(char* buffer, int length);
	virtual int valueSalt(void);
	virtual int valuePH(void);
	virtual int valueOxy(void);
	virtual int valueTemp(void);
	virtual int valueNH3(void);
	virtual int valueH2S(void);
	
protected:
	volatile int _Salt;
	volatile int _PH;
	volatile int _Oxy;
	volatile int _Temp;
	volatile int _NH3;
	volatile int _H2S;
	
	int _getLine(Pipe<char>* pipe, char* buf, int len);
	int _parseFormated(Pipe<char>* pipe, int len, const char* fmt);
	int _parseMatch(Pipe<char>* pipe, int len, const char* sta, const char* end);
	void _dumpUART(const char* buf, int len);
};

extern SensorDigital sensorDigital;

#endif
