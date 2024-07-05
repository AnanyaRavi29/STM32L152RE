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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t transfer_cplt;
RTC_TimeTypeDef sTime ;
  RTC_DateTypeDef sDate ;
  char time[30];
  char date[30];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void microDelay(uint32_t time)
{
	uint32_t start=__HAL_TIM_GetCounter(&htim6);
	while((__HAL_TIM_GetCounter(&htim6)-start)<time)
	{

	}

}
#define DHT22_PORT GPIOA
#define DHT22_PIN GPIO_PIN_8
uint8_t hum1, hum2, tempC1, tempC2, SUM, CHECK;
uint32_t pMillis, cMillis;
float temp_Celsius = 0;
float temp_Fahrenheit = 0;
float Humidity = 0;
uint8_t hum_integral, hum_decimal, tempC_integral, tempC_decimal, tempF_integral, tempF_decimal;
char string[15];


uint8_t DHT22_Start (void)
{
  uint8_t Response = 0;
  GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
  GPIO_InitStructPrivate.Pin = DHT22_PIN;
  GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStructPrivate);
  HAL_GPIO_WritePin (DHT22_PORT, DHT22_PIN, 0);
  microDelay (1300);
  HAL_GPIO_WritePin (DHT22_PORT, DHT22_PIN, 1);
  microDelay (30);
  GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStructPrivate);
  microDelay (40);
  if (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)))
  {
    microDelay (80);
    if ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN))) Response = 1;
  }
  pMillis = HAL_GetTick();
  cMillis = HAL_GetTick();
  while ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)) && pMillis + 2 > cMillis)
  {
    cMillis = HAL_GetTick();
  }
  return Response;
}

uint8_t DHT22_Read (void)
{
  uint8_t x,y;
  for (x=0;x<8;x++)
  {
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)) && pMillis + 2 > cMillis)
    {
      cMillis = HAL_GetTick();
    }
    microDelay (40);
    if (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)))   // if the pin is low
      y&= ~(1<<(7-x));
    else
      y|= (1<<(7-x));
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)) && pMillis + 2 > cMillis)
    {  // wait for the pin to go low
      cMillis = HAL_GetTick();
    }
  }
  return y;
}

char charData1[20];
char charData2[20];
char charData3[20];
char charData4[20];

char time_data[20];
uint32_t total_run_time_ms = 0;
uint32_t active_time_ms = 0;
uint32_t standby_time_ms = 0;
uint32_t start_time_ms = 0;
uint32_t end_time_ms = 0;
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
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  HAL_TIM_Base_Start(&htim6);



	        if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	          {
	            /* Clear Standby flag */
	        	start_time_ms = HAL_GetTick();
	            __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
	            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1);
	            char *wakeUpMsg = "MCU has woken up from standby mode.\r\n";
	                HAL_UART_Transmit(&huart2, (uint8_t *)wakeUpMsg, strlen(wakeUpMsg), HAL_MAX_DELAY);
	                HAL_Delay(1000);

	                char *wakeUpMsg1 = "Collecting sensor data.\r\n";
	                        HAL_UART_Transmit(&huart2, (uint8_t *)wakeUpMsg1, strlen(wakeUpMsg1), HAL_MAX_DELAY);

	                        HAL_Delay(2000);
	                        	  if(DHT22_Start())
	                        	  	     {


	                        	  	       hum1 = DHT22_Read();
	                        	  	       hum2 = DHT22_Read();
	                        	  	       tempC1 = DHT22_Read();
	                        	  	       tempC2 = DHT22_Read();
	                        	  	       SUM = DHT22_Read();
	                        	  	       CHECK = hum1 + hum2 + tempC1 + tempC2;
	                        	  	       if (CHECK == SUM)
	                        	  	       {
	                        	  	         if (tempC1>127)
	                        	  	         {
	                        	  	           temp_Celsius = (float)tempC2/10*(-1);
	                        	  	         }
	                        	  	         else
	                        	  	         {
	                        	  	           temp_Celsius = (float)((tempC1<<8)|tempC2)/10;
	                        	  	         }

	                        	  	         temp_Fahrenheit = temp_Celsius * 9/5 + 32;

	                        	  	         Humidity = (float) ((hum1<<8)|hum2)/10;


	                        	  	         hum_integral = Humidity;
	                        	  	         hum_decimal = Humidity*10-hum_integral*10;

	                        	  	         printf("Relative Humidity : %d.%d %%  \n", hum_integral, hum_decimal);
	                        	  	         sprintf(charData1,"Relative Humidity is : %d.%d \r\n",hum_integral,hum_decimal);
	                        	  	       HAL_UART_Transmit(&huart2,(uint8_t *)charData1,strlen(charData1),1000);




	                        	  	           tempC_integral = temp_Celsius;
	                        	  	           tempC_decimal = temp_Celsius*10-tempC_integral*10;


	                        	  	           printf("Temp in Celcius %d.%d C   \n", tempC_integral, tempC_decimal);
	                        	  	         sprintf(charData2,"Temp in Celcius: %d.%d C \r\n",tempC_integral, tempC_decimal);
	                        	  	        HAL_UART_Transmit(&huart2,(uint8_t *)charData2,strlen(charData2),1000);



	                        	  	           tempF_integral = temp_Fahrenheit;
	                        	  	           tempF_decimal = temp_Fahrenheit*10-tempF_integral*10;


	                        	  	           printf("Temp in Fahrenheit %d.%d F   \n", tempF_integral, tempF_decimal);
	                        	  	         sprintf(charData3,"Temp in Fahrenheit: %d.%d F \r\n",tempF_integral, tempF_decimal);
	                        	  	        HAL_UART_Transmit(&huart2,(uint8_t *)charData3,strlen(charData3),1000);

	                        	  	      sprintf(charData4,"*************************************************************** \r\n");
	                        	  	      	  	        HAL_UART_Transmit(&huart2,(uint8_t *)charData4,strlen(charData4),1000);

	                        	  	       }
	                        	  	     }
//	                        	  sprintf(time_data, "Total Run Time: %lu ms \r\n", total_run_time_ms);
//	                        	  HAL_UART_Transmit(&huart2, (uint8_t *)time_data, strlen(time_data), HAL_MAX_DELAY);
	                        	  end_time_ms = HAL_GetTick();
	                        	  	          active_time_ms = end_time_ms - start_time_ms;
	                        	  sprintf(time_data, "Active Time: %lu ms \r\n", active_time_ms);
	                        	  HAL_UART_Transmit(&huart2, (uint8_t *)time_data, strlen(time_data), HAL_MAX_DELAY);

//	                        	  sprintf(time_data, "Standby Time: %lu ms \r\n", standby_time_ms);
//	                        	  HAL_UART_Transmit(&huart2, (uint8_t *)time_data, strlen(time_data), HAL_MAX_DELAY);

	          }


	        HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

	          /* Re-enable all used wakeup sources*/
	          /* ## Setting the Wake up time ############################################*/
	          /*  RTC Wakeup Interrupt Generation:
	            Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
	            Wakeup Time = Wakeup Time Base * WakeUpCounter
	              = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI)) * WakeUpCounter
	              ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

	            To configure the wake up timer to 4s the WakeUpCounter is set to 0x1FFF:
	            RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
	            Wakeup Time Base = 16 /(~39.000KHz) = ~0,410 ms
	            Wakeup Time = ~4s = 0,410ms  * WakeUpCounter
	              ==> WakeUpCounter = ~4s/0,410ms = 9750 = 0x2616 */
	          HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x5F86, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

	          /* Clear all related wakeup flags */
	          __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	          HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,0);


	          char *standbyMsg = "MCU is entering standby mode.\r\n";
	            HAL_UART_Transmit(&huart2, (uint8_t *)standbyMsg, strlen(standbyMsg), HAL_MAX_DELAY);
	          /* Enter the Standby mode */
	            //standby_time_ms = HAL_GetTick() - active_time_ms;
	          HAL_PWR_EnterSTANDBYMode();
	    /* USER CODE END 2 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCCEx_EnableLSECSS();
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x1;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS
                              |RTC_ALARMMASK_MINUTES;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer(&hrtc, 0, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 32-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 0xffff;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	// Callback called from the RTC interrupt service routine
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
