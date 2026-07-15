/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "grbllib.h"

void SystemClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();

    grbl_enter();
}

/*
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* HSE 8 MHz -> PLL -> 64 MHz SYSCLK */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN = 16;                    // VCO = 8 * 16 = 128 MHz
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;         // SYSCLK = 128 / 2 = 64 MHz
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        Error_Handler();

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
        Error_Handler();

#if USB_SERIAL_CDC

    /* USB clock: HSI48 trimmed against USB SOF via CRS */

    RCC_OscInitTypeDef HSI48_Init = {
        .OscillatorType = RCC_OSCILLATORTYPE_HSI48,
        .HSI48State = RCC_HSI48_ON,
        .PLL.PLLState = RCC_PLL_NONE
    };
    if (HAL_RCC_OscConfig(&HSI48_Init) != HAL_OK)
        Error_Handler();

    RCC_PeriphCLKInitTypeDef PeriphClkInit = {
        .PeriphClockSelection = RCC_PERIPHCLK_USB,
        .UsbClockSelection = RCC_USBCLKSOURCE_HSI48
    };
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
        Error_Handler();

    __HAL_RCC_CRS_CLK_ENABLE();

    RCC_CRSInitTypeDef CRS_Init = {
        .Prescaler = RCC_CRS_SYNC_DIV1,
        .Source = RCC_CRS_SYNC_SOURCE_USB,
        .Polarity = RCC_CRS_SYNC_POLARITY_RISING,
        .ReloadValue = RCC_CRS_RELOADVALUE_DEFAULT,
        .ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT,
        .HSI48CalibrationValue = RCC_CRS_HSI48CALIBRATION_DEFAULT
    };
    HAL_RCCEx_CRSConfig(&CRS_Init);

#endif // USB_SERIAL_CDC
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
