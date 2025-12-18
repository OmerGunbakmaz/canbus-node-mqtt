#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "uart.h"
#include "dht22.h"

void SysTick_Handler(void);

int main(void)
{
    HAL_Init();
    UART2_Init_GPIOA();
    DHT22_Init(GPIOE, GPIO_PIN_3);
    uint16_t temp = 0;
    uint16_t hum = 0;

    while (1)
    {
        uint8_t result = DHT22_Get_Data(&temp, &hum);

        if (result == 0)
        {
            printf("Temp = %d.%d°C - Hum = %d.%d%% \r\n",
                   temp / 10, temp % 10, hum / 10, hum % 10);
        }
        else if (result == 1)
        {
            printf("DHT22 yanit vermiyor!\r\n");
        }
        else if (result == 2)
        {
            printf("Checksum hatasi!\r\n");
        }

        HAL_Delay(2000);
    }
}
void SysTick_Handler(void)
{
    HAL_IncTick();
}