#include "stm32f4xx_hal.h"
#ifndef DHT22_H
#define DHT22_H

void DHT22_Init(GPIO_TypeDef *Port, uint16_t Pin); // DHT22 initialize etme
uint8_t DHT22_Get_Data(uint16_t *Temp, uint16_t *Hum);
extern TIM_HandleTypeDef htim3;

extern uint8_t Hum_byte1, Hum_byte2, Temp_byte1, Temp_byte2;
extern uint16_t Sum;



#endif