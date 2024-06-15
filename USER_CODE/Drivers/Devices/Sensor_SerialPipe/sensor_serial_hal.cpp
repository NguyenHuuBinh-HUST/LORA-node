#include <stdio.h>
#include "common.h"
#include "usart.h"
#include "main.h"
#include "systick_driver.h"

#include "sensor_serial_hal.h"


#define UART4_TIME_SEND_DATA              500

/* RX Data from UART Interrupt */
char UART4_RX_Data;

void HW_UART4_Init(void)
{
	MX_UART4_Init();
	
  //Start UART
  __HAL_UART_FLUSH_DRREGISTER(&huart4);
  //HAL_UART_Receive_IT(&huart4, (uint8_t *)&UART4_RX_Data, 1);
	//Start Receive DMA
	HAL_UART_Receive_DMA(&huart4, (uint8_t *)&UART4_RX_Data, 1);
}

void HW_UART4_DeInit(void)
{
	HAL_UART_DeInit(&huart4);
}

void UART4_Send(const char* buf, int len)
{
	HAL_UART_Transmit(&huart4, (uint8_t *)buf, len, UART4_TIME_SEND_DATA);
}
// ----------------------------------------------------------------
// Parsing
// ----------------------------------------------------------------
enum {
    // waitFinalResp Responses
    SENSOR_OK     =  1,
    NOT_FOUND     =  0,
    WAIT          = -1, // TIMEOUT

    // getLine Responses
    #define LENGTH(x)  (x & 0x00FFFF) //!< extract/mask the length
    #define TYPE(x)    (x & 0xFF0000) //!< extract/mask the type

    TYPE_UNKNOWN    = 0x000000,
    TYPE_RESULT     = 0x110000,
	
    TIMEOUT_BLOCKING = 0xfffffff,
};

//! check for timeout
#define TIMEOUT(t, ms)  ((ms != TIMEOUT_BLOCKING) && ((HAL_Timer_Get_Milli_Seconds() - t) > ms))

SensorDigital::SensorDigital(int rxSize) : SerialPipe (rxSize)
{
    //Do nothing
    _Salt = 0;
    _Oxy = 0;
    _PH = 0;
    _Temp = 0;
	_H2S = 0;
	_NH3 = 0;
}

SensorDigital::~SensorDigital(void)
{
	HW_UART4_DeInit();
}

void SensorDigital::start(void)
{
	HW_UART4_Init();
}

void SensorDigital::purge(void)
{
    while (readable())
            _pipeRx.getc();
}
/**
* waitFinalResp
* Return 1 => got data
* Return 0 => no data
*/
int SensorDigital::waitFinalResp(uint32_t timeout_ms)
{
	char buf[MAXSIZE_SENSOR_DIGITAL_PIPE + 16 /* add some more space for framing */];
    int a1, a2, a3, a4, a5, a6, a7, a8, a9;
	uint32_t start = HAL_Timer_Get_Milli_Seconds();
	do
	{
		int ret = getLine(buf, sizeof(buf));
		if ((ret != WAIT) && (ret != NOT_FOUND))
		{
			int type = TYPE(ret);
			if(type == TYPE_RESULT)
			{
				const char* cmd = buf+7; //bypass \r\n@
				if (sscanf(cmd, "|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|$", &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9, &_Temp, &_PH, &_Salt, &_Oxy, &_NH3, &_H2S) == 15) 
				{
					return SENSOR_OK;
				}
			}
			if(type == TYPE_UNKNOWN)
			{
				#ifdef DEBUG_SENSOR_DIGITAL
				int len = LENGTH(ret);
				DEBUG_D("\nRecv \"UNK\": "); _dumpUART(buf, len);
				#endif
			}
		}
		if(_pipeRx.size() == (MAXSIZE_SENSOR_DIGITAL_PIPE-1)) purge();
		HAL_Delay_ms(50);	
	}while(!TIMEOUT(start, timeout_ms));
  return 0;
}

int SensorDigital::getLine(char* buffer, int length)
{
    return _getLine(&_pipeRx, buffer, length);
}

int SensorDigital::valueSalt(void)
{
    return _Salt;
}

int SensorDigital::valuePH(void)
{
    return _PH;
}

int SensorDigital::valueOxy(void)
{
    return _Oxy;
}

int SensorDigital::valueTemp(void)
{
    return _Temp;
}

int SensorDigital::valueNH3(void)
{
    return _NH3;
}

int SensorDigital::valueH2S(void)
{
    return _H2S;
}

int SensorDigital::_getLine(Pipe<char>* pipe, char* buf, int len)
{
    int unkn = 0;
    int sz = pipe->size();
    int fr = pipe->free();
    if (len > sz)
        len = sz;
    while (len > 0)
    {
        static struct {
              const char* fmt;                              int type;
        } lutF[] = {
            { "*|SHRIM|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|$",                   TYPE_RESULT    },
        };
        for (int i = 0; i < (int)(sizeof(lutF)/sizeof(*lutF)); i ++) {
            pipe->set(unkn);
            int ln = _parseFormated(pipe, len, lutF[i].fmt);
            if (ln == WAIT && fr)
                return WAIT;
            if ((ln != NOT_FOUND) && (unkn > 0))
                return TYPE_UNKNOWN | pipe->get(buf, unkn);
            if (ln > 0)
                return lutF[i].type  | pipe->get(buf, ln);
        }
        unkn ++;
        len--;
    }

    return WAIT;
}

int SensorDigital::_parseFormated(Pipe<char>* pipe, int len, const char* fmt)
{
    int o = 0;
    int num = 0;
    if (fmt) {
        while (*fmt) {
            if (++o > len)                  return WAIT;
            char ch = pipe->next();
            if (*fmt == '%') {
                fmt++;
                if (*fmt == 'd') { // numeric
                    fmt ++;
                    num = 0;
                    while (ch >= '0' && ch <= '9') {
                        num = num * 10 + (ch - '0');
                        if (++o > len)      return WAIT;
                        ch = pipe->next();
                    }
                }
                else if (*fmt == 'c') { // char buffer (takes last numeric as length)
                    fmt ++;
                    while (num --) {
                        if (++o > len)      return WAIT;
                        ch = pipe->next();
                    }
                }
                else if (*fmt == 's') {
                    fmt ++;
                    if (ch != '\"')         return NOT_FOUND;
                    do {
                        if (++o > len)      return WAIT;
                        ch = pipe->next();
                    } while (ch != '\"');
                    if (++o > len)          return WAIT;
                    ch = pipe->next();
                }
            }
            if (*fmt++ != ch)               return NOT_FOUND;
        }
    }
    return o;
}

void SensorDigital::_dumpUART(const char* buf, int len)
{
//    DEBUG_D(" %3d \"", len);
//    while (len --) {
//        char ch = *buf++;
//        if ((ch > 0x1F) && (ch < 0x7F)) { // is printable
//            if      (ch == '%')  DEBUG_D("%%");
//            else if (ch == '"')  DEBUG_D("\\\"");
//            else if (ch == '\\') DEBUG_D("\\\\");
//            else DEBUG_D("%c", ch);
//        } else {
//            if      (ch == '\a') DEBUG_D("\\a"); // BEL (0x07)
//            else if (ch == '\b') DEBUG_D("\\b"); // Backspace (0x08)
//            else if (ch == '\t') DEBUG_D("\\t"); // Horizontal Tab (0x09)
//            else if (ch == '\n') DEBUG_D("\\n"); // Linefeed (0x0A)
//            else if (ch == '\v') DEBUG_D("\\v"); // Vertical Tab (0x0B)
//            else if (ch == '\f') DEBUG_D("\\f"); // Formfeed (0x0C)
//            else if (ch == '\r') DEBUG_D("\\r"); // Carriage Return (0x0D)
//            else                 DEBUG_D("\\x%02x", (unsigned char)ch);
//        }
//    }
//    DEBUG_D("\"\r\n");
}
