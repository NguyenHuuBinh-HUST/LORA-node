#ifndef DEBUG_HAL_H
#define DEBUG_HAL_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void DEBUG_D(const char* format, ...);
int SYSLOG_SetDebugLevel(int level);
void SYSLOG_TEST(const char* format, ...);
void SYSLOG_INFO(const char* format, ...);
void SYSLOG_WARN(const char* format, ...);
void SYSLOG_ERROR(const char* format, ...);
void SYSLOG_DUMP(const char* buf, int len);

#ifdef __cplusplus
}
#endif

#endif
