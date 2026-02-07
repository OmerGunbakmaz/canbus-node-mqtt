#include "stm32f4xx_hal.h"
#ifndef DHT22_H
#define DHT22_H


typedef struct dth22_data{
  uint16_t temperature;
  uint16_t humidity;
  uint8_t sum;
  uint8_t checksum;
  uint8_t data[8];
}dht22_data; 

void DHT22_Init(GPIO_TypeDef *Port, uint16_t Pin); // DHT22 initialize etme
uint8_t DHT22_Get_Data(dht22_data *data);
extern TIM_HandleTypeDef htim3;

extern uint16_t Sum;

#endif