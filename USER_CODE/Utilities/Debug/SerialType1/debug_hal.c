#include <stdarg.h>
#include "usart.h"
#include "debug_hal.h"

/* ===[ DEFINE SYSTEM FEATURE ]=== */
#define ENABLED_DEBUG
#define ENABLED_DEBUG_WITH_COLOUR
#define DEBUG_MAX_CHAR_PRINT	256

/* ===[ DEFINE DEBUG COLOUR ]=== */
//Ref: http://misc.flogisoft.com/bash/tip_colors_and_formatting
#if 0 // colored terminal output using ANSI escape sequences
  #define COL(c) "\033[" c
#else
  #define COL(c) ""
#endif
#define DEF COL("39m") 		//Default foreground color
#define BLA COL("30m")
#define RED COL("31m")
#define GRE COL("32m")
#define YEL COL("33m")
#define BLU COL("34m")
#define MAG COL("35m")
#define CYA COL("36m")
#define WHY COL("37m")
#define LRED COL("91m")		//Light color
#define LGRE COL("92m")
#define LYEL COL("93m")
#define LBLU COL("94m")
#define LMAG COL("95m")
#define LCYA COL("36m")
#define BGDEF COL("49m")	//Background color
#define BGRED COL("41m")
#define BGGRE COL("42m")
#define BGYEL COL("43m")
#define BGBLU COL("44m")
#define BGMAG COL("45m")
#define BGCYA COL("46m")

static int _debugLevel = 3;

/* ===[ FUNTCIONS DEBUG ]=== */
void DEBUG_D(const char* format, ...)
{
	#ifdef ENABLED_DEBUG
	char buffer[DEBUG_MAX_CHAR_PRINT];
	uint32_t len;
	va_list vArgs;		    
	va_start(vArgs, format);
	len = vsprintf((char *)&buffer[0], (char const *)format, vArgs);
	va_end(vArgs);
	if(len >= DEBUG_MAX_CHAR_PRINT) len = DEBUG_MAX_CHAR_PRINT;
//	for(i = 0;i < len; i++)
//	{
//		HAL_UART_Transmit(&huart1, &buffer[i], 1, 100);
//	}
	UART3_Send(buffer, len);
	#endif
}

void _debugPrint(int level, const char* color, const char* format, ...)
{
    if (_debugLevel >= level)
    {
        va_list args;
        va_start (args, format);
        if (color) DEBUG_D(color);
        DEBUG_D(format, args);
        if (color) DEBUG_D(DEF);
        va_end (args);
        DEBUG_D("\r\n");
    }
}

int SYSLOG_SetDebugLevel(int level)
{
	if ((_debugLevel >= -1) && (level >= -1) &&
        (_debugLevel <=  3) && (level <=  3)) {
        _debugLevel = level;
        return 1;
    }
    else return 0;
}

void SYSLOG_TEST(const char* format, ...)
{
	va_list args;
	va_start (args, format);
	#ifdef ENABLED_DEBUG_WITH_COLOUR
		_debugPrint(3, CYA, format, args);
	#else
		_debugPrint(3, 0, format, args);
	#endif
	va_end(args);
}

void SYSLOG_INFO(const char* format, ...)
{
	va_list args;
	va_start (args, format);
	#ifdef ENABLED_DEBUG_WITH_COLOUR
		_debugPrint(2, GRE, format, args);
	#else
		_debugPrint(2, 0, format, args);
	#endif
	va_end(args);
}

void SYSLOG_WARN(const char* format, ...)
{
	va_list args;
	va_start (args, format);
	#ifdef ENABLED_DEBUG_WITH_COLOUR
		_debugPrint(1, YEL, format, args);
	#else
		_debugPrint(1, 0, format, args);
	#endif
	va_end(args);
}

void SYSLOG_ERROR(const char* format, ...)
{
	va_list args;
	va_start (args, format);
	#ifdef ENABLED_DEBUG_WITH_COLOUR
		_debugPrint(0, RED, format, args);
	#else
		_debugPrint(0, 0, format, args);
	#endif
	va_end(args);
}

void SYSLOG_DUMP(const char* buf, int len)
{
	DEBUG_D(" %3d \"", len);
    while (len --) {
        char ch = *buf++;
        if ((ch > 0x1F) && (ch < 0x7F)) { // is printable
            if      (ch == '%')  DEBUG_D("%%");
            else if (ch == '"')  DEBUG_D("\\\"");
            else if (ch == '\\') DEBUG_D("\\\\");
            else DEBUG_D("%c", ch);
        } else {
            if      (ch == '\a') DEBUG_D("\\a"); // BEL (0x07)
            else if (ch == '\b') DEBUG_D("\\b"); // Backspace (0x08)
            else if (ch == '\t') DEBUG_D("\\t"); // Horizontal Tab (0x09)
            else if (ch == '\n') DEBUG_D("\\n"); // Linefeed (0x0A)
            else if (ch == '\v') DEBUG_D("\\v"); // Vertical Tab (0x0B)
            else if (ch == '\f') DEBUG_D("\\f"); // Formfeed (0x0C)
            else if (ch == '\r') DEBUG_D("\\r"); // Carriage Return (0x0D)
            else                 DEBUG_D("\\x%02x", (unsigned char)ch);
        }
    }
    DEBUG_D("\"\r\n");
}

