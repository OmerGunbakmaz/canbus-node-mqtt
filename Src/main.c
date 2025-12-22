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
    dht22_data data = {0};
    while (1)
    {
        uint8_t result = DHT22_Get_Data(&data);

        if (result == 0)
        {
            printf("Temp = %d.%dCelcius - Hum = %d.%d%% \r\n",
                   data.temperature / 10, data.temperature % 10, data.humidity / 10, data.humidity % 10);
        }
        else if (result == 1)
        {
            printf("DHT22 yanit vermiyor!\r\n");
        }
        else if (result == 2)
        {
            printf("Checksum hatasi! Temp = %d.%dCelcius - Hum = %d.%d%%  \r\n  temp_byte1 = %d , temp_byte2 = %d ,hum_byte1 = %d , hum_byte2 = %d Sum = %d , calculation = %d\r\n", data.temperature / 10, data.temperature % 10, data.humidity / 10, data.humidity % 10, data.data[2], data.data[3], data.data[0], data.data[1], data.sum, data.checksum);
        }
        else if (result == 3)
        {
            printf("Bit okuma timeout!\r\n");
        }

        HAL_Delay(2000);
    }
}
void SysTick_Handler(void)
{
    HAL_IncTick();
}