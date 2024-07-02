#include <stdio.h>
#include "adc.h"
#include "sensorAnalog.h"
char sensor_Buffer[20];
uint32_t adcValue;

extern void get_ADC_Value(void);
uint32_t Read_ADC_Value(void);
float Convert_ADC_To_Voltage(uint32_t adcValue);
void Float_To_String(float value, char* buffer);



void get_ADC_Value(){
		adcValue = Read_ADC_Value();
		float voltage = Convert_ADC_To_Voltage(adcValue);
		Float_To_String(voltage, sensor_Buffer);
	
}

uint32_t Read_ADC_Value(void) {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    return adcValue;
}
float Convert_ADC_To_Voltage(uint32_t adcValue) {
    float voltage = (adcValue * 5) / 1024.0;
    return voltage;
}
void Float_To_String(float value, char* buffer) {
    sprintf(buffer, "%.2f", value);
}