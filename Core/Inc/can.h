#ifndef CAN_H
#define CAN_H

#include "stm32f4xx_hal.h"

typedef enum {
    CAN_OK = 0,
    CAN_ERROR_INIT,
    CAN_ERROR_FILTER,
    CAN_ERROR_START,
    CAN_ERROR_TX
} CAN_Status;

CAN_Status CAN_User_Init(CAN_HandleTypeDef *hcan);
CAN_Status CAN_SendData(uint8_t *data, uint8_t len);
CAN_Status CAN_SendDHT22(uint16_t temp, uint16_t hum);
void CAN_SetTxID(uint16_t id);

extern CAN_HandleTypeDef hcan1;
extern CAN_RxHeaderTypeDef rxheader;
extern uint8_t RxData[8];

#endif