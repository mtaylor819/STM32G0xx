/*
  stm32g0xx_hal_msp.c - MSP Initialization for the grblHAL STM32G0xx driver
*/

#include "main.h"
#include "driver.h"

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}
