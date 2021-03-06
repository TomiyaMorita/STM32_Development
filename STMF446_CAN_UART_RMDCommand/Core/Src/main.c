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
#include<stdio.h>
#include<stdlib.h>
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
 CAN_HandleTypeDef hcan1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
CAN_FilterTypeDef sFilterConfig;
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint8_t TxData[8];
uint8_t RxData[8];
uint32_t TxMailbox;
uint8_t cmd_data = 0x00;
uint8_t UART1_Data[8];
uint8_t get_uart_flag = 0;
uint8_t get_can_flag = 0; //canを受信したかどうかのflag
char RMD_cmd;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t uart_data;
char tx_data[]="get_data\r\n";
int flag = 1;

void RMDCommand(){
	get_can_flag=1;
	TxHeader.StdId=0x141;
	//TxHeader.ExtId=0x01;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 0x08;
	TxHeader.TransmitGlobalTime = DISABLE;

switch(UART1_Data[0]){
case 0xA4:

	TxData[0] = 0xA4;
	TxData[1] = UART1_Data[1];
	TxData[2] = 0x64;
	TxData[3] = 0x00;
	TxData[4] = UART1_Data[4];
	TxData[5] = UART1_Data[5];
//	TxData[4] = 0x50;
//	TxData[5] = 0x46;
	TxData[6] = 0x00;
	TxData[7] = 0x00;
	break;
case 0x90:
	TxData[0] = 0x90;
	TxData[1] = RxData[1];
	TxData[2] = RxData[2];
	TxData[3] = RxData[3];
	TxData[4] = RxData[4];
	TxData[5] = RxData[5];
	TxData[6] = RxData[6];
	TxData[7] = RxData[7];
	break;
default:
	break;
}

}
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
  MX_USART2_UART_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */

sFilterConfig.FilterBank = 0;
sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
sFilterConfig.FilterIdHigh = 0x0000;
sFilterConfig.FilterIdLow = 0x0000;
sFilterConfig.FilterMaskIdHigh = 0x0000;
sFilterConfig.FilterMaskIdLow = 0x0000;
sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
//	sFilterConfig.FilterFIFOAssignment =CAN_FILTER_FIFO0;
sFilterConfig.FilterActivation=ENABLE;
sFilterConfig.SlaveStartFilterBank=14;

if(HAL_CAN_ConfigFilter(&hcan1,&sFilterConfig) != HAL_OK)
{
	Error_Handler();
}
if(HAL_CAN_Start(&hcan1)!=HAL_OK)
{
	Error_Handler();
}

if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
{
	Error_Handler();
}
//	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
//	{
//	   Error_Handler ();
//	}
//	HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
//	HAL_CAN_Start(&hcan1);
//	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);


//if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
//	  {
//	     Error_Handler ();
//	  }

char KeyCommand[1];
KeyCommand[0]=0;
//int KeyChange;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //受信割り込みの開始=6byte受け取ったら次の処理へ
	  get_uart_flag=0;
	  get_can_flag=0;
	  int i=0;
	  HAL_UART_Receive_IT(&huart2, (uint8_t *)KeyCommand, 1);
//	  printf("Key %d\r\n",KeyChange);
	  HAL_UART_Transmit_IT(&huart2, (uint8_t *)KeyCommand, 1);
	  //受信するまで待つ
	  while(!get_uart_flag){}


  	   //canデータ受信用のフラグを0に
//	  printf("beforecase\r\n");
	  if(0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)){
		  if(KeyCommand[0]!=0 ){
			  switch(KeyCommand[0]){
			  case '1':
				  UART1_Data[0] = 0xA4;
				  UART1_Data[4] = 0x00;
				  UART1_Data[5] = 0x00;
				  printf("case : 1\r\n");
				  KeyCommand[0]=0;
				  break;
			  case'2':
				  UART1_Data[0] = 0xA4;
				  UART1_Data[4] = 0x50;
				  UART1_Data[5] = 0x46;
				  printf("case : 2\r\n");
				  KeyCommand[0]=0;
				  break;
			  case'3':
				  UART1_Data[0] = 0x90;
				  KeyCommand[0]=0;
			  default:
				  KeyCommand[0]=0;
				  break;
			  }
			  RMDCommand();
		  }else{
			  printf("Mailbox congestion\r\n");
		  }
		  HAL_CAN_AddTxMessage(&hcan1,&TxHeader,TxData,&TxMailbox);
		  printf("Command : 0x%x\r\n",TxData[0]);
		  for(i=0;i < 8;i++){
			  printf("DATA[%d]",i);
			  printf("0x%x\r\n",TxData[i]);
		  }
		  while(!get_can_flag){}
	  }

//	  if(KeyCommand[2]=='2'){
//	  	  		  printf("2\r\n");
//	  	  	  }
//	  if(KeyCommand[3]=='3'){
//	  	  	  		  printf("3\r\n");
//	  	  	  	  }
//	  else{
//		  printf("Nocomand\r\n");
//	  }
//	  scanf("%s",&KeyCommand);
//	  printf("%s",KeyCommand);

//	  RMDCommand();

	 // printf("UARTdata:%s\r\n",KeyCommand[0]);


//	  if(get_can_flag)


//	  HAL_Delay(100);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 4;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_7TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

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
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

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

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)	//printfに必要
{
  HAL_UART_Transmit(&huart2,(uint8_t *)ptr,len,10);
  return len;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)
{
//	HAL_UART_Transmit(&huart2,UART1_Data,6,1000);
	get_uart_flag=1;

}
void HAL_CAN_TxMailbox0CompleteCallack(CAN_HandleTypeDef *hcan)
{
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1);
	  printf("Tx_can\r\n");

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan_)
{
	printf("Rx_can\r\n");
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0, &RxHeader,RxData);
	HAL_UART_Transmit(&huart2,RxData,sizeof(RxData),1000);//受け取ったデータを送信
	printf("\r\n");

//HAL_UART_Transmit(&huart2,&RxHeader,4);
  get_can_flag=1;
//  HAL_UART_Transmit(&huart2,RxData,1,3000);
//  flag *= -1;
//  if(flag >0){
//	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1);
//  }else{
//	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,0);
//  }
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
