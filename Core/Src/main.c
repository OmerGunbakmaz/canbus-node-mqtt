/* USER CODE BEGIN Header */
#include "main.h"
#include "dht22.h"
#include "can.h"
#include <stdio.h>
#include "uart.h"

void Error_Handler(void);
void SystemClock_Config(void);
void Leds_Init_Discovery(void);
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  DHT22_Init(GPIOE, GPIO_PIN_3);//dht22 sensor initialization
  dht22_data dht22_data = {0};
  
  Leds_Init_Discovery();//Leds that i need for debugging can bus in discovery board
  MX_CAN1_Init();
  CAN_TxHeader_Config();

  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); // Red = Error
    while (1)
      ;
  }

  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET); // Green = CAN Bus is working

  while (1)
  {
    DHT22_Get_Data(&dht22_data);
    CAN_SendDHT22(dht22_data.temperature, dht22_data.humidity);
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);//Orange = Data sent
    HAL_Delay(500);
  }
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}
void Leds_Init_Discovery(void)
{
  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitTypeDef gpio = {0};
  gpio.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &gpio);
}