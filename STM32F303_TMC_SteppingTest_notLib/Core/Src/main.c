/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include <stdio.h>
#include "tmc/ic/TMC2209/TMC2209.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TMC2209_CRC(data, length) tmc_CRC8(data, length, 1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//static TMC2209TypeDef TMC2209; //TMC2209の設定などの定義
//static ConfigurationTypeDef *TMC2209_config; //TMC2209の設定
TMC2209TypeDef TMC2209; //TMC2209の設定などの定義
ConfigurationTypeDef TMC2209_config; //TMC2209の設定
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
static void periodicJob(uint32_t tick);
static uint8_t reset(void);
static uint8_t restore(void);
//static TMC2209TypeDef TMC2209;


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim3){ //htim3に設定したタイマー割り込みの時間で発火する
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);//ピンの出力を切り替え
    }
}*/
void tmc2209_readWriteArray(uint8_t channel, uint8_t *data, size_t writeLength, size_t readLength)
{
	//set your uart read write
	//UART_readWrite(channelToUART(channel), data, writeLength, readLength);
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	HAL_UART_Transmit(&huart1, data, writeLength,3000);

	if(readLength > 0){
		HAL_HalfDuplex_EnableReceiver(&huart1);
		HAL_UART_Receive(&huart1, data, readLength, 3000);
	}
}

uint8_t tmc2209_CRC8(uint8_t *data, size_t length)
{
	return TMC2209_CRC(data, length);
}

static void periodicJob(uint32_t tick)
{

	tmc2209_periodicJob(&TMC2209, tick);

	//StepDir_periodicJob(0);
}

static uint8_t reset()
{
	//StepDir_init(STEPDIR_PRECISION);
	//StepDir_setPins(0, Pins.STEP, Pins.DIR, Pins.DIAG);

	return tmc2209_reset(&TMC2209);
}

static uint8_t restore()
{
	return tmc2209_restore(&TMC2209);
}


void TMC2209_INIT()
{
	tmc_fillCRC8Table(0x07, true, 1);

	TMC2209_config.reset = reset;
	TMC2209_config.restore = restore;
	tmc2209_init(&TMC2209, 0, 0, &TMC2209_config, &tmc2209_defaultRegisterResetState[0]);


	//restore();
}

static inline TMC2209TypeDef *motorToIC(uint8_t motor)
{
	UNUSED(motor);

	return &TMC2209;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	setbuf(stdout, NULL);

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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  //int getserial=0;
  TMC2209_INIT();
  printf("Hello\r\n");

  int i;
   char rxbuf[1];
   char start[] ="1:front,2:back\r\n";
   char front[] ="front turn\r\n";
   char back[] ="back turn\r\n";
   char nonans[] ="Not Understand \r\n";


   HAL_UART_Transmit(&huart2,(uint8_t *)start,sizeof(start),3000);

  //printf("Start \r\n");


  //uint8_t rxbuf[1];
   //int32_t *value=3;

   //TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT, 3 );

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //example read register value
	  //int32_t value = 0;
	  HAL_UART_Receive(&huart2,(uint8_t *)rxbuf, sizeof(rxbuf), 100);
	  //tmc2209_writeInt(&TMC2209, TMC2209_TSTEP, 16);

	  /*
	  value = tmc2209_readInt(&TMC2209, TMC2209_FACTORY_CONF);
	  //printf("Hello\r\n");
	  //printf("Goodbye\r\n");
	  //periodicJob(HAL_GetTick());
	  //HAL_UART_Transmit(&huart2,(uint8_t *)value,sizeof(value),3000);
	  printf("FACTORY_CONF: %ld\r\n", value);
	  //HAL_UART_Transmit(&huart2,(uint8_t *)value,sizeof(value),3000);
	  //getserial=value;

	  value = tmc2209_readInt(&TMC2209, TMC2209_SG_RESULT);
	  //periodicJob(HAL_GetTick());
	  //tmc2209_writeInt(&TMC2209, );
	  printf("SG_RESULT: %ld\r\n", value);
	  //tmc2209_writeInt(&TMC2209, TMC2209_TSTEP, 16);
	  value = tmc2209_readInt(&TMC2209, TMC2209_TSTEP);
	  printf("TMC2209_TSTE: %ld\r\n", value);
	  tmc2209_writeInt(&TMC2209, 0x22, 20000);
	  value = tmc2209_readInt(&TMC2209, TMC2209_VACTUAL);
	  printf("TMC2209_VACTUAL: %ld\r\n", value);
	  value = tmc2209_readInt(&TMC2209, TMC2209_CHOPCONF);
	  printf("CHOPCONF: %ld\r\n", value);
	  value = tmc2209_readInt(&TMC2209, TMC2209_IOIN);
	  printf("TMC2209_IOIN: %ld\r\n", value);
	  */


	  	  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_MASK, 0);


	  	  if (rxbuf[0] != 0){
	  		  //HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  			  tmc2209_writeInt(&TMC2209, TMC2209_ENN_MASK, 1);
	  		  HAL_UART_Transmit(&huart2,(uint8_t *)start, sizeof(start),3000);
	  		  //tmc2209_writeInt(motorToIC(motor), TMC2209_TPWMTHRS, *value);
	  		  if(rxbuf[0] == '1'){
	  			  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_MASK, 1);
	  			  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_SHIFT, 1);
	  			  //HAL_UART_Transmit(&huart2,(uint8_t *)front, sizeof(front),3000);
	  			  //HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,SET);
	  			  int32_t value = 0;
	  			  /*value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT);
	  			  printf("TMC2209_FIELD_READ_before : %ld\r\n", value);

	  			  TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT, 3);

	  			  value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT);
	  			  printf("TMC2209_FIELD_READ_after : %ld\r\n", value);*/
	  			  //value = tmc2209_readInt(&TMC2209, TMC2209_CHOPCONF);
	  			  //value=TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT);
	  			  //printf("tmc2209_readInt_before: %ld\r\n", value);

	  			  //tmc2209_writeInt(&TMC2209, TMC2209_CHOPCONF, 0x00);
	  			  //value = 0;
	  			  //TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT, value);
	  			  //tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	  			  //HAL_Delay(200);
	  			  //value=TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT);
	  			  //value = tmc2209_readInt(&TMC2209, TMC2209_CHOPCONF);
	  		  	  //printf("tmc2209_readInt_after: %ld\r\n", value);
	  		  	//tmc2209_writeInt(&TMC2209, TMC2209_ENN_MASK, 0);
	  		  	//tmc2209_writeInt(&TMC2209, TMC2209_ENN_SHIFT, 0);

	  		  	HAL_UART_Transmit(&huart2,(uint8_t *)back, sizeof(back),3000);
	  		  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,SET);
	  			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,RESET);
	  		  	for(i=0;i<=3200;i++){
	  		  		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,SET);
	  		  		HAL_Delay(1);
	  		  		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,RESET);
	  		  	}
	  		  	  rxbuf[0]=0;
	  		  }
	  		  else if(rxbuf[0] == '2'){
		  		  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_MASK, 0);
		  		  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_SHIFT, 0);

	  			  HAL_UART_Transmit(&huart2,(uint8_t *)back, sizeof(back),3000);
	  			  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,RESET);
	  			  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,RESET);
	  			  for(i=0;i<=3200;i++){
	  				  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,SET);
	  				  HAL_Delay(1);
	  				  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,RESET);
	  			  }
	  			  	  //HAL_Delay(100);

	  			  	  rxbuf[0]=0;
	  		  }
	  		  else if(rxbuf[0] == '3'){
	  			  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_MASK, 1);
	  			  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_SHIFT, 1);
	  			  //HAL_UART_Transmit(&huart2,(uint8_t *)front, sizeof(front),3000);
	  			  //HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,SET);
	  			  int32_t value = 0;
	  			  int32_t read_value = 0;
	  			  /*value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT);
	  			  printf("TMC2209_FIELD_READ_before : %ld\r\n", value);

	  			  TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT, 3);

	  			  value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT);
	  			  printf("TMC2209_FIELD_READ_after : %ld\r\n", value);*/
	  			  //value = tmc2209_readInt(&TMC2209, TMC2209_CHOPCONF);
	  			read_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT);
	  			  printf("tmc2209_readInt_before: %ld\r\n", read_value);

	  			  //tmc2209_writeInt(&TMC2209, TMC2209_CHOPCONF, 0x00);
	  			  value = 15;
	  			  TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT, value);
	  			  //tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	  			  HAL_Delay(200);
	  			read_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT);
	  			  //value = tmc2209_readInt(&TMC2209, TMC2209_CHOPCONF);
	  		  	  printf("tmc2209_readInt_after: %ld\r\n", read_value);


	  		  		  rxbuf[0]=0;
	  		  	  }
	  		  else if(rxbuf[0] == '4'){
	  			  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_MASK, 1);
	  			  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_SHIFT, 1);
	  			  //HAL_UART_Transmit(&huart2,(uint8_t *)front, sizeof(front),3000);
	  			  //HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,SET);
	  			  int32_t value = 0;
	  			  /*value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT);
	  			  printf("TMC2209_FIELD_READ_before : %ld\r\n", value);

	  			  TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT, 3);

	  			  value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT);
	  			  printf("TMC2209_FIELD_READ_after : %ld\r\n", value);*/
	  			  //value = tmc2209_readInt(&TMC2209, TMC2209_CHOPCONF);
	  			  value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT);
	  			  printf("tmc2209_readInt_before: %ld\r\n", value);

	  			  //tmc2209_writeInt(&TMC2209, TMC2209_CHOPCONF, 0x00);
	  			  value = 20;
	  			  TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT, value);
	  			  tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	  			  HAL_Delay(200);
	  			  value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT);

	  			  //value = tmc2209_readInt(&TMC2209, TMC2209_CHOPCONF);
	  		  	  printf("tmc2209_readInt_after: %ld\r\n", value);

	  		  		  rxbuf[0]=0;
	  		  	  }
	  		  else if(rxbuf[0] == '5'){
	  			  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_MASK, 1);
	  			  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_SHIFT, 1);
	  			  //HAL_UART_Transmit(&huart2,(uint8_t *)front, sizeof(front),3000);
	  			  //HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,SET);
	  			  int32_t value = 0;
	  			  /*value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT);
	  			  printf("TMC2209_FIELD_READ_before : %ld\r\n", value);

	  			  TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT, 3);

	  			  value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT);
	  			  printf("TMC2209_FIELD_READ_after : %ld\r\n", value);*/
	  			  //value = tmc2209_readInt(&TMC2209, TMC2209_CHOPCONF);

	  			  value=TMC2209_FIELD_READ(&TMC2209, TMC2209_DRVSTATUS, TMC2209_CS_ACTUAL_MASK, TMC2209_CS_ACTUAL_SHIFT);
	  			  printf("tmc2209_current: %ld\r\n", value);
	  			  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,SET);
	  			  rxbuf[0]=0;
	  		  }
	  		  else if(rxbuf[0] == '6'){
	  			  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_MASK, 1);
	  			  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_SHIFT, 1);
	  			  //HAL_UART_Transmit(&huart2,(uint8_t *)front, sizeof(front),3000);
	  			  //HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,SET);
	  			  int32_t value = 0;
	  			  /*value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT);
	  			  printf("TMC2209_FIELD_READ_before : %ld\r\n", value);

	  			  TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT, 3);

	  			  value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT);
	  			  printf("TMC2209_FIELD_READ_after : %ld\r\n", value);*/
	  			  //value = tmc2209_readInt(&TMC2209, TMC2209_CHOPCONF);
	  			  //value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT);
	  			  TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_AUTOSCALE_MASK, TMC2209_PWM_AUTOSCALE_SHIFT, 1);
	  			  value=TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_AUTOSCALE_MASK, TMC2209_PWM_AUTOSCALE_SHIFT);
	  		  	  printf("write pwm auto scale: %ld\r\n", value);

	  		  	  rxbuf[0]=0;
	  		  	  }
	  		  else{
	  			  HAL_UART_Transmit(&huart2,(uint8_t *)nonans, sizeof(nonans),3000);
	  			  rxbuf[0]=0;
	  		  }

	  	  }

	  //HAL_Delay(100);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_HalfDuplex_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart2,(uint8_t *)ptr,len,10);
  return len;
}

//int _write(int file, char *ptr, int len)
//{
//  int DataIdx;
//  for(DataIdx=0; DataIdx<len; DataIdx++)
//  {
//    ITM_SendChar(*ptr++);
//  }
//  return len;
//}

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
