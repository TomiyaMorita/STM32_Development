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
#include <math.h>
#include<string.h>
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
uint8_t canRxdata[8];
uint8_t RMDRx_can_data[8];
uint8_t RMDTx_can_data[8];
uint32_t TxMailbox;
uint8_t KeyCommand[1];
uint8_t uart_data;
uint8_t get_uart_flag=0;
uint8_t RMD_data[8];
uint8_t nextcan_flag;


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


void RMDCommand(uint8_t com){
	uint16_t canget_data[8];
	memset(RMDTx_can_data,0,sizeof(RMDTx_can_data));
	switch(com){
		case 0x31:
			RMDTx_can_data[0] = 0x31;	//PI設定
			break;
		case 0x32:
			RMDTx_can_data[0] = 0x32;	//PI設定
			RMDTx_can_data[2] = 0xC8;	//anglePidKp
			RMDTx_can_data[3] = 0x64;	//anglePidKi
			RMDTx_can_data[4] = 0x32;	//speedPidKp
			RMDTx_can_data[5] = 0x14;	//speedPidKi
			RMDTx_can_data[6] = 0x32;	//iqPidKp
			RMDTx_can_data[7] = 0x14;	//iqPidKi
			break;
		case 0x34:
			RMDTx_can_data[0] = 0x34;	//accelePI設定
			break;
		case 0x91:
			RMDTx_can_data[0] = 0x91;
			RMDTx_can_data[6] = RMD_data[6];
			RMDTx_can_data[7] = RMD_data[7];
		case 0xA1:
			RMDTx_can_data[0] = 0xA1;	//Torque設定
		case 0xA4:
			RMDTx_can_data[0] = 0xA4;
			RMDTx_can_data[2] = 0x2C;	//speed low
			RMDTx_can_data[3] = 0x01;	//speed high
			RMDTx_can_data[4] =RMD_data[4];
			RMDTx_can_data[5] =RMD_data[5];
			break;
		case 0xA7:
			RMDTx_can_data[0] = 0xA7;
			RMDTx_can_data[2] = 0x2C;	//speed low
			RMDTx_can_data[3] = 0x01;	//speed high
			RMDTx_can_data[4] =RMD_data[4];
			RMDTx_can_data[5] =RMD_data[5];
			RMDTx_can_data[6] =	0x00;
			RMDTx_can_data[7] = 0x00;
			break;
		default:
			break;
		}
	TxHeader.StdId=0x141;
	TxHeader.ExtId = 0x00;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 0x08;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	nextcan_flag=0;
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)<3){}
	HAL_CAN_AddTxMessage(&hcan1,&TxHeader,RMDTx_can_data,&TxMailbox);
	HAL_Delay(10);
	printf("HAL_CAN_AddTxMessager\r\n");
	switch(com){
		case 0x31:
			canget_data[0] = RMDRx_can_data[0];	//PI設定
			break;
		case 0x32:
			canget_data[0] = RMDRx_can_data[0];	//PI設定
			canget_data[1] = RMDRx_can_data[1];	//anglePidKp
			canget_data[2] = RMDRx_can_data[2];	//anglePidKi
			canget_data[3] = RMDRx_can_data[3];	//speedPidKp
			canget_data[4] = RMDRx_can_data[4];	//speedPidKi
			canget_data[5] = RMDRx_can_data[5];	//iqPidKp
			canget_data[6] = RMDRx_can_data[6];	//iqPidKi
			break;
		case 0x34:
			canget_data[0] = RMDRx_can_data[0];	//accelePI設定
			break;
		case 0x91:
			canget_data[0] = RMDRx_can_data[0];
			canget_data[1] = RMD_data[6];
			canget_data[2] = RMD_data[7];
		case 0xA1:
			canget_data[0] = RMDRx_can_data[0];	//Torque設定
		case 0xA4:
			canget_data[0] = RMDRx_can_data[0];
			canget_data[1] = RMDRx_can_data[1];
			canget_data[2] = (RMDRx_can_data[2]) | ((RMDRx_can_data[3])<<8);
			canget_data[3] = (RMDRx_can_data[4]) | ((RMDRx_can_data[5])<<8);
			canget_data[4] = (RMDRx_can_data[6]) | ((RMDRx_can_data[7])<<8);
			printf("temp_value : %d\r\n",canget_data[1]);
			printf("torque_value : %d\r\n",canget_data[2]);
			printf("speed_value : %d\r\n",canget_data[3]);
			printf("encoder_value : %d\r\n",canget_data[4]);
			break;
		case 0xA7:
			canget_data[0] = RMDRx_can_data[0];
			canget_data[1] =(RMDRx_can_data[6]&0xFF) | ((RMDRx_can_data[7]&0xFF)<<8);
			printf("encoder_value : %d\r\n",canget_data[1]);
			break;
		default:
			break;
		}

	com=0;
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
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  uint8_t com;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
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
  HAL_UART_Receive_IT(&huart2, (uint8_t *)KeyCommand, 1);
  printf("InIt end\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  get_uart_flag=0;
	  //受信するまで待つ
	  printf("waiting...\r\n");
	  while(!get_uart_flag){}
	  if(KeyCommand[0]!=0 ){
		  switch(KeyCommand[0]){
		  case '0':
			  printf("case:0\r\n");
			  com=0x32;
			  RMDCommand(com);
			  KeyCommand[0]=0;
			  break;
		  case'1':
			  printf("case:1\r\n");
			  com=0xA4;
			  RMD_data[4] = 0x50;	//poslow
			  RMD_data[5] = 0x46;
			  RMDCommand(com);
			  KeyCommand[0]=0;
			  break;
		  case'2':
			  printf("case:2\r\n");
			  com=0xA4;
			  RMD_data[4] = 0x00;	//poslow
			  RMD_data[5] = 0x00;
			  RMDCommand(com);
			  KeyCommand[0]=0;
			  break;
		  case'3':
			  printf("case:3\r\n");
			  com=0xA7;
			  RMD_data[4] = 0x50;	//poslow
			  RMD_data[5] = 0x46;
			  RMDCommand(com);
			  KeyCommand[0]=0;
			  break;
		  case'4':
			  printf("case:4\r\n");
			  com=0xA7;
			  RMD_data[4] = 0xE8;	//poslow
			  RMD_data[5] = 0xE9;
			  RMDCommand(com);
			  KeyCommand[0]=0;
			  break;
		  default:
			  KeyCommand[0]=0;
			  printf("No,command\r\n");
			  break;
		  }
	  }
//	  while(!nextcan_flag){}

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
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_7TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
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

//	HAL_UART_Transmit(&huart2,UART1_Data,1,1000);	//1byte受け取ったら次へ
	get_uart_flag=1;
	HAL_UART_Receive_IT(&huart2, (uint8_t *)KeyCommand, 1);

}
void HAL_CAN_TxMailbox0CompleteCallack(CAN_HandleTypeDef *hcan)
{
//	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan_)
{
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0, &RxHeader, canRxdata);
	if(RxHeader.StdId==0x141){
		memcpy(RMDRx_can_data, canRxdata, 8);
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
  printf("error\r\n");
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
