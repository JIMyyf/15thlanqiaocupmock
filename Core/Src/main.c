/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "led.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint64_t usart_time = 0, time = 0; // 计算sleeping进入时间
int mode = 0, gear = 1;
double temp;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
bool start_time = false;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  led_init();
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  HAL_ADC_Start_IT(&hadc2);
  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
  HAL_UART_Receive_IT(&huart1, &USART1_RECEIVE_DATA, USART1_RECEIVE_BUUFER_SIZE);
  HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (USART1_RECEIVE_STATE & 0x8000)
    {
      USART1_RECEIVE_STATE = 0;
      time = 0;
      start_time = true;
      if (USART1_RECEIVE_ARRAY[0] == 'B')
      {
        if (USART1_RECEIVE_ARRAY[1] == '1')
        {
          if (mode == 3)
          {
            mode = 0;
            time = 0;
          }
          else
          {
            mode++;
            if (mode == 2)
              mode = 0;
            time = 0;
          }
        }
        else if (USART1_RECEIVE_ARRAY[1] == '2')
        {
          if (mode == 3)
          {
            mode = 0;
            time = 0;
          }
          else if (mode == 1)
          {
            gear++;
            if (gear == 4)
              gear = 1;
          }
        }
        else if (USART1_RECEIVE_ARRAY[1] == '3')
        {
          if (mode == 3)
          {
            mode = 0;
            time = 0;
          }
          else if (mode == 1)
          {
            gear--;
            if (gear == 0)
              gear = 3;
          }
        }
        else
        {
          printf("NULL\r\n");
        }
      }
      else
      {
        printf("NULL\r\n");
      }
    }
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == 0)
    {
      while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == 0)
        ;
      mode++;
      if (mode == 2)
        mode = 0;
      time = 0;
    }
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0)
    {
      while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0)
        ;
      if (mode == 1)
      {
        gear++;
        if (gear == 4)
          gear = 1;
      }
      else if (mode == 3)
      {
        mode = 0;
      }
      time = 0;
    }
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0)
    {
      while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0)
        ;
      if (mode == 1)
      {
        gear--;
        if (gear == 0)
          gear = 3;
      }
      else if (mode == 3)
      {
        mode = 0;
      }
      time = 0;
    }
    else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0)
    {
      while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0)
        ;
      time = 0;
    }
    if (gear == 1)
    {
      led_ON(1);
      led_OFF(2);
      led_OFF(3);
    }
    else if (gear == 2)
    {
      led_OFF(1);
      led_ON(2);
      led_OFF(3);
    }
    else
    {
      led_OFF(1);
      led_OFF(2);
      led_ON(3);
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
char text[30];
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    time++; // 时间自增
    if (time >= 50)
    {
      mode = 3;
    }
    if (start_time == true && usart_time < 30)
    {
      led_ON(4);
      usart_time++;
    }
    else
    {
      start_time = false;
      usart_time = 0;
    }
    // 获取温度
    double v = getADC(&hadc2);
    if (v >= 3)
      temp = 40;
    else if (v <= 1)
      temp = 10;
    else
      temp = 10 * v + 10;

    if (mode == 0)
    {
      led_ON(8);
      if (temp < 25)
        gear = 1;
      else if (temp > 30)
        gear = 3;
      else
        gear = 2;

      LCD_DisplayStringLine(Line2, (u8 *)"        DATA      ");
      sprintf(text, "     TEMP:%.1lf  ", temp);
      LCD_DisplayStringLine(Line4, (u8 *)text);
      LCD_DisplayStringLine(Line5, (u8 *)"     MODE=AUTO  ");
      sprintf(text, "     GEAR:%d  ", gear);
      LCD_DisplayStringLine(Line6, (u8 *)text);
    }
    else if (mode == 1)
    {
      led_OFF(8);
      LCD_DisplayStringLine(Line2, (u8 *)"        DATA      ");
      sprintf(text, "     TEMP:%.1lf  ", temp);
      LCD_DisplayStringLine(Line4, (u8 *)text);
      LCD_DisplayStringLine(Line5, (u8 *)"     MODE=MANU  ");
      sprintf(text, "     GEAR:%d  ", gear);
      LCD_DisplayStringLine(Line6, (u8 *)text);
    }
    else if (mode == 3)
    {
      led_OFF(8);
      LCD_DisplayStringLine(Line2, (u8 *)"                  ");
      LCD_DisplayStringLine(Line4, (u8 *)"                  ");
      LCD_DisplayStringLine(Line5, (u8 *)"     SLEEPING  ");
      sprintf(text, "     GEAR:%d  ", gear);
      LCD_DisplayStringLine(Line6, (u8 *)text);
    }
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    if (gear == 1)
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 10);
    else if (gear == 2)
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 40);
    else
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 80);
    HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
  }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
