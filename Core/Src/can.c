#include "can.h"
#include <stdint.h>
CAN_HandleTypeDef hcan1;
uint8_t TxData[8];
uint8_t RxData[8];
uint32_t MailBox;
CAN_TxHeaderTypeDef txheader;
CAN_RxHeaderTypeDef rxheader;
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
static void CAN_Filter_Config(void)
{
    CAN_FilterTypeDef canfilter;

    canfilter.FilterBank = 0;
    canfilter.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilter.FilterScale = CAN_FILTERSCALE_32BIT;
    canfilter.FilterIdHigh = 0x0000;
    canfilter.FilterIdLow = 0x0000;
    canfilter.FilterMaskIdHigh = 0x0000; 
    canfilter.FilterMaskIdLow = 0x0000;  
    canfilter.FilterFIFOAssignment = CAN_RX_FIFO0;
    canfilter.FilterActivation = ENABLE;
    canfilter.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan1, &canfilter) != HAL_OK)
    {
        Error_Handler();
    }
}
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
}

void CAN_TxHeader_Config(void)
{
    txheader.StdId = 0x100; // STM32'nin ID'si
    txheader.ExtId = 0;
    txheader.IDE = CAN_ID_STD;
    txheader.RTR = CAN_RTR_DATA;
    txheader.DLC = 8;
    txheader.TransmitGlobalTime = DISABLE;
}
void CAN_SendDHT22(uint16_t temp, uint16_t hum)
{
    TxData[0] = (temp >> 8) & 0xFF; // Temp High
    TxData[1] = temp & 0xFF;        // Temp Low
    TxData[2] = (hum >> 8) & 0xFF;  // Hum High
    TxData[3] = hum & 0xFF;         // Hum Low
    TxData[4] = 0x00;
    TxData[5] = 0x00;
    TxData[6] = 0x00;
    TxData[7] = 0x00;

    HAL_CAN_AddTxMessage(&hcan1, &txheader, TxData, &MailBox);
}




void MX_CAN1_Init(void)
{
  MX_GPIO_Init();//can icin
  txheader.StdId = 0x100;

  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_LOOPBACK;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  CAN_Filter_Config();
}




// void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
// {
//   HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxheader, RxData);

//   // Raspberry Pi'den gelen komutlarÄ± iÅŸle
//   if (rxheader.StdId == 0x200) // RPi'nin ID'si
//   {
//     if (RxData[0] == 0x01)
//     {
//       // Ã–rnek: LED yak
//     }
//     else if (RxData[0] == 0x02)
//     {
//       // Ã–rnek: SensÃ¶r oku ve gÃ¶nder
//       CAN_SendDHT22(temperature, humidity);
//     }
//   }
// }
