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
CAN_HandleTypeDef hcan2;

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
uint8_t home_position_state =0;
uint8_t get_uart_flag = 0;
uint8_t get_can_flag = 0; //canを受信したかどうかのflag
uint8_t nextcan_flag = 0;
int Writecom=0x00;
int Readcom=0x00;
char RMD_cmd;
float p_getdata=0.0;
float t_position=0.0;
uint16_t p_getintdata=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN2_Init(void);
static void MX_CAN1_Init(void);
/* USER CODE BEGIN PFP */
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//stepper 0 end stop senssor
	if(GPIO_Pin == GPIO_PIN_5 ){
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)){	//end stop ONの時フラグを立てる
			home_position_state |= 1;
		}
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)){
			home_position_state &= ~1;
		}
	}
}

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

//	printf("pack_cmd data\r\n");

//	for(i=0;i<8;i++){
//		printf("data%d : ",i);
//		printf("%x\r\n",data[i]);
//	}
//	printf("Position : %lf\r\n",p_des);
//	printf("Velocity : %lf\r\n",v_des);
//	printf("Kp : %lf\r\n",kp);
//	printf("Kd : %lf\r\n",kd);
//	printf("current : %lf\r\n",t_ff);
//	printf("\r\n");
}
void BLDC_CANTx(){
	TxHeader.StdId=0x01;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 0x08;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
//	HAL_CAN_AddTxMessage(&hcan1,&TxHeader,data,&TxMailbox);
//	HAL_Delay(10);

//	int i = 0;
	int count=0;
	if(0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan2) ){
		HAL_CAN_AddTxMessage(&hcan2,&TxHeader,data,&TxMailbox);
		HAL_Delay(10);
//		printf("Can Send\r\n"3);
		while(!nextcan_flag){
			printf("Mailboxes: %d\r\n",HAL_CAN_GetTxMailboxesFreeLevel(&hcan2));
			HAL_CAN_AddTxMessage(&hcan2,&TxHeader,data,&TxMailbox);
			HAL_Delay(1);
			count++;
			if(count==10){
				nextcan_flag=1;
				get_can_flag=1;
				HAL_CAN_Stop (&hcan2);
				//HAL_CAN_AbortTxRequest(&hcan1,TxMailbox);
				HAL_CAN_Start(&hcan2);
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
}

void unpack_reply(){
	data[0] = RxData[0];
	data[1] = RxData[1];
	data[2] = RxData[2];
	data[3] = RxData[3];
	data[4] = RxData[4];
	data[5] = RxData[5];
	data[6] = RxData[6];
	data[7] = RxData[7];
    /// unpack ints from can buffer ///
    uint16_t id = data[0];
    uint16_t p_int = (data[1]<<8)|data[2];
    uint16_t v_int = (data[3]<<4)|(data[4]>>4);
    uint16_t i_int = ((data[4]&0xF)<<8)|data[5];
    uint16_t RxBitshift[4];

    p_getintdata=p_int;

    RxBitshift[0]=RxData[0];
    RxBitshift[1]=RxData[1]<<8;
   	RxBitshift[1]+=RxData[2];
   	RxBitshift[2]=RxData[3]<<4;
   	RxBitshift[2]+=RxData[4]>>4;
   	RxBitshift[3]=RxData[4]<<8;
   	RxBitshift[3]+=RxData[5];
    /// convert uints to floats ///
    float p = uint_to_float(p_int, P_MIN, P_MAX, 16);
    float v = uint_to_float(v_int, V_MIN, V_MAX, 12);
    float t = uint_to_float(i_int, -T_MAX, T_MAX, 12);
    p_getdata=p;
    //printf(" get data \r\n");
    printf("GetUnpackCanData\r\n");
    printf("ID : %x\r\n",id);
    printf("Position : %lf\r\n",p);
    printf("Velocity : %lf\r\n",v);
    printf("Current : %lf\r\n",t);
	printf("\r\n");

}
void BLDC_MotorRotate(){
	float Kp_change_exp;
	float Kp_change;
	uint8_t id1_end=0;
	pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);	//現在の場所の所得
	BLDC_CANTx();
	HAL_Delay(10);
	unpack_reply();
	Kp_change=fabsf(t_position-p_getdata);	//目標地点ー現在の地点＝移動距離
	Kp_change_exp=0.0;
//	for(float i = -5.0 ;i<=2.0 ;i+=7.0/(20.0 * Kp_change)){
//		Kp_change_exp=100.0*(exp(i)/exp(2.0));
//		pack_cmd(t_position, 0.0, Kp_change_exp, 3.0, 0.0);
//		BLDC_CANTx();
//		HAL_Delay(10);
//	}
	float i = -5.0;
	while(id1_end==0){

			if(i<=2.0){
				i+=(7.0/(20.0 * Kp_change));
				Kp_change_exp=100.0*(exp(i)/exp(2.0));
				pack_cmd(t_position, 0.0, Kp_change_exp, 3.0, 0.0);
				BLDC_CANTx();
				HAL_Delay(10);
			}else{
				id1_end=1;
			}
	}

	if(Kp_change_exp!=100.0){
		pack_cmd(t_position, 0.0, 100.0 , 3.0, 0.0);
		BLDC_CANTx();
		HAL_Delay(10);
		printf("Kp_hosei\r\n");
	}
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
  MX_CAN2_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)!=1){	//リミットスイッチON→待機状態
		  home_position_state=1;
		  printf("home_position_state\r\n");
	  }else{
		  home_position_state=0;
		  printf("Error!\r\n");
	  }
sFilterConfig.FilterBank = 14;
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

if(HAL_CAN_ConfigFilter(&hcan2,&sFilterConfig) != HAL_OK)
{
	Error_Handler();
}
if(HAL_CAN_Start(&hcan2)!=HAL_OK)
{
	Error_Handler();
}

if(HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
{
	Error_Handler();
}

//float t_position;
//float Kp_change;
//float Kp_change_seki;
//float Kp_change_exp;
char KeyCommand[1];
char Mode_Type[4];
uint32_t Mode_selection=0;
uint32_t t_int_position;
uint32_t Init_flag=0;
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
//	  KeyCommand[0]=0;
//	  home_position_state=0;
	  get_uart_flag=0;

	  HAL_UART_Receive_IT(&huart2, (uint8_t *)KeyCommand, 1);
	  HAL_UART_Transmit(&huart2, (uint8_t *)KeyCommand, 1,300);
	  //受信するまで待つ
	  while(!get_uart_flag){}

	  get_can_flag=1;//canデータ受信用のフラグを0に
	  if(KeyCommand[0]!=0 && home_position_state==1){
		  switch(KeyCommand[0]){
		  case'i':
			  printf("case : i\r\n");
			  Mode_Type[0]=0xFF;
			  Mode_Type[1]=0xFF;
			  Mode_Type[2]=0xFF;
			  Mode_Type[3]=0x0E;
//			  printf("Mode_selection　%x\r\n",Mode_selection);
			  Mode_selection=Mode_Type[0]<<24|Mode_Type[1]<<16|Mode_Type[2]<<8|Mode_Type[3];
			  Mode_selection=Mode_selection&0x000000FF;
			  printf("Mode_selection　%x\r\n",Mode_selection);
			  HAL_Delay(10);
			  KeyCommand[0]=0;
			  break;
		  default:
			  printf("NoCommand:%x\r\n",KeyCommand[0]);
			  KeyCommand[0]=0;
			  get_can_flag=1;
			  break;
		  }
		  get_uart_flag=0;
	  }

	  if(KeyCommand[0]!=0 && home_position_state==0){
		  switch(KeyCommand[0]){
		  //void pack_cmd(float _p_des, float _v_des, float _kp, float _kd, float _t_ff)
		  case'm':
			  printf("case : m\r\n");
			  Mode_Type[0]=0xFF;
			  Mode_Type[1]=0xFF;
			  Mode_Type[2]=0xFF;
			  Mode_Type[3]=0x0A;
//			  printf("Mode_selection　%x\r\n",Mode_selection);
			  Mode_selection=Mode_Type[0]<<24|Mode_Type[1]<<16|Mode_Type[2]<<8|Mode_Type[3];
			  Mode_selection=Mode_selection&0x000000FF;
//			  printf("Mode_selection　%x\r\n",Mode_selection);
			  HAL_Delay(10);
			  KeyCommand[0]=0;
			  break;
		  case't':
			  printf("case : t\r\n");
			  Mode_Type[0]=0xFF;
			  Mode_Type[1]=0xFF;
			  Mode_Type[2]=0xFF;
			  Mode_Type[3]=0x0B;
			  Mode_selection=Mode_Type[0]<<24|Mode_Type[1]<<16|Mode_Type[2]<<8|Mode_Type[3];
			  Mode_selection=Mode_selection&0x000000FF;
			  HAL_Delay(10);
			  KeyCommand[0]=0;
			  break;
		  case'e':
			  printf("case : e\r\n");
			  Mode_Type[0]=0xFF;
			  Mode_Type[1]=0xFF;
			  Mode_Type[2]=0xFF;
			  Mode_Type[3]=0x0C;
			  Mode_selection=Mode_Type[0]<<24|Mode_Type[1]<<16|Mode_Type[2]<<8|Mode_Type[3];
			  Mode_selection=Mode_selection&0x000000FF;
			  HAL_Delay(10);
			  KeyCommand[0]=0;
			  break;
		  default:
			  printf("NoCommand\r\n");
			  KeyCommand[0]=0;
			  get_can_flag=1;
			  break;
		  }
		  get_uart_flag=0;
	  }
		  switch(Mode_selection){
		  case 0x0E:
			  printf("Mode0x0E\r\n");	//初期位置モード
			  EnterMotorMode();
			  BLDC_CANTx();
			  HAL_Delay(10);
			  t_position=PI;
			  BLDC_MotorRotate();
			  KeyCommand[0]=0;
			  break;
		  case 0x0A:
			  printf("Mode0x0A\r\n");	//動作モード
			  t_position = uint_to_float(t_int_position, P_MIN, P_MAX, 16);
			  BLDC_MotorRotate();
			  KeyCommand[0]=0;
			  break;
		  case 0x0B:
			  printf("Mode0x0B\r\n");	//ティーチングモード
			  pack_cmd(0.0, 0.0, 0.0, 1.0, 0.0);
			  BLDC_CANTx();
			  HAL_Delay(10);
			  HAL_UART_Receive_IT(&huart2, (uint8_t *)KeyCommand, 1);
			  HAL_UART_Transmit(&huart2, (uint8_t *)KeyCommand, 1,300);
			  while(!get_uart_flag){}
			  if(KeyCommand[0]==0x0d){
				  pack_cmd(0.0, 0.0, 0.0, 1.0, 0.0);
				  BLDC_CANTx();
				  unpack_reply();
				  pack_cmd(p_getdata, 0.0, 100.0, 3.0, 0.0);
				  BLDC_CANTx();
				  unpack_reply();
				  t_int_position=p_getintdata;
				  printf("T_Success!\r\n");
			  }
			  else{
				  printf("No_Success!\r\n");
			  }
			  KeyCommand[0]=0;
			  break;
		  case 0x0C:	//待機位置に戻す
			  printf("Mode0x0C\r\n");
			  t_position=0;
			  BLDC_MotorRotate();
			  KeyCommand[0]=0;
			  break;
		  default:
			  printf("NoCommand\r\n");
			  KeyCommand[0]=0;
			  get_can_flag=1;
			  break;
		  }
		  get_uart_flag=0;
		  while(!get_can_flag){}

  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


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
  * @brief CAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 4;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_7TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */

  /* USER CODE END CAN2_Init 2 */

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

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

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
	  printf("TxCan\r\n");
	  nextcan_flag=1;

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	get_can_flag=1;
//	printf("RxCan\r\n");
	HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0, &RxHeader,RxData);
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
