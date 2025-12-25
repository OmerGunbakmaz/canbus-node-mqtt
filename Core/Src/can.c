#include "can.h"

static CAN_TxHeaderTypeDef txheader;
static CAN_FilterTypeDef canfilter;
static uint32_t TxMailbox;
static uint8_t TxData[8];

CAN_RxHeaderTypeDef rxheader;
uint8_t RxData[8];

/**
 * @brief TX Header ayarları
 */
static void CAN_TxHeader_Config(void)
{
    txheader.StdId = 0x100;
    txheader.ExtId = 0;
    txheader.IDE = CAN_ID_STD;
    txheader.RTR = CAN_RTR_DATA;
    txheader.DLC = 8;
    txheader.TransmitGlobalTime = DISABLE;
}

/**
 * @brief Filter ayarları (tüm mesajları al)
 */
static CAN_Status CAN_Filter_Config(CAN_HandleTypeDef *hcan)
{
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

    if (HAL_CAN_ConfigFilter(hcan, &canfilter) != HAL_OK)
    {
        return CAN_ERROR_FILTER;
    }
    return CAN_OK;
}

/**
 * @brief CAN başlatma (Filter + Start + Interrupt)
 */
CAN_Status CAN_User_Init(CAN_HandleTypeDef *hcan)
{
    CAN_TxHeader_Config();

    if (CAN_Filter_Config(hcan) != CAN_OK)
    {
        return CAN_ERROR_FILTER;
    }

    if (HAL_CAN_Start(hcan) != HAL_OK)
    {
        return CAN_ERROR_START;
    }

    if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        return CAN_ERROR_INIT;
    }

    return CAN_OK;
}

/**
 * @brief TX ID değiştir
 */
void CAN_SetTxID(uint16_t id)
{
    txheader.StdId = id;
}

/**
 * @brief Ham veri gönderme
 */
CAN_Status CAN_SendData(uint8_t *data, uint8_t len)
{
    if (len > 8) len = 8;
    
    txheader.DLC = len;

    if (HAL_CAN_AddTxMessage(&hcan1, &txheader, data, &TxMailbox) != HAL_OK)
    {
        return CAN_ERROR_TX;
    }
    return CAN_OK;
}

/**
 * @brief DHT22 verisi gönderme
 */
CAN_Status CAN_SendDHT22(uint16_t temp, uint16_t hum)
{
    TxData[0] = (temp >> 8) & 0xFF;
    TxData[1] = temp & 0xFF;
    TxData[2] = (hum >> 8) & 0xFF;
    TxData[3] = hum & 0xFF;
    TxData[4] = 0x00;
    TxData[5] = 0x00;
    TxData[6] = 0x00;
    TxData[7] = 0x00;

    return CAN_SendData(TxData, 8);
}

/**
 * @brief RX Callback (main.c veya stm32f4xx_it.c'de çağrılabilir)
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxheader, RxData);

    if (rxheader.StdId == 0x200)
    {
    }
}