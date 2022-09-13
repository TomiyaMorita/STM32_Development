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
uint8_t TxData[20];
uint8_t RxData[20];
uint8_t data[20];
uint32_t TxMailbox;
uint32_t canid;
uint8_t cmd_data = 0x00;
uint8_t home_position_state =0;
uint8_t get_uart_flag = 0;
uint8_t get_can_flag = 0; //canを受信したかどうかのflag
uint8_t nextcan_flag = 0;
uint8_t selectmode_flag = 0;
int BLDCm1_dir=1;
int BLDCm2_dir=-1;
int Writecom=0x00;
int Readcom=0x00;
char RMD_cmd;
float p_getdata=0.0;
float p_getdata_s1=0.0;
float p_getdata_s2=0.0;
float t_position[2];
uint16_t p_getintdata=0;
float l1=250.0;
float l2=350.0;

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
uint8_t buffersize=20;


typedef struct {
	uint16_t rxMsg;
	uint16_t txMsg;
}CANMessage;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//stepper 0 end stop senssor
	if(GPIO_Pin == GPIO_PIN_6 ){
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){	//end stop ONの時フラグを立てる
			home_position_state |= 1;
		}
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){
			home_position_state &= ~1;
		}
	}
}

void BLDC_FK(float s1,float s2,float* xp,float* yp);
void BLDC_IK(float x,float y,float* s1p,float* s2p);
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
}
void BLDC_CANTx(){
	TxHeader.StdId=canid;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 0x08;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
//	HAL_CAN_AddTxMessage(&hcan1,&TxHeader,data,&TxMailbox);
//	HAL_Delay(10);

//	int i = 0;
	int count=0;
	if(0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) ){
		HAL_CAN_AddTxMessage(&hcan1,&TxHeader,data,&TxMailbox);
		HAL_Delay(10);
		while(!nextcan_flag){
			HAL_CAN_AddTxMessage(&hcan1,&TxHeader,data,&TxMailbox);
			HAL_Delay(10);
			count++;
			if(count==10){
				nextcan_flag=1;
				get_can_flag=1;
				HAL_CAN_Stop (&hcan1);
				//HAL_CAN_AbortTxRequest(&hcan1,TxMailbox);
				HAL_CAN_Start(&hcan1);
			}
		}
		nextcan_flag=0;
		count=0;
	}else{
		nextcan_flag=0;
		get_can_flag=1;
	}
}

void unpack_reply(){
    /// unpack ints from can buffer ///
    uint16_t id = data[0];
    uint16_t p_int = (data[1]<<8)|data[2];
    uint16_t v_int = (data[3]<<4)|(data[4]>>4);
    uint16_t i_int = ((data[4]&0xF)<<8)|data[5];

    p_getintdata=p_int;
    /// convert uints to floats ///
    float p = uint_to_float(p_int, P_MIN, P_MAX, 16);
    float v = uint_to_float(v_int, V_MIN, V_MAX, 12);
    float t = uint_to_float(i_int, -T_MAX, T_MAX, 12);
    p_getdata=p;

    if(id==0x01){
        p_getdata_s1=(-p_getdata);
    }else if(id==0x02)
    {
        p_getdata_s2=(-(p_getdata+M_PI));
    }
}

void sent_position(){
	float current_coordinate[2];
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;
	BLDC_FK(p_getdata_s1,p_getdata_s2,&current_coordinate[0],&current_coordinate[1]);

	x1=round(l1*cos(p_getdata_s1));
	y1=round(l1*sin(p_getdata_s1));
	x2=round(current_coordinate[0]);	//手先座標系から機械座標系への変換
	y2=round(current_coordinate[1]);
	TxData[0]=0xAA;
	TxData[1]=x1&0xFF;
	TxData[2]=(x1>>8)&0xFF;
	TxData[3]=y1&0xFF;
	TxData[4]=(y1>>8)&0xFF;
	TxData[5]=x2&0xFF;
	TxData[6]=(x2>>8)&0xFF;
	TxData[7]=y2&0xFF;
	TxData[8]=(y2>>8)&0xFF;
	TxData[19]=0xAA;
	HAL_UART_Transmit(&huart2,TxData,sizeof(TxData),100);
}

void BLDC_MotorRotate(){
	float count_rad=0.05;
	float set_Kp=40.0;
	float setMax_kp=200.0;
	uint8_t count_end=0;
	uint32_t canid_select_far;
	uint32_t canid_select_near;
	float far_dis_dir=0;
	float near_dis_dir=0;
	float running_pos[2];
	float far_dis;
	float near_dis;
	float i=0.0;
	float m1_startpos;
	float m2_startpos;
	float m1_target_pos;
	float m2_target_pos;
	float far_getpos;
	float near_getpos;
	canid=0x01;
	pack_cmd(0.0, 0.0, 0.0, 3.0, 0.0);	//現在の場所の所得
	BLDC_CANTx();
	unpack_reply();
	m1_startpos=p_getdata;
	m1_target_pos=-t_position[0];
	running_pos[0]=fabsf(m1_target_pos-m1_startpos);	//目標地点ー現在の地点＝移動距離
	canid=0x02;
	pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);	//現在の場所の所得
	BLDC_CANTx();
	unpack_reply();
	m2_target_pos=(-t_position[1]+M_PI);
	m2_startpos=p_getdata;
	running_pos[1]=fabsf(m2_target_pos-m2_startpos);

	if(running_pos[0]>running_pos[1]){
		far_dis=running_pos[0];
		near_dis=running_pos[1];
		canid_select_far=0x01;
		canid_select_near=0x02;
		far_getpos=m1_startpos;
		near_getpos=m2_startpos;
		far_dis_dir=((m1_target_pos-m1_startpos)>=0)?1:-1;
		near_dis_dir=((m2_target_pos-(m2_startpos))>=0)?1:-1;

	}else{
		far_dis=running_pos[1];
		near_dis=running_pos[0];
		canid_select_far=0x02;
		canid_select_near=0x01;
		far_getpos=m2_startpos;
		near_getpos=m1_startpos;
		far_dis_dir=((m2_target_pos-m2_startpos)>=0)?1:-1;
		near_dis_dir=((m1_target_pos-m1_startpos)>=0)?1:-1;
	}

	if (far_dis>count_rad){
		while(!count_end){
			i+=(far_dis_dir*count_rad);
			if(fabsf(i)<far_dis){
				canid=canid_select_far;
				pack_cmd(far_getpos+i, 0.0, set_Kp, 2.0, 0.0);	//現在の場所の所得
				BLDC_CANTx();
				unpack_reply();
				HAL_Delay(20);
				canid=canid_select_near;
				pack_cmd(near_getpos+fabsf(i)*(near_dis/far_dis)*near_dis_dir, 0.0, set_Kp, 2.0, 0.0);	//現在の場所の所得
				BLDC_CANTx();
				unpack_reply();
				sent_position();
				HAL_Delay(20);
			}
			else{
				count_end=1;
			}
		}
	}
	for(int i=1;i<=50;i++){
		if(set_Kp+i*10<=setMax_kp){
			canid=0x01;
			pack_cmd(m1_target_pos, 0.0, set_Kp+i*10, 3.0, 0.0);	//現在の場所の所得
			BLDC_CANTx();
			unpack_reply();
			HAL_Delay(20);
			canid=0x02;
			pack_cmd(m2_target_pos, 0.0, set_Kp+i*10, 3.0, 0.0);	//現在の場所の所得
			BLDC_CANTx();
			unpack_reply();
			HAL_Delay(20);
		}else{
			i=50;
		}
	}
	sent_position();

}
void BLDC_M1Rotate(){
	float m1_startpos;
	float m1_target_pos;
	float i=0.0;
	float m1_dis;
	float set_Kp=40.0;
	float setMax_kp=200.0;
	float count_rad=0.05;
	int m1_dir;
	int count=10;
	uint8_t count_end=0;
	canid=0x02;
	pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);	//現在の場所の所得
	BLDC_CANTx();
	unpack_reply();
	pack_cmd(p_getdata, 0.0, 200.0, 4.0, 0.0);	//現在の場所の所得
	BLDC_CANTx();
	unpack_reply();
	canid=0x01;
	pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);	//現在の場所の所得
	BLDC_CANTx();
	unpack_reply();

	m1_startpos=p_getdata;
	m1_target_pos=-t_position[0];
	m1_dis=fabsf(m1_target_pos-m1_startpos);	//目標地点ー現在の地点＝移動距離
	m1_dir=((m1_target_pos-m1_startpos)>=0)?1:-1;
	i=count_rad;
	if(count_rad<m1_dis){
		while(!count_end){
			i+=(m1_dir*count_rad);
			if(fabsf(i)<m1_dis){
				canid=0x01;
				pack_cmd(m1_startpos+i, 0.0, set_Kp, 2.0, 0.0);	//現在の場所の所得
				BLDC_CANTx();
				sent_position();
				HAL_Delay(40);
			}else{
				count_end=1;
			}
		}
	}
	for(int i=1;i<=50;i++){
			if(set_Kp+i*10<=setMax_kp){
				canid=0x01;
				pack_cmd(m1_target_pos, 0.0, set_Kp+i*10, 3.0, 0.0);	//現在の場所の所得
				BLDC_CANTx();
				HAL_Delay(20);
			}else{
				i=50;
			}
		}
	sent_position();
}
void BLDC_M2Rotate(){
	float m2_startpos;
	float m2_target_pos;
	float i=0.0;
	float m2_dis;
	float set_Kp=40.0;
	float setMax_kp=200.0;
	float count_rad=0.05;
	int m2_dir;
	uint8_t count_end=0;
	canid=0x01;
	pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);	//現在の場所の所得
	BLDC_CANTx();
	unpack_reply();
	pack_cmd(p_getdata, 0.0, 200.0, 4.0, 0.0);	//現在の場所の所得
	BLDC_CANTx();
	unpack_reply();
	canid=0x02;
	pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);	//現在の場所の所得
	BLDC_CANTx();
	unpack_reply();
	m2_startpos=p_getdata;
	m2_target_pos=(-t_position[1]+M_PI);
	m2_dis=fabsf((m2_target_pos-m2_startpos));
	m2_dir=((m2_target_pos-m2_startpos)>=0)?1:-1;

	i=count_rad;
	if(count_rad<m2_dis){
		while(!count_end){
			i+=(m2_dir*count_rad);
			if(fabsf(i)<m2_dis){
				canid=0x02;
				pack_cmd(m2_startpos+i, 0.0, set_Kp, 2.0, 0.0);	//現在の場所の所得
				BLDC_CANTx();
				p_getdata_s2=p_getdata;
				sent_position();
				HAL_Delay(40);
			}else{
				count_end=1;
			}
		}
	}

	for(int i=1;i<=50;i++){
			if(set_Kp+i*10<=setMax_kp){
				canid=0x02;
				pack_cmd(m2_target_pos, 0.0, set_Kp+i*10, 3.0, 0.0);	//現在の場所の所得
				BLDC_CANTx();
				HAL_Delay(20);
			}else{
				i=50;
			}
		}
	sent_position();
	count_end=0;
}

void command_judgment(uint8_t *ModeSelect){
	int modeflag_count=0;
	for(uint8_t i=0; i<buffersize ;i++){
		ModeSelect[i]= ~(ModeSelect[i]);
		if(ModeSelect[i]==0){
			modeflag_count++;
			}
		}
	if(modeflag_count<(buffersize-3)){	//動作モード
		selectmode_flag=0;
	}
	else if(modeflag_count==(buffersize-3)){	//コマンドモード,9byte ModeSelectが0だったら
		selectmode_flag=1;
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
  MX_CAN2_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  uint32_t Init_flag=0;
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)!=1){	//リミットスイッチON→待機状態
	  home_position_state=1;
  }else{
	  home_position_state=0;
  }
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
uint8_t judgment[20];
uint8_t modeflag=0;
uint8_t checkdata[20];
uint8_t mode_select;
int16_t receive_xy_coordinate[2];
int16_t sent_data[20];

char Mode_Type[4];
float rad_data[2];
float machine_coordinate_data[2];
float machine_origin_rad[20];
float machine_origin_coordinate[20];
int receive_int_coordinate[2];
machine_origin_coordinate[0]=0.0;
machine_origin_coordinate[1]=400.0;
BLDC_IK(machine_origin_coordinate[0],machine_origin_coordinate[1],&rad_data[0],&rad_data[1]);
machine_origin_rad[0]=rad_data[0];
machine_origin_rad[1]=rad_data[1];


//RxData[0]=0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  home_position_state=0;

	  get_uart_flag=0;
	  HAL_UART_Receive_IT(&huart2, (uint8_t *)RxData, buffersize);
//	  HAL_UART_Transmit(&huart2, (uint8_t *)RxData, buffersize,300);
	  //受信するまで待つ
	  while(!get_uart_flag){}
	  get_can_flag=1;//canデータ受信用のフラグを0に
	  for(int i=0;i<buffersize;i++){
		  judgment[i]=RxData[i];
	  }
	  if(RxData[0]==0xAA&&RxData[(buffersize-1)]==0xAA){
		  command_judgment(judgment);
	  }else{
		  selectmode_flag=2;
		  TxData[0]=0xAA;
		  TxData[buffersize-1]=0xAA;
		  for(int i=1;i<buffersize-1;i++){
			  TxData[i]=0xFF;
		  	  }
		  HAL_UART_Transmit(&huart2,TxData,sizeof(TxData),100);
	  }

	  if(selectmode_flag==1){
		  mode_select=RxData[1];	//モード変更
		  }
	  else if(selectmode_flag==0){

		  receive_xy_coordinate[0]=RxData[5]&0xFF;	//受け取った座標データをビットシフトしてx,yの値に直す
		  receive_xy_coordinate[0]+= (RxData[6]&0xFF)<<8;
		  receive_xy_coordinate[1]=RxData[7]&0xFF;
		  receive_xy_coordinate[1]+= (RxData[8]&0xFF)<<8;
//		  HAL_UART_Transmit_IT(&huart2,RxData,sizeof(RxData));
		  machine_coordinate_data[0]=(float)receive_xy_coordinate[0];
		  machine_coordinate_data[1]=(float)receive_xy_coordinate[1];
		  BLDC_IK(machine_coordinate_data[0],machine_coordinate_data[1],&rad_data[0],&rad_data[1]);
	  }
	  if(home_position_state==1){	//初期位置モード
		  switch(mode_select){
		  case 0x0A:
			  canid=0x01;
			  EnterMotorMode();
			  BLDC_CANTx();
			  canid=0x02;
			  EnterMotorMode();
			  BLDC_CANTx();
			  break;
//		  case 0x1A:
//			  t_position[0] = machine_origin_rad[0];
//			  t_position[1] = machine_origin_rad[1];
//			  BLDC_MotorRotate();
//			  RxData[0]=0;
//			  break;
//		  case 0x2A:
//			  if(selectmode_flag==0){	//初期位置の変更
//				  receive_xy_coordinate[0] = RxData[0]&0xFF;
//				  receive_xy_coordinate[0] += (RxData[1]&0xFF)<<8;
//				  receive_xy_coordinate[1] = RxData[2]&0xFF;
//				  receive_xy_coordinate[1] += (RxData[3]&0xFF)<<8;
//				  machine_origin_coordinate[0]=(float)receive_xy_coordinate[0];
//				  machine_origin_coordinate[1]=(float)receive_xy_coordinate[1];
//				  BLDC_IK(machine_origin_coordinate[0],machine_origin_coordinate[1],&rad_data[0],&rad_data[1]);
//				  machine_origin_rad[0]=rad_data[0];
//				  machine_origin_rad[1]=rad_data[1];
//			  }
//			  break;
		  default:
			  get_can_flag=1;
			  break;
		  }
	  }
	  else if(home_position_state==0){
		  switch(mode_select){	//
		  case 0x01:	//モーターON
			  canid=0x01;
			  EnterMotorMode();
			  BLDC_CANTx();
			  canid=0x02;
			  EnterMotorMode();
			  BLDC_CANTx();

			  break;
		  case 0x02:	//モーターOFF
			  canid=0x01;
			  ExitMotorMode();
			  BLDC_CANTx();
			  canid=0x02;
			  ExitMotorMode();
			  BLDC_CANTx();
			  break;
		  case 0x03:	//動作モード
			  if(selectmode_flag==0){
				  t_position[0] = rad_data[0];
				  t_position[1] = rad_data[1];
				  BLDC_MotorRotate();
			  }
			  break;
		  case 0x04:
			  t_position[0] = 0;
			  t_position[1] = 0;
			  BLDC_M2Rotate();
			  BLDC_M1Rotate();
			  t_position[0] = machine_origin_rad[0];	//スタンバイ状態
			  t_position[1] = machine_origin_rad[1];
			  BLDC_MotorRotate();
			  break;
		  case 0x05:	//初期状態へ戻る
		 	  t_position[0] = 0;
		 	  t_position[1] = M_PI;
		 	  BLDC_MotorRotate();
		 	  break;
		  case 0x06:	//腕伸ばした状態
			  t_position[0] = 0;
			  t_position[1] = 0;
			  BLDC_MotorRotate();
			  break;
		  case 0x07:	//ティーチングモード開始
			  canid=0x01;
			  pack_cmd(0.0, 0.0, 0.0, 1.0, 0.0);
			  BLDC_CANTx();
			  canid=0x02;
			  pack_cmd(0.0, 0.0, 0.0, 1.0, 0.0);
			  BLDC_CANTx();
			  break;
		  case 0x08:	//ティーチング決定
			  canid=0x01;
			  pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);
			  BLDC_CANTx();
			  unpack_reply();
			  pack_cmd(p_getdata, 0.0, 100.0, 3.0, 0.0);
			  BLDC_CANTx();
			  unpack_reply();
			  rad_data[0]=p_getdata;
			  canid=0x02;
			  pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);
			  BLDC_CANTx();
			  unpack_reply();
			  pack_cmd(p_getdata, 0.0, 100.0, 3.0, 0.0);
			  BLDC_CANTx();
			  unpack_reply();
			  rad_data[1]=p_getdata;
			  BLDC_FK(-rad_data[0],-(rad_data[1]+M_PI),&machine_coordinate_data[0],&machine_coordinate_data[1]);
			  sent_data[0]=round(machine_coordinate_data[0]);	//手先座標系から機械座標系への変換
			  sent_data[1]=round(machine_coordinate_data[1]);
			  TxData[0]=0xAA;
			  TxData[5]=sent_data[0]&0xFF;
			  TxData[6]=(sent_data[0]>>8)&0xFF;
		   	  TxData[7]=sent_data[1]&0xFF;
			  TxData[8]=(sent_data[1]>>8)&0xFF;
			  TxData[19]=0xAA;
			  HAL_UART_Transmit(&huart2,TxData,sizeof(TxData),100);
			  break;

		  case 0x90:
			  canid=0x01;
			  pack_cmd(0.0, 0.0, 0.0, 3.0, 0.0);
			  BLDC_CANTx();
			  unpack_reply();
			  rad_data[0]=p_getdata;
			  canid=0x02;
			  pack_cmd(0.0, 0.0, 0.0, 3.0, 0.0);
			  BLDC_CANTx();
			  unpack_reply();
			  rad_data[1]=p_getdata;

			  canid=0x01;
			  pack_cmd(rad_data[0]+0.1, 0.0, 50.0, 2.0, 0.0);
			  BLDC_CANTx();
			  unpack_reply();
			  rad_data[0]=p_getdata;
			  canid=0x02;
			  pack_cmd(rad_data[1]+0.1, 0.0, 50.0, 2.0, 0.0);
			  BLDC_CANTx();
			  unpack_reply();
			  rad_data[1]=p_getdata;

			  BLDC_FK(rad_data[0],rad_data[1],&machine_coordinate_data[0],&machine_coordinate_data[1]);
			  sent_data[0]=round(machine_coordinate_data[0]);	//手先座標系から機械座標系への変換
			  sent_data[1]=round(machine_coordinate_data[1]);
			  TxData[0]=0xAA;
			  TxData[5]=sent_data[0]&0xFF;
			  TxData[6]=(sent_data[0]>>8)&0xFF;
			  TxData[7]=sent_data[1]&0xFF;
			  TxData[8]=(sent_data[1]>>8)&0xFF;
			  TxData[19]=0xAA;
			  HAL_UART_Transmit_IT(&huart2,TxData,sizeof(TxData));

			  break;
		  default:
			  get_can_flag=1;
			  break;
		  }

		  get_uart_flag=0;
		  while(!get_can_flag){}
	  }

	  mode_select=RxData[1];	//モード変更
	  TxData[0]=0xAA;
	  TxData[1]=mode_select;
	  TxData[buffersize-1]=0xAA;
	  for(int i=2;i<buffersize-1;i++){
		  TxData[i]=0xFF;
	  }
	  HAL_UART_Transmit(&huart2,TxData,sizeof(TxData),100);
	  for(int i=0;i<buffersize;i++){
	  		  RxData[i]=0;
	  		  TxData[i]=0;
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
  hcan1.Init.TimeSeg1 = CAN_BS1_7TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
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
  hcan2.Init.TimeSeg1 = CAN_BS1_7TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_2TQ;
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

  /*Configure GPIO pins : PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
void BLDC_FK(float s1,float s2,float* xp,float* yp){
	float x=0.0;
	float y=0.0;
	x=l1*cos(s1)+l2*cos(s1+s2);
	y=l1*sin(s1)+l2*sin(s1+s2);
	printf("s1=%f\r\n",s1);
	printf("s2=%f\r\n",s2);
	printf("x=%f\r\n",x);
	printf("y=%f\r\n",y);
	*xp=x;
	*yp=y;
}
void BLDC_IK(float x,float y,float* s1p,float* s2p){
	float s1=0.0;
	float s2=0.0;
	s2=-acos((x*x+y*y-l1*l1-l2*l2)/(2*l1*l2));
	s1=atan2((-l2*sin(s2)*x+(l1+l2*cos(s2))*y),((l1+l2*cos(s2))*x+l2*sin(s2)*y));
	printf("s1=%f\r\n",s1);
	printf("s2=%f\r\n",s2);
	printf("x=%f\r\n",x);
	printf("y=%f\r\n",y);
	*s1p=s1;
	*s2p=s2;
}
//int _write(int file, char *ptr, int len)	//printfに必要
//{
//  HAL_UART_Transmit(&huart2,(uint8_t *)ptr,len,10);
//  return len;
//}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)
{
	get_uart_flag=1;
//	HAL_UART_Transmit(&huart2, (uint8_t *)RxData, buffersize,10);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)RxData, buffersize);
	if(RxData[0]==0xAA&&RxData[1]==0xCC&&RxData[(buffersize-1)]==0xAA){
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)RxData, buffersize);
	}

}
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
//	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1);
	  nextcan_flag=1;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	get_can_flag=1;
//	printf("RxCan\r\n");
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0, &RxHeader,data);

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
