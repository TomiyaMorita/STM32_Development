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
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
float acceleration;
volatile unsigned int minStepInterval;
void (*dirFunc)(int);
void (*stepFunc)();

uint8_t homing;
unsigned int c0;
volatile int stepPosition;

volatile int dir; // 1 or -1
volatile int dir_inv; //set 1 to reverse the dir

volatile int pull_off; //set pull_off distance

volatile int seeking_vel;
volatile int homing_vel;

volatile unsigned int totalSteps;

volatile char movementDone;
volatile unsigned int rampUpStepCount;
volatile unsigned int n; // index
volatile float d; // current interval length
volatile unsigned long di; // d -> unsigned  di
volatile unsigned int stepCount;

}stepperInfo;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
 volatile stepperInfo stepper[1];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Step0(){                                       //step pin
	//pull+

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
}

void Dir0(int dir){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, dir); //Dir0関数にdirピンを指定して格納
}

uint8_t remainingstepperFlag = 0;
uint8_t stopflag = 0;
void motorrotate(int absolute_steps){
	volatile stepperInfo* si = &stepper[0];	//stepper[0]のアドレスをstepperInfo* si に代入
	int steps = absolute_steps - si->stepPosition;
//	printf("steps:%d\r\n",steps);
	if(steps == 0){
					return;
				}
	si->dirFunc( steps < 0 ? 1 : 0);	//Dir0のプラスマイナスの判定して0より小さいなら1、大きいなら0をHAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, dir)のdirにいれる
	si->dir = steps > 0 ? 1:-1;			//stepsが0より大きければ1、小さければ２をdirにいれる
	si->totalSteps = abs(steps);	//stepsの値をアブソリュートでtotalstepsに格納
	si->di = si->acceleration;      //変数*siがメンバc0にアクセス.した値に変数*siがメンバaccelerationにアクセス.した値を代入する
	si->stepCount = 0;
	si->n = 0;
//	printf("steps:%d\r\n",steps);
	remainingstepperFlag |= 1;
}
volatile uint8_t nextStepperFlag = 0;
void setNextInterruptInterval(){
	unsigned int mind = 999999;
	if( (remainingstepperFlag) && stepper[0].di < mind ){
		mind = stepper[0].di;		//diに格納されているの(acceleration)を読み込む
	}
	nextStepperFlag = 0;
	if( 1 && stepper[0].di == mind ){
		nextStepperFlag |= 1;
	}
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, mind);		//mindにコンペアを設定
	if (remainingstepperFlag == 0){
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65500);
	}
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, mind);
	//printf("nownompare:%d\r\n",mind);
}
void rotationalprocess(){
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65500);
//	int32_t current_value = 0;
//	current_value=tmc2209_readInt(&TMC2209, TMC2209_SG_RESULT);	//保持トルクの設定
//	printf("SG_RESULT : %ld\r\n", current_value);
	volatile stepperInfo* s = &stepper[0];
		if( (s->stepCount) < (s->totalSteps) ){		//1周ごとにカウントしたstepCountがabs(steps)より小さかったら==動作が終了するまで
				s->stepFunc();							//stepをOFF ONとする
				s->stepCount++;							//カウント
				s->stepPosition += s->dir;		//dir(+もしくはー(dirの向き）を加算してstepPositionに格納
				if ( (s->stepCount) >= (s->totalSteps) ){	//stepCountがtotalStepsを超えたとき、remainingstepperFlagを
					remainingstepperFlag &= ~1;
				}
		}
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){      //指定したコンペアを超えた時発火
	//HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_1);

		rotationalprocess();
		setNextInterruptInterval();
		__HAL_TIM_SET_COUNTER(&htim3, 0);			//カウンター0

}

void runAndWait(){
	setNextInterruptInterval();
//	currentcheck();
	while(remainingstepperFlag);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char rxbuf[1];
	char nonans[] ="Not Understand \r\n";

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  stepper[0].dirFunc = Dir0;
  stepper[0].stepFunc = Step0;
  stepper[0].acceleration = 100-1;
  stepper[0].minStepInterval = 100;
  stepper[0].homing = 0;
  stepper[0].dir_inv = 1;
  stepper[0].seeking_vel = 100;
  stepper[0].homing_vel = 200;
  stepper[0].pull_off = 100;
  rxbuf[0]=0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  HAL_UART_Receive_IT(&huart2,(uint8_t *)rxbuf, 1);
//	  HAL_UART_Transmit(&huart2,(uint8_t *)rxbuf,sizeof(rxbuf),3000);
//	  		  if (rxbuf[0] != 0){
//	  			  if(rxbuf[0] == '1'){
//	  				  //HAL_UART_Transmit_IT(&huart2,(uint8_t *)front, sizeof(front));
//	  				  printf("1:start\r\n");
	  				  motorrotate(5000);
	  				  runAndWait();
//	  				  motorrotate(0);
//	  				  runAndWait();
//	  				  HAL_Delay(100);
//	  				  printf("1:end\r\n");
//	  				  rxbuf[0]=0;

//	  			  }
//	  			  else{
//	  				  HAL_UART_Transmit_IT(&huart2,(uint8_t *)nonans, sizeof(nonans));
//	  				  rxbuf[0]=0;
//	  			  }
//	  		  }
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 840-1;
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
  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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
