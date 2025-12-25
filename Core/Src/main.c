#include "main.h"
#include "can.h"
#include "dht22.h"

CAN_HandleTypeDef hcan1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);

int main(void)
{
    dht22_data dht22_data = {0};
    uint8_t dht_status;

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_CAN1_Init();
    DHT22_Init(GPIOE, GPIO_PIN_3);

    // CAN başlat - tek satır!
    if (CAN_User_Init(&hcan1) != CAN_OK)
    {
        Error_Handler();
    }

    while (1)
    {
        dht_status = DHT22_Get_Data(&dht22_data);

        if (dht_status == 0)
        {
            CAN_SendDHT22(dht22_data.temperature, dht22_data.humidity);
        }

        HAL_Delay(2000);
    }
}
