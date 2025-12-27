#ifndef CAN_H
#define CAN_H
#include "stm32f4xx_hal.h"
void MX_CAN1_Init(void);
void CAN_TxHeader_Config(void);
void CAN_SendDHT22(uint16_t temp, uint16_t hum);
void Error_Handler(void);
extern CAN_HandleTypeDef hcan1;


#endif