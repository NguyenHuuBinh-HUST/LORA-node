#ifndef SENSORANALOG_H
#define SENSORANALOG_H

#include <stdio.h>
#include "adc.h"

uint32_t Read_ADC_Value(void);
float Convert_ADC_To_Voltage(uint32_t adcValue);
void Float_To_String(float value, char* buffer);
extern void get_ADC_Value();


#endif 