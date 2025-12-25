
#include "dht22.h"
#include "stm32f4xx_hal.h"
#define PRIVATE static

TIM_HandleTypeDef htim3;
PRIVATE GPIO_TypeDef *wire_port;
PRIVATE uint16_t wire_pin;
uint8_t err;
PRIVATE void timer_config(void)
{
    RCC_ClkInitTypeDef myClkTypedef;

    uint32_t clockSpeed;
    uint32_t flashLatency;
    HAL_RCC_GetClockConfig(&myClkTypedef, &flashLatency);
    if (myClkTypedef.APB1CLKDivider == RCC_HCLK_DIV1)
    {
        clockSpeed = HAL_RCC_GetPCLK1Freq();
    }
    else
    {
        clockSpeed = HAL_RCC_GetPCLK1Freq() * 2;
    }

    clockSpeed *= 0.000001;

    __HAL_RCC_TIM3_CLK_ENABLE();

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = clockSpeed - 1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 0xFFFF;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&htim3);

    __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
}

PRIVATE void delay_(uint32_t uSecDelay)
{
    if (uSecDelay > 0xffff)
        uSecDelay = 0xffff;
    else if (uSecDelay == 0)
        return;

    __HAL_TIM_SET_AUTORELOAD(&htim3, uSecDelay - 1);
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
    HAL_TIM_Base_Start(&htim3);

    while (__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_UPDATE) == RESET)
        ;
    HAL_TIM_Base_Stop(&htim3);
}

void DHT22_Init(GPIO_TypeDef *Port, uint16_t Pin)
{
    wire_port = Port;
    wire_pin = Pin;
    if (wire_port == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (wire_port == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (wire_port == GPIOC)
        __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (wire_port == GPIOD)
        __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (wire_port == GPIOE)
        __HAL_RCC_GPIOE_CLK_ENABLE();
    timer_config();
}

PRIVATE void gpio_output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = wire_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    
    HAL_GPIO_Init(wire_port, &GPIO_InitStruct);
}
PRIVATE void gpio_input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = wire_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(wire_port, &GPIO_InitStruct);
}

PRIVATE void Start_Dht22(void)
{
    gpio_output();
    HAL_GPIO_WritePin(wire_port, wire_pin, GPIO_PIN_RESET);
    delay_(18000);
    HAL_GPIO_WritePin(wire_port, wire_pin, GPIO_PIN_SET);
    delay_(30);
    gpio_input();
}


PRIVATE void Response_Dht22(void)
{
    err = 1;
    uint16_t timeout;
    
    timeout = 1000;
    while (HAL_GPIO_ReadPin(wire_port, wire_pin) == GPIO_PIN_SET && timeout--)
        ;
    if (timeout == 0) return;
    
    timeout = 1000;
    while (HAL_GPIO_ReadPin(wire_port, wire_pin) == GPIO_PIN_RESET && timeout--)
        ;
    if (timeout == 0) return;
    
    timeout = 1000;
    while (HAL_GPIO_ReadPin(wire_port, wire_pin) == GPIO_PIN_SET && timeout--)
        ;
    if (timeout == 0) return;
    
    err = 0;   
}

PRIVATE uint8_t Read_Data_Dht22()
{
    uint8_t i = 0, j;
    uint16_t timeout;
    
    for (j = 0; j < 8; j++)
    {
        timeout = 1000;
        while (HAL_GPIO_ReadPin(wire_port, wire_pin) == GPIO_PIN_RESET && timeout--)
            ;
        if (timeout == 0) { err = 3; return 0; }
        
        delay_(30);         
        if (HAL_GPIO_ReadPin(wire_port, wire_pin) == GPIO_PIN_SET)
            i |= (1 << (7 - j));  // Bit 1
        
        timeout = 1000;
        while (HAL_GPIO_ReadPin(wire_port, wire_pin) == GPIO_PIN_SET && timeout--)
            ;
        if (timeout == 0) { err = 3; return 0; }
    }
    
    return i;
}
uint8_t DHT22_Get_Data(dht22_data *data)
{
    __disable_irq();      
    err = 0;
    Start_Dht22();
    Response_Dht22();
    
    if (err == 1) {
        __enable_irq();  
        return err;
    }

    data->data[0] = Read_Data_Dht22();
    data->data[1] = Read_Data_Dht22();
    data->data[2] = Read_Data_Dht22();
    data->data[3] = Read_Data_Dht22();
    data->data[4] = Read_Data_Dht22();
    data->checksum = ((data->data[0]+data->data[1]+data->data[2]+data->data[3])&0xFF);
    __enable_irq();  
    data->sum = data->data[4]+1;
    data->temperature = ((data->data[2] << 8) | data->data[3])>>1;
    data->humidity = ((data->data[0] << 8) | data->data[1])>>1;

    if (data->checksum != data->sum)
        return 2;

    return err;
}