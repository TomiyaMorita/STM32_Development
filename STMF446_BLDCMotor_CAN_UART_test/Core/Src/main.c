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
#include "math_ops.h"
#include "leg_message.h"
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
uint8_t data[8];
uint32_t TxMailbox;
uint8_t cmd_data = 0x00;
uint8_t UART1_Data[8];
uint8_t get_uart_flag = 0;
uint8_t get_can_flag = 0; //canを受信したかどうかのflag
uint8_t nextcan_flag = 0;
int Writecom=0x00;
int Readcom=0x00;
char RMD_cmd;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);

/// Value Limits ///
 #define P_MIN -12.5f
 #define P_MAX 12.5f
 #define V_MIN -65.0f
 #define V_MAX 65.0f
 #define KP_MIN 0.0f
 #define KP_MAX 500.0f
 #define KD_MIN 0.0f
 #define KD_MAX 5.0f
 #define T_MIN -18.0f
 #define T_MAX 18.0f

 /// Joint Soft Stops ///
 #define A_LIM_P 1.5f
 #define A_LIM_N -1.5f
 #define H_LIM_P 5.0f
 #define H_LIM_N -5.0f
 #define K_LIM_P 0.2f
 #define K_LIM_N 7.7f
 #define KP_SOFTSTOP 100.0f
 #define KD_SOFTSTOP 0.4f;

#define ENABLE_CMD 0xFFFF
#define DISABLE_CMD 0x1F1F

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t uart_data;
char tx_data[]="get_data\r\n";
int flag = 1;
typedef struct {
	uint16_t rxMsg;
	uint16_t txMsg;
}CANMessage;

void Zero(){
	data[0] = 0xFF;
    data[1] = 0xFF;
    data[2] = 0xFF;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;
    data[6] = 0xFF;
    data[7] = 0xFE;
    //WriteAll();
    }

void EnterMotorMode(){
	data[0] = 0xFF;
    data[1] = 0xFF;
    data[2] = 0xFF;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;
    data[6] = 0xFF;
    data[7] = 0xFC;
    //WriteAll();
    }

void ExitMotorMode(){
	data[0] = 0xFF;
	data[1] = 0xFF;
	data[2] = 0xFF;
	data[3] = 0xFF;
	data[4] = 0xFF;
	data[5] = 0xFF;
	data[6] = 0xFF;
	data[7] = 0xFD;
    //WriteAll();
    }


void pack_cmd(float _p_des, float _v_des, float _kp, float _kd, float _t_ff){
	/// limit data to be within bounds ///
	float p_des = fminf(fmaxf(P_MIN, _p_des), P_MAX);
	float v_des = fminf(fmaxf(V_MIN, _v_des), V_MAX);
	float kp = fminf(fmaxf(KP_MIN, _kp), KP_MAX);
	float kd = fminf(fmaxf(KD_MIN, _kd), KD_MAX);
	float t_ff = fminf(fmaxf(T_MIN, _t_ff), T_MAX);
	/// convert floats to unsigned ints ///
	uint16_t p_int = float_to_uint(p_des, P_MIN, P_MAX, 16);
	uint16_t v_int = float_to_uint(v_des, V_MIN, V_MAX, 12);
	uint16_t kp_int = float_to_uint(kp, KP_MIN, KP_MAX, 12);
	uint16_t kd_int = float_to_uint(kd, KD_MIN, KD_MAX, 12);
	uint16_t t_int = float_to_uint(t_ff, T_MIN, T_MAX, 12);
	/// pack ints into the can buffer ///
	data[0] = p_int>>8;
	data[1] = p_int&0xFF;
	data[2] = v_int>>4;
	data[3] = ((v_int&0xF)<<4)|(kp_int>>8);
	data[4] = kp_int&0xFF;
	data[5] = kd_int>>4;
	data[6] = ((kd_int&0xF)<<4)|(t_int>>8);
	data[7] = t_int&0xFF;

	int i=0;
	printf("pack_cmd data\r\n");
	for(i=0;i<8;i++){
		printf("data%d : ",i);
		printf("%x\r\n",data[i]);
	}
	printf("\r\n");
}
void BLDC_CANTx(){
	TxHeader.StdId=0x1;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 0x08;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
//	HAL_CAN_AddTxMessage(&hcan1,&TxHeader,data,&TxMailbox);
//	HAL_Delay(10);

	int i = 0;
	int count=0;
	if(0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) ){
		HAL_CAN_AddTxMessage(&hcan1,&TxHeader,data,&TxMailbox);
		HAL_Delay(1);
//		printf("Can Send\r\n");
		while(!nextcan_flag){
			printf("Mailboxes: %d\r\n",HAL_CAN_GetTxMailboxesFreeLevel(&hcan1));
			HAL_CAN_AddTxMessage(&hcan1,&TxHeader,data,&TxMailbox);
			HAL_Delay(1);
			count++;
			if(count==10){
				nextcan_flag=1;
				HAL_CAN_Stop (&hcan1);
				//HAL_CAN_AbortTxRequest(&hcan1,TxMailbox);
				HAL_CAN_Start(&hcan1);
				printf("CAN RESET\r\n");
			}
		}
		nextcan_flag=0;
		count=0;
	}else{
		printf("Mailbox_congestion\r\n");
		nextcan_flag=0;
		get_can_flag=1;
	}
	HAL_Delay(10);
	printf("Send Command Data: 0x%x\r\n",data[0]);
	for(i=0;i < 8;i++){
		printf("data[%d] :",i);
		printf("0x%x\r\n",data[i]);
	}
	printf("\r\n");
}

void unpack_reply(){
    /// unpack ints from can buffer ///
    uint16_t id = data[0];
    uint16_t p_int = (data[1]<<8)|data[2];
    uint16_t v_int = (data[3]<<4)|(data[4]>>4);
    uint16_t i_int = ((data[4]&0xF)<<8)|data[5];
    /// convert uints to floats ///
    float p = uint_to_float(p_int, P_MIN, P_MAX, 16);
    float v = uint_to_float(v_int, V_MIN, V_MAX, 12);
    float t = uint_to_float(i_int, -T_MAX, T_MAX, 12);
    //printf(" get data \r\n");
    printf("GetUnpackCanData\r\n");
    printf("ID : %x\r\n",id);
    printf("Position : %lf\r\n",p);
    printf("Velocity : %lf\r\n",v);
    printf("Current : %lf\r\n",t);
	printf("\r\n");

}


void BLDC_CANRx(){
	uint16_t RxBitshift[4];

	RxBitshift[0]=RxData[0];
	RxBitshift[1]=RxData[1]<<8;
	RxBitshift[1]+=RxData[2];
	RxBitshift[2]=RxData[3]<<4;
	RxBitshift[2]+=RxData[4]>>4;
	RxBitshift[3]=RxData[4]<<8;
	RxBitshift[3]+=RxData[5];
	printf("GetCanData\r\n");
	printf("ID : 0x%x\r\n",RxBitshift[0]);
	printf("Position : 0x%x\r\n",RxBitshift[1]);
	printf("Velocity : 0x%x\r\n",RxBitshift[2]);
	printf("Current : 0x%x\r\n",RxBitshift[3]);
	printf("\r\n");
	data[0] = RxData[0];
	data[1] = RxData[1];
	data[2] = RxData[2];
	data[3] = RxData[3];
	data[4] = RxData[4];
	data[5] = RxData[5];
	data[6] = RxData[6];
	data[7] = RxData[7];
	unpack_reply();

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
//int RxBitshift[4];
//int TxBitshift[4];
//int current=0;
//int current_range=0;
printf("BLDC Start\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  get_uart_flag=0;
	  HAL_UART_Receive_IT(&huart2, (uint8_t *)KeyCommand, 1);
	  HAL_UART_Transmit(&huart2, (uint8_t *)KeyCommand, 1,300);
	  //受信するまで待つ
	  while(!get_uart_flag){}
	  get_can_flag=0;
	  //canデータ受信用のフラグを0に
	  if(KeyCommand[0]!=0 ){
		  switch(KeyCommand[0]){
		  //void pack_cmd(float _p_des, float _v_des, float _kp, float _kd, float _t_ff)
		  case '1':
			  printf("case : 1\r\n");
			  pack_cmd(0.0, 1.0, 5.0, 3.0, 0.0);
			  BLDC_CANTx();
			  HAL_Delay(10);
			  BLDC_CANRx();
			  KeyCommand[0]=0;
			  break;
		  case'2':
			  printf("case : 2\r\n");
			  pack_cmd(2.0*PI, 1.0, 5.0, 3.0, 0.0);
			  BLDC_CANTx();
			  HAL_Delay(10);
			  BLDC_CANRx();
			  KeyCommand[0]=0;
			  break;
		  case'3':
			  printf("case : 3\r\n");
			  pack_cmd(2.0*PI, 1.0, 5.0, 3.0, 4.0);
			  BLDC_CANTx();
			  HAL_Delay(10);
			  BLDC_CANRx();
			  KeyCommand[0]=0;
			  break;
		  case'm':
			  printf("case : m\r\n");
			  EnterMotorMode();
			  BLDC_CANTx();
			  HAL_Delay(10);
			  KeyCommand[0]=0;

			  break;
		  case 27:
			  printf("case : esc\r\n");
			  ExitMotorMode();
			  BLDC_CANTx();
			  HAL_Delay(10);
			  KeyCommand[0]=0;

			  break;
		  case'z':
			  printf("case : z\r\n");
			  Zero();
			  BLDC_CANTx();
			  HAL_Delay(10);
			  KeyCommand[0]=0;
		  case'a':
			  printf("case : a\r\n");
			  pack_cmd(-PI/2, 1.0, 5.0, 4.0, 0.0);
			  BLDC_CANTx();
			  HAL_Delay(10);
			  KeyCommand[0]=0;
			  break;
		  case'b':
			  printf("case : a\r\n");
			  pack_cmd(PI/2, 1.0, 5.0, 4.0, 0.0);
			  BLDC_CANTx();
			  HAL_Delay(10);
			  KeyCommand[0]=0;
			  break;
		  case'c':
			  printf("case : c\r\n");
			  pack_cmd(0.0, 1.0, 5.0, 4.0, 0.0);
			  BLDC_CANTx();
			  HAL_Delay(10);
			  KeyCommand[0]=0;
			  break;

		  default:
			  printf("NoCommand\r\n");
			  KeyCommand[0]=0;
			  get_can_flag=1;
			  break;
		  }
		  while(!get_can_flag){}

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
//	printf("UART_START\r\n");
//if(get_can_flag==0){
	get_uart_flag=1;
//	printf("UART_GET\r\n");
//}

}
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
//	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1);
//	  printf("TxCan\r\n");
	  nextcan_flag=1;

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan_)
{
	get_can_flag=1;
	printf("RxCan\r\n");
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0, &RxHeader,RxData);
	HAL_UART_Transmit(&huart2,RxData,sizeof(RxData),100);//受け取ったデータを送信

	//printf("\r\n");

//HAL_UART_Transmit(&huart2,&RxHeader,4);
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
