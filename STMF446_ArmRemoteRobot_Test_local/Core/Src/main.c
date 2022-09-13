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
#include<string.h>
#include <math.h>
#include "math_ops.h"
#include "leg_message.h"
#include "tmc/ic/TMC2209/TMC2209.h"
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
	volatile int stall_off;

	volatile int seeking_vel;
	volatile int homing_vel;

	volatile unsigned int totalSteps;

	volatile char movementDone;
	volatile unsigned int rampUpStepCount;
	volatile unsigned int n; // index
	volatile float d; // current interval length
	volatile unsigned long di; // d -> unsigned  di
	volatile unsigned int stepCount;

}stepperInfo;   //stepperInfo型の構造体にメンバを格納

typedef struct{
	volatile unsigned int checkcount;
}checkerInfo;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TMC2209_CRC(data, length) tmc_CRC8(data, length, 1)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 CAN_HandleTypeDef hcan1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
CAN_FilterTypeDef sFilterConfig;
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint8_t TxData[20];
uint8_t RxData[20];
uint8_t can_data[20];

uint8_t Rx_can_data[20];
uint8_t Tx_can_data[20];
uint8_t m1Tx_can_data[20];
uint8_t m2Tx_can_data[20];
uint8_t RMDTx_can_data[20];
uint8_t m1Rx_can_data[20];
uint8_t m2Rx_can_data[20];
uint8_t RMDRx_can_data[20];

uint32_t TxMailbox;
uint32_t canid;
uint8_t cmd_data = 0x00;
uint8_t home_position_state =0;
uint8_t get_uart_flag = 0;
uint8_t get_can_flag = 0; //canを受信したかどうかのflag
uint8_t nextcan_flag = 0;
uint8_t selectmode_flag = 0;
uint8_t motion_stop_flag = 0;

int BLDCm1_dir=1;
int BLDCm2_dir=-1;
float p_getdata=0.0;
float p_getdata_s1=0.0;
float p_getdata_s2=0.0;
float t_position[2];
uint16_t p_getintdata=0;
float l1=250.0;
float l2=350.0;
//SteppingMotorの設定
TMC2209TypeDef TMC2209; //TMC2209の設定などの定義
ConfigurationTypeDef TMC2209_config; //TMC2209の設定
volatile stepperInfo steppers[1];    //stepperInfo型の配列stepperにNUM_STEPPERSで定義した値を代入
volatile checkerInfo currentget[1];
uint8_t zhome_endstop_state = 0;
uint8_t homing_flag = 0;
uint8_t current_stall = 0;
//RMDの設定
int Writecom=0x00;
int Readcom=0x00;
uint8_t UART1_Data[8];
uint8_t end_flag = 0; //canを受信したかどうかのflag
char RMD_cmd;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
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
//BLDC//
uint8_t uart_data;
uint8_t buffersize=1;
typedef struct {
	uint16_t rxMsg;
	uint16_t txMsg;
}CANMessage;
//steppingmotor//
static uint8_t reset(void);
static uint8_t restore(void);


void BLDC_FK(float s1,float s2,float* xp,float* yp);
void BLDC_IK(float x,float y,float* s1p,float* s2p);

void Zero(){
	m1Tx_can_data[0] = 0xFF;
	m1Tx_can_data[1] = 0xFF;
	m1Tx_can_data[2] = 0xFF;
	m1Tx_can_data[3] = 0xFF;
	m1Tx_can_data[4] = 0xFF;
	m1Tx_can_data[5] = 0xFF;
	m1Tx_can_data[6] = 0xFF;
	m1Tx_can_data[7] = 0xFE;

	m2Tx_can_data[0] = 0xFF;
	m2Tx_can_data[1] = 0xFF;
	m2Tx_can_data[2] = 0xFF;
	m2Tx_can_data[3] = 0xFF;
	m2Tx_can_data[4] = 0xFF;
	m2Tx_can_data[5] = 0xFF;
	m2Tx_can_data[6] = 0xFF;
	m2Tx_can_data[7] = 0xFE;
    //WriteAll();
    }

void EnterMotorMode(){	//アームBLDC ON	コマンド
	m1Tx_can_data[0] = 0xFF;
	m1Tx_can_data[1] = 0xFF;
	m1Tx_can_data[2] = 0xFF;
	m1Tx_can_data[3] = 0xFF;
	m1Tx_can_data[4] = 0xFF;
	m1Tx_can_data[5] = 0xFF;
	m1Tx_can_data[6] = 0xFF;
	m1Tx_can_data[7] = 0xFC;

	m2Tx_can_data[0] = 0xFF;
	m2Tx_can_data[1] = 0xFF;
	m2Tx_can_data[2] = 0xFF;
	m2Tx_can_data[3] = 0xFF;
	m2Tx_can_data[4] = 0xFF;
	m2Tx_can_data[5] = 0xFF;
	m2Tx_can_data[6] = 0xFF;
	m2Tx_can_data[7] = 0xFC;

    //WriteAll();
    }

void ExitMotorMode(){	//アームBLDC OFFコマンド
	m1Tx_can_data[0] = 0xFF;
	m1Tx_can_data[1] = 0xFF;
	m1Tx_can_data[2] = 0xFF;
	m1Tx_can_data[3] = 0xFF;
	m1Tx_can_data[4] = 0xFF;
	m1Tx_can_data[5] = 0xFF;
	m1Tx_can_data[6] = 0xFF;
	m1Tx_can_data[7] = 0xFD;

	m2Tx_can_data[0] = 0xFF;
	m2Tx_can_data[1] = 0xFF;
	m2Tx_can_data[2] = 0xFF;
	m2Tx_can_data[3] = 0xFF;
	m2Tx_can_data[4] = 0xFF;
	m2Tx_can_data[5] = 0xFF;
	m2Tx_can_data[6] = 0xFF;
	m2Tx_can_data[7] = 0xFD;
    //WriteAll();
    }
void pack_cmd(float _p_des, float _v_des, float _kp, float _kd, float _t_ff){	//floatのradをcanで送れるuint8に変換
	uint8_t tx_can_data[20];

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
	tx_can_data[0] = p_int>>8;
	tx_can_data[1] = p_int&0xFF;
	tx_can_data[2] = v_int>>4;
	tx_can_data[3] = ((v_int&0xF)<<4)|(kp_int>>8);
	tx_can_data[4] = kp_int&0xFF;
	tx_can_data[5] = kd_int>>4;
	tx_can_data[6] = ((kd_int&0xF)<<4)|(t_int>>8);
	tx_can_data[7] = t_int&0xFF;
	if(canid==0x01){
		memcpy(m1Tx_can_data,tx_can_data,sizeof(m1Tx_can_data));
	}else if(canid==0x02){
		memcpy(m2Tx_can_data,tx_can_data,sizeof(m2Tx_can_data));
	}

//	if(canid==0x01){
//		for(int i=0;i<20;i++){	//memset(メモリーセット)で一発でできる
//			m1Tx_can_data[i]=tx_can_data[i];
//		}
//	}else if(canid==0x02){
//		for(int i=0;i<20;i++){	//memset(メモリーセット)で一発でできる
//			m2Tx_can_data[i]=tx_can_data[i];
//		}
//	}

}
void BLDC_CANTx(){	//BLDCにcanを送る
	uint8_t BLDC_can_data[20];
	TxHeader.StdId=canid;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 0x08;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
//	HAL_CAN_AddTxMessage(&hcan1,&TxHeader,data,&TxMailbox);
//	HAL_Delay(10);

//	int i = 0;
	int count=0;
	uint8_t can_mailboxflag=0;
	//HAL_CAN_GetTxMailboxesFreeLevelがあくまで待機
	if(canid==0x01){
		memcpy(BLDC_can_data,m1Tx_can_data,sizeof(m1Tx_can_data));
	}else if(canid==0x02){
		memcpy(BLDC_can_data,m2Tx_can_data,sizeof(m2Tx_can_data));
	}

	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)==0){}
	HAL_CAN_AddTxMessage(&hcan1,&TxHeader,BLDC_can_data,&TxMailbox);
	HAL_Delay(10);

//	while(!nextcan_flag){
//		if(0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) ){
//			HAL_CAN_AddTxMessage(&hcan1,&TxHeader,BLDC_can_data,&TxMailbox);
//			HAL_Delay(10);
//			count++;
//			if(count==10){
//				nextcan_flag=1;
//				get_can_flag=1;
//				HAL_CAN_Stop (&hcan1);
//				HAL_CAN_Start(&hcan1);
//			}
//		}
//	}
	nextcan_flag=0;
	count=0;
//	}else{
//		HAL_CAN_Stop (&hcan1);
//		HAL_CAN_Start(&hcan1);
//		nextcan_flag=1;
//		get_can_flag=1;
//	}
}

void unpack_reply(){	//canで受け取ったデータをfloatに変換
    /// unpack ints from can buffer ///
	uint8_t BLDC_can_data[20];
	uint16_t id ;
	uint16_t p_int ;
	uint16_t v_int ;
	uint16_t i_int ;

	if(canid==0x01){
		memcpy(BLDC_can_data,m1Rx_can_data,sizeof(m1Rx_can_data));
	}else if(canid==0x02){
		memcpy(BLDC_can_data,m2Rx_can_data,sizeof(m2Rx_can_data));
	}
//	printf("m1Rx_can_data%d\r\n",m1Rx_can_data[0]);
//	printf("m2Rx_can_data%d\r\n",m2Rx_can_data[0]);
	id = BLDC_can_data[0];
	p_int = (BLDC_can_data[1]<<8)|BLDC_can_data[2];
	v_int = (BLDC_can_data[3]<<4)|(BLDC_can_data[4]>>4);
	i_int = ((BLDC_can_data[4]&0xF)<<8)|BLDC_can_data[5];

//    p_getintdata=p_int;
    /// convert uints to floats ///
    float p = uint_to_float(p_int, P_MIN, P_MAX, 16);
    float v = uint_to_float(v_int, V_MIN, V_MAX, 12);
    float t = uint_to_float(i_int, -T_MAX, T_MAX, 12);
    p_getdata=p;

    if(id==0x01){
    	p_getdata_s1=(-p_getdata);
//    	printf("m1_p_getdata%f\r\n",p_getdata);
    }else if(id==0x02)
    {
//    	printf("m1_p_getdata%f\r\n",p_getdata);
    	p_getdata_s2=(-p_getdata+M_PI);
    }
}
void RMDTx(){	//RMDへのca
	int count=0;
	TxHeader.StdId=0x141;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 0x08;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxData[1] = 0x00;

	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)==0){}
	while(!nextcan_flag){
		HAL_CAN_AddTxMessage(&hcan1,&TxHeader,RMDTx_can_data,&TxMailbox);
		HAL_Delay(10);
		count++;
		if(count==10){
			nextcan_flag=1;
			get_can_flag=1;
			HAL_CAN_Stop (&hcan1);
			HAL_CAN_Start(&hcan1);
		}
	}
	nextcan_flag=0;
	count=0;
}
void RMDZero(){
	int get_RMD_deg;
	int first_deg_pos=90*100;

	get_RMD_deg=first_deg_pos+100*(((-p_getdata_s1-p_getdata_s2+M_PI)*180)/M_PI);
	RMDTx_can_data[0] = 0xA4;
	RMDTx_can_data[1] = 0x00;
	RMDTx_can_data[2] = 0x64;	//speed low
	RMDTx_can_data[3] = 0x00;	//speed high
	RMDTx_can_data[4] =(get_RMD_deg&0xFF);	//poslow
	RMDTx_can_data[5] =(get_RMD_deg>>8)&0xFF;
	RMDTx_can_data[6] = 0x00;
	RMDTx_can_data[7] = 0x00;
	RMDTx();
}
//void RMDZero(){
//	int get_RMD_deg;
//	int first_deg_pos=90*100;
//
//	get_RMD_deg=first_deg_pos+100*((-p_getdata_s1-p_getdata_s2+M_PI)*180)/M_PI;
//	can_data[0] = 0xA4;
//	can_data[1] = 0x00;
//	can_data[2] = 0x2C;	//speed low
//	can_data[3] = 0x01;	//speed high
//	can_data[4] =(get_RMD_deg&0xFF);	//poslow
//	can_data[5] =(get_RMD_deg>>8)&0xFF;
//	can_data[6] = 0x00;
//	can_data[7] = 0x00;
//
//	RMDTx();
//}
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

	RMDZero();	//RMDを正面に向ける
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
	printf("m1_startpos=%lf\r\n",p_getdata);
	m1_target_pos=-t_position[0];
	running_pos[0]=fabsf(m1_target_pos-m1_startpos);	//目標地点ー現在の地点＝移動距離
	printf("running_posm1=%lf\r\n",running_pos[0]);
	canid=0x02;
	pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);	//現在の場所の所得
	BLDC_CANTx();
	unpack_reply();
	m2_target_pos=(-t_position[1]+M_PI);
	m2_startpos=p_getdata;
	printf("m2_startpos=%lf\r\n",p_getdata);
	running_pos[1]=fabsf(m2_target_pos-m2_startpos);
	printf("running_posm2=%lf\r\n",running_pos[1]);

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
		while(!count_end&&!motion_stop_flag){
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
		if(set_Kp+i*10<=setMax_kp&&!motion_stop_flag){
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
	motion_stop_flag=0;
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
		while(!count_end&&!motion_stop_flag){
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
			if(set_Kp+i*10<=setMax_kp&&!motion_stop_flag){
				canid=0x01;
				pack_cmd(m1_target_pos, 0.0, set_Kp+i*10, 3.0, 0.0);	//現在の場所の所得
				BLDC_CANTx();
				HAL_Delay(20);
			}else{
				i=50;
			}
		}
	sent_position();
	motion_stop_flag=0;
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
		while(!count_end&&!motion_stop_flag){
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
			if(set_Kp+i*10<=setMax_kp&&!motion_stop_flag){
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
	motion_stop_flag=0;
}

//void command_judgment(){
//	int modeflag_count=0;
//	for(uint8_t i=0; i<buffersize ;i++){
//		RxData[i]= ~(RxData[i]);
//		if(RxData[i]==0){
//			modeflag_count++;
//			}
//		}
//	if(modeflag_count<(buffersize-3)){	//動作モード
//		selectmode_flag=0;
//	}
//	else if(modeflag_count==(buffersize-3)){	//コマンドモード,9byte ModeSelectが0だったら
//		selectmode_flag=1;
//	}
//}

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

void Step0(){                                       //step pin
	//pull+

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	/*
	 * pull-
	 * HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	 * HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	 */
}

void Dir0(int dir){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, dir); //Dir0関数にdirピンを指定して格納
}


void resetStepperInfo(stepperInfo* si){    //resetStepperInfo関数に引数をstepperInfo構造隊にポイント型変数siとして与える
	si->n = 0;  //変数*siがメンバnにアクセス.した値に0を代入する
	si->d = 0;
	si->di = 0;
	si->stepCount = 0;
	si->rampUpStepCount = 0;
	si->totalSteps = 0;
	si->stepPosition = 0;
	si->movementDone = 0;
}


void resetStepper(volatile stepperInfo* si){    //resetStepper関数に引数をstepperInfo構造隊にポイント型変数siとして与える
	si->c0 = si->acceleration;      //変数*siがメンバc0にアクセス.した値に変数*siがメンバaccelerationにアクセス.した値を代入する
	si->d = si->c0;
	si->di = si->d;
	si->stepCount = 0;
	si->n = 0;
	si->rampUpStepCount = 0;
	si->movementDone = 0;
}

volatile uint8_t remainingSteppersFlag = 0; //remainingSteppersFlagをuint8_t型で定義し、初期化

void prepareMovement(int steps){
	printf("prepareMovement_IN\n\r");
	if(steps == 0){
		return;
	}
	volatile stepperInfo* si = &steppers[0];
	if(si->dir_inv){
		si->dirFunc( steps < 0 ? 0 : 1);
		si->dir = steps > 0 ? -1:1;
	}else{
		si->dirFunc( steps < 0 ? 1 : 0);
		si->dir = steps > 0 ? 1:-1;
	}
		si->totalSteps = abs(steps);
		resetStepper(si);
		remainingSteppersFlag |= (1 << 0);

}

void prepareAbsoluteMovement(int absolute_steps){   //prepareAbsoluteMovementで与えられたステップ数だけモータを回す(main文から）
	printf("prepareAbsoluteMovement_IN\n\r");
	volatile stepperInfo* si = &steppers[0];	//steppers[0]のアドレスをstepperInfo* si に代入
	int steps = absolute_steps - si->stepPosition;	//目標値からstepPositionを引いた値（stepposition=毎ステップカウントされるdirの値)
	if(steps == 0){
					return;
				}
	si->dirFunc( steps < 0 ? 1 : 0);
	si->dir = steps > 0 ? 1:-1;
	//printf("prepareAbsoluteMovement\r\n");
	si->totalSteps = abs(steps);	//stepsの値をアブソリュートでtotalstepsに格納

	resetStepper(si);
	remainingSteppersFlag |= (1 << 0);
}
void currentcheck(){
	uint32_t value=0;
	volatile checkerInfo* t = &currentget[0];
		t->checkcount++;
		if(t->checkcount==2000){
			value=tmc2209_readInt(&TMC2209, TMC2209_SG_RESULT);

//			value=TMC2209_FIELD_READ(&TMC2209, TMC2209_TSTEP, TMC2209_TSTEP_MASK, TMC2209_TSTEP_SHIFT);
//			printf("tstep: %ld\r\n", value);

			tmc2209_periodicJob(&TMC2209, HAL_GetTick());
			t->checkcount=0;
		}
}
volatile uint8_t nextStepperFlag = 0;

void setNextInterruptInterval(){
	unsigned int mind = 999999;
//	currentcheck();
		if( ((1 << 0)& remainingSteppersFlag) && steppers[0].di < mind ){
			mind = steppers[0].di;

		}
		nextStepperFlag = 0;

			if( (1 << 0) && steppers[0].di == mind ){
				nextStepperFlag |= (1 << 0);
			}

		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, mind);		//mindにコンペアを設定
			//printf("mind:%d \r\n",mind);

	if (remainingSteppersFlag == 0){
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65500);    //65500→コンペアマッチさせない
	}

	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, mind);
	return;

}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){      //指定したコンペアを超えた時発火
	//HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_1);

	if(htim == &htim3){
	unsigned int tmpCtr = __HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1);	//指定されたタイマーチャンネルのコンペアを所得
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65500);			//コンペアを65500にセット

	if( homing_flag & (1 << 0)){
				if(zhome_endstop_state & (1 << 0)){
					remainingSteppersFlag &= ~(1 << 0);
				}
			}
	if ( ! (nextStepperFlag & (1 << 0)) ){				//nextStepperFlagが真でない(0なら)
			steppers[0].di -= tmpCtr;           //tmpCtrの値からsteppers[i].di引いた値を新しくsteppers[i].diに代入
		}
		//printf("steppers:%ld \r\n",steppers[0].di);

		volatile stepperInfo* s = &steppers[0];

		if( (s->stepCount) < (s->totalSteps)&& !(current_stall)  ){		//1周ごとにカウントしたstepCountがabs(steps)より小さかったら==動作が終了するまで
			s->stepFunc();
			s->stepCount++;							//カウントし続ける
			s->stepPosition += s->dir;		//dir(+もしくはー(dirの向き）を加算してstepPositionに格納
			if ( (s->stepCount) >= (s->totalSteps) ){	//stepCountがtotalStepsを超えたとき
				s->movementDone = 1;	//movementDoneを1にする
				remainingSteppersFlag &= ~(1 << 0);
			}

		}

		if (s->rampUpStepCount == 0){
			s->n++;									//sにnを代入して、nをプラス1する
			s->d = s->d - (2*s->d) / (4*s->n +1);	//加減速遅延時間
			if (s->d <= s->minStepInterval ){		//加減速遅延時間がminStepIntervalより短いなら（定数で指定）
				s->d = s->minStepInterval;			//minStepIntervalを加減速遅延時間にする
				s->rampUpStepCount = s->stepCount;	//rampUpStepCountはstepCountとする
			}
			if (s->stepCount >= (s->totalSteps / 2) ){  //目標値から現在のステップを引いた値の1/2よりstepCountが大きくなったとき
				s->rampUpStepCount = s->stepCount;		//rampUpStepCountはstepCountとする
			}
		} else if ( s->stepCount >= s->totalSteps - s->rampUpStepCount) {	//目標値から現在のステップを引いた値からrampUpStepCountを引いた値がstepCountより小さいなら
			s->d = (s->d * (4 * s->n + 1)) / (4 * s->n + 1 -2);				//減速の時のステップ
			s->n--;		//sにnを代入し、nから1を引く
		}
		s->di = s->d;	//計算結果sをstepperInfoのメンバdに代入したものをメンバdiに代入
//		printf("stepPosition:%d \r\n",s->stepPosition);
		setNextInterruptInterval();
		__HAL_TIM_SET_COUNTER(&htim3, 0);
	}
}

void sensorStateInit(){							//割り込みリセット
	if( ! HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)){	//割り込みピンが真でない(1でない=0なら)
		zhome_endstop_state |= 1;					//zhome_endstop_stateに左辺か右辺が1なら1を代入
	}
}

void stallguard_back(){
	prepareMovement(steppers[0].stall_off);	//stall_off
	current_stall=0;
	return;
}
void stallguard_error(){
	TxData[0]=0xAA;
	TxData[1]=0x62;
	TxData[buffersize-1]=0xAA;
	for(int i=2;i<buffersize-1;i++){
		TxData[i]=0xFF;
	}
	HAL_UART_Transmit_IT(&huart2,TxData,sizeof(TxData));

	return;
}

void runAndWait(){
	setNextInterruptInterval();
	while(remainingSteppersFlag && !(current_stall));
	if(current_stall==1){
		stallguard_back();
	}
	HAL_Delay(100);
	end_flag=1;
	//printf("posi%d\r\n",re->totalSteps);
}
void AbsoluteReset(){
	volatile stepperInfo* re = &steppers[0];

	re->n = 0;  //変数*siがメンバnにアクセス.した値に0を代入する
	re->d = 0;
	re->di = 0;
	re->stepCount = 0;
	re->rampUpStepCount = 0;
	re->totalSteps = 0;
	re->stepPosition = 0;
	end_flag=1;
}


void stepperHoming(){
	homing_flag |= (1 << 0);
	//seeking
	steppers[0].minStepInterval = steppers[0].seeking_vel;	//ホームピンに反応するまでモータ回す
	prepareMovement( -100000000);
	runAndWait();
	HAL_Delay(500);
	//pull-off

	homing_flag &= ~(1 << 0);								//zhome_endstop_stateが反応したらpull_offだけモータ回す
	prepareMovement(steppers[0].pull_off);
	runAndWait();
	HAL_Delay(500);
	//homing

	homing_flag |= (1 << 0);								//ホームピンに反応するまでモータ回す
	steppers[0].minStepInterval = steppers[0].homing_vel;
	prepareMovement( -100000000);
	runAndWait();
	HAL_Delay(500);
	//pull-off
	homing_flag &= ~(1 << 0);
	prepareMovement(steppers[0].pull_off);
	runAndWait();
	HAL_Delay(500);

	steppers[0].stepPosition = 0;
	steppers[0].homing = 1;
	steppers[0].minStepInterval = 500;

}
void MotorOFF(){
	int32_t value = 0;

	value = 0;
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT);	//保持電流

	value = 0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_FREEWHEEL_MASK, 20);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_PWMCONF, TMC2209_FREEWHEEL_MASK, 20, value);

	end_flag=1;


}

void InvertDir(){
	int32_t value = 0;

	value = 0;
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_GCONF, TMC2209_SHAFT_SHIFT, TMC2209_SHAFT_MASK, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_SHAFT_SHIFT, TMC2209_SHAFT_MASK);	//保持電流
}

void TMCsetup(){
	int32_t mstep_value = 0;
	int32_t toff_value = 0;
	int32_t microstep_value = 0;
	int32_t IRUN_value = 0;
	int32_t IHOLD_value = 0;
	int32_t IHOLDDELAY_value= 0;
	int32_t stallgard_setup_value=0;
	int32_t TCOOLTHRS_value=0;
	int32_t SEMAX_value=0;
	int32_t TBL_value = 0;
	int32_t stealthChop_value = 0;
	int32_t autoscale_value = 0;
	int32_t PWMAuto_value = 0;
	int32_t PWMAuto_scale = 0;
	int32_t value = 0;


	IRUN_value = 22
			;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT, IRUN_value);	//実行電流
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	IRUN_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT);

	IHOLD_value= 16;																						//モーター待機時の電流の設定
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT, IHOLD_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	IHOLD_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT);	//保持電流

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK,TMC2209_PDN_DISABLE_SHIFT);
	value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK, TMC2209_PDN_DISABLE_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK, TMC2209_PDN_DISABLE_SHIFT);

	TCOOLTHRS_value=1000;
	tmc2209_writeInt(&TMC2209, TMC2209_TCOOLTHRS, TCOOLTHRS_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK,TMC2209_EN_SPREADCYCLE_SHIFT);
	value = 0;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT ,value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK,TMC2209_EN_SPREADCYCLE_SHIFT);

//	value=10;
//	tmc2209_writeInt(&TMC2209, TMC2209_TPWMTHRS, value);
//	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_INTERNAL_RSENSE_MASK,TMC2209_INTERNAL_RSENSE_SHIFT);
	value = 0;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_INTERNAL_RSENSE_MASK, TMC2209_INTERNAL_RSENSE_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_INTERNAL_RSENSE_MASK, TMC2209_INTERNAL_RSENSE_SHIFT);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_FREQ_MASK, TMC2209_PWM_FREQ_SHIFT);
	value=1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_FREQ_MASK, TMC2209_PWM_FREQ_SHIFT, value);
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_FREQ_MASK, TMC2209_PWM_FREQ_SHIFT);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209,TMC2209_COOLCONF, TMC2209_SEIMIN_MASK, TMC2209_SEIMIN_SHIFT);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_DRVSTATUS, TMC2209_CS_ACTUAL_MASK, TMC2209_CS_ACTUAL_SHIFT);

	mstep_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK,TMC2209_MSTEP_REG_SELECT_SHIFT);	//uartでのmicrostepの有効化
	mstep_value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK, TMC2209_MSTEP_REG_SELECT_SHIFT,mstep_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	mstep_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK, TMC2209_MSTEP_REG_SELECT_SHIFT);

	toff_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT);	//保持トルクの設定
	toff_value = 3;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT,toff_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	toff_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT);

	microstep_value = 256 >> TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT);	//microstepの設定
	microstep_value = 4;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT,microstep_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	microstep_value = 256 >> TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT);

	IHOLDDELAY_value=4;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLDDELAY_MASK, TMC2209_IHOLDDELAY_SHIFT, IHOLDDELAY_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	IHOLDDELAY_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLDDELAY_MASK, TMC2209_IHOLDDELAY_SHIFT);

	stallgard_setup_value=100;											//SG_RESULTと比較して、
	tmc2209_writeInt(&TMC2209, TMC2209_SGTHRS, stallgard_setup_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	stallgard_setup_value = tmc2209_readInt(&TMC2209, TMC2209_SGTHRS);

	stealthChop_value = 100;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT, stealthChop_value);
	stealthChop_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());


	value=5;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEMIN_MASK, TMC2209_SEMIN_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	SEMAX_value = 2;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEMAX_MASK, TMC2209_SEMAX_SHIFT, SEMAX_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	TBL_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	TBL_value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT, TBL_value);
	TBL_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT);

	stealthChop_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	autoscale_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_AUTOSCALE_MASK, TMC2209_PWM_AUTOSCALE_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	PWMAuto_value = tmc2209_readInt(&TMC2209,TMC2209_PWM_AUTO);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	PWMAuto_scale = tmc2209_readInt(&TMC2209,TMC2209_PWMSCALE);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value = 0;
	value = TMC2209_FIELD_READ(&TMC2209, 0x07, TMC2209_FCLKTRIM_MASK, TMC2209_FCLKTRIM_SHIFT);

	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("TMCsetup_OK\r\n");

	end_flag=1;

}

void RMDCommand(){
//	int i = 0;
	int count=0;
	TxHeader.StdId=0x141;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 0x08;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	can_data[1] = 0x00;
	can_data[2] = 0x00;
	can_data[3] = 0x00;
	can_data[4] = 0x00;
	can_data[5] = 0x00;
	can_data[6] = 0x00;
	can_data[7] = 0x00;
	switch(Writecom){
		case 0x31:
			can_data[0] = 0x31;	//PI設定
			can_data[2] = UART1_Data[2];	//anglePidKp
			can_data[3] = UART1_Data[3];	//anglePidKi
			can_data[4] = UART1_Data[4];	//speedPidKp
			can_data[5] = UART1_Data[5];	//speedPidKi
			can_data[6] = UART1_Data[6];	//iqPidKp
			can_data[7] = UART1_Data[7];	//iqPidKi
		case 0x32:
			can_data[0] = 0x32;	//PI設定
			can_data[2] = UART1_Data[2];	//anglePidKp
			can_data[3] = UART1_Data[3];	//anglePidKi
			can_data[4] = UART1_Data[4];	//speedPidKp
			can_data[5] = UART1_Data[5];	//speedPidKi
			can_data[6] = UART1_Data[6];	//iqPidKp
			can_data[7] = UART1_Data[7];	//iqPidKi
			break;
		case 0x34:
			can_data[0] = 0x34;	//accelePI設定
			can_data[4] = UART1_Data[4];	//speedI
			can_data[5] = UART1_Data[5];	//TorqueP
			can_data[6] = UART1_Data[6];	//TorqueI
			can_data[7] = UART1_Data[7];	//PosI
			break;
		case 0xA1:
			can_data[0] = 0xA1;	//Torque設定
			can_data[4] = UART1_Data[4];	//currentlow
			can_data[5] = UART1_Data[5];	//currenthigh
			break;
		case 0xA4:
			can_data[0] = 0xA4;
			can_data[1] = 0x00;
			can_data[2] = 0x2C;	//speed low
			can_data[3] = 0x01;	//speed high
			can_data[4] = UART1_Data[4];	//poslow
			can_data[5] = UART1_Data[5];
			can_data[6] = 0x00;
			can_data[7] = 0x00;	//poshigh
			break;
		default:
			end_flag=1;
			break;
		}
	Writecom=0x00;
	switch(Readcom){
		case 0x30:
			can_data[0] = 0x30;
			break;
		case 0x33:
			can_data[0] = 0x33;
			break;
		case 0x9C:
			can_data[0] = 0x9C;
			break;
		case 0xA1:
			can_data[0] = 0xA1;
		case 0x90:
			can_data[0] = 0x90;
			break;
		default:
			end_flag=1;
			break;
	}
	Readcom=0x00;
	if(0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) ){
		HAL_CAN_AddTxMessage(&hcan1,&TxHeader,can_data,&TxMailbox);
		HAL_Delay(10);
//		printf("Can Send\r\n");
		while(!nextcan_flag){
//			printf("Mailboxes: %d\r\n",HAL_CAN_GetTxMailboxesFreeLevel(&hcan1));
			HAL_CAN_AddTxMessage(&hcan1,&TxHeader,can_data,&TxMailbox);
			HAL_Delay(10);
			count++;
			if(count==10){
				nextcan_flag=1;
				HAL_CAN_Stop (&hcan1);
				//HAL_CAN_AbortTxRequest(&hcan1,TxMailbox);
				HAL_CAN_Start(&hcan1);
			}
		}
		nextcan_flag=0;
		count=0;
	}else{
		nextcan_flag=0;
		end_flag=1;
	}
}
void SetUpRMD(){
	can_data[4] = 0x50;
	can_data[5] = 0x46;
	Writecom = 0xA4;
	RMDCommand();
}
void RMDLeftTurn(){
	can_data[4] = 0xE0;
	can_data[5] = 0x2E;
	Writecom=0xA4;
	RMDCommand();
}
void RMDRightTurn(){
	can_data[4] = 0xC0;
	can_data[5] = 0x5D;
	Writecom=0xA4;
	RMDCommand();
}
void checkRMDdata(){
	int RxBitshift[4];
	RxBitshift[0]=0;
	RxBitshift[1]=0;
	RxBitshift[2]=0;
	RxBitshift[3]=0;
	Readcom=0x30;
	RMDCommand();

	Readcom=0x33;
	RMDCommand();

	Readcom=0x9C;
	RMDCommand();
	RxBitshift[0]=RxData[2] & 0xFF;
	RxBitshift[0]+=(RxData[3] & 0xFF) << 8;
	RxBitshift[1]=RxData[4] & 0xFF;
	RxBitshift[1]+=(RxData[5] & 0xFF) << 8;
	RxBitshift[2]=RxData[6] & 0xFF;
	RxBitshift[2]+=(RxData[7] & 0xFF) << 8;

	Readcom=0x90;
	RMDCommand();
	RxBitshift[0]=RxData[2] & 0xFF;
	RxBitshift[0]+=(RxData[3] & 0xFF) << 8;
//	RxBitshift[0]=RxBitshift[0]-1;
	RxBitshift[0]=RxBitshift[0]<<2;
	RxBitshift[0]=(RxBitshift[0]>>2);
	RxBitshift[1]=RxData[4] & 0xFF;
	RxBitshift[1]+=(RxData[5] & 0xFF) << 8;
	RxBitshift[2]=RxData[6] & 0xFF;
	RxBitshift[2]+=(RxData[7] & 0xFF) << 8;
}
void RMDPIDSetting(){
	can_data[2] = 0x64;
	can_data[3] = 0x64;
	can_data[4] = 0x28;
	can_data[5] = 0xE;
	can_data[6] = 0x14;
	can_data[7] = 0x14;
	Writecom=0x31;
	RMDCommand();
}


void UART1_InIt(){
	UART1_Data[0] = 0x00;
	UART1_Data[1] = 0x00;
	UART1_Data[2] = 0x00;
	UART1_Data[3] = 0x00;
	UART1_Data[4] = 0x00;
	UART1_Data[5] = 0x00;
	UART1_Data[6] = 0x00;
	UART1_Data[7] = 0x00;
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
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
//  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)!=1){	//リミットスイッチON→待機状態
//	  home_position_state=1;
//
//  }else{
//	  home_position_state=0;
//  }
 TMC2209_INIT();
 //rxbufを受信したらフラグを
 HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
 steppers[0].dirFunc = Dir0;
 steppers[0].stepFunc = Step0;
 steppers[0].acceleration = 1000;
 steppers[0].minStepInterval = 100;
 steppers[0].homing = 0;
 steppers[0].dir_inv = 1;
 steppers[0].seeking_vel = 100;
 steppers[0].homing_vel = 200;
 steppers[0].pull_off = 500;
 steppers[0].stall_off=10000;
 get_uart_flag=0;

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
 printf("CAN_OK\r\n");
uint8_t judgment[10];
uint8_t modeflag=0;
uint8_t checkdata[10];
uint8_t mode_select;
int16_t transmit_receive_data[10];
int16_t sent_data[10];



char Mode_Type[4];
float rad_data[2];
float machine_coordinate_data[2];
float tip_coordinate_data[2];
float machine_origin_rad[10];
float machine_origin_coordinate[10];
int sent_step=0;
int next_step;
int16_t next_RMDpos;
int execution_step;
int execution_RMDpos;
machine_origin_coordinate[0]=0.0;
machine_origin_coordinate[1]=300.0;
BLDC_IK(machine_origin_coordinate[0],machine_origin_coordinate[1],&rad_data[0],&rad_data[1]);
machine_origin_rad[0]=rad_data[0];
machine_origin_rad[1]=rad_data[1];
//TMCsetup();
printf("Init_OK\r\n");

//RxData[0]=0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  get_uart_flag=0;
	  home_position_state=0;
	  HAL_UART_Receive_IT(&huart2, (uint8_t *)RxData, buffersize);
//	  HAL_UART_Transmit(&huart2, (uint8_t *)RxData, buffersize,300);
	  //受信するまで待つ
	  while(!get_uart_flag){}
	  get_can_flag=1;//canデータ受信用のフラグを0に
//	  for(int i=0;i<buffersize;i++){
//	  		  judgment[i]=RxData[i];
//	  	  }
//	  if(RxData[0]==0xAA&&RxData[(buffersize-1)]==0xAA){
//		  command_judgment(judgment);
//	  }else{
//		  selectmode_flag=2;
//		  TxData[0]=0xAA;
//		  TxData[buffersize]=0xAA;
//		  for(int i=1;i<buffersize-1;i++){
//			  TxData[i]=0xFF;
//		  	  }
//		  HAL_UART_Transmit(&huart2,TxData,sizeof(TxData),100);
//	  }
//
//	  if(selectmode_flag==1){
//		  mode_select=judgment[1];	//モード変更
//		  }
//
//	  else if(selectmode_flag==0){
//		  transmit_receive_data[0]=RxData[1]&0xFF;	//x1
//		  transmit_receive_data[0]+= (RxData[2]&0xFF)<<8;
//		  transmit_receive_data[1]=RxData[3]&0xFF;//y1
//		  transmit_receive_data[1]+= (RxData[4]&0xFF)<<8;
//		  transmit_receive_data[2]=RxData[5]&0xFF;	//x2
//		  transmit_receive_data[2]+= (RxData[6]&0xFF)<<8;
//		  transmit_receive_data[3]=RxData[7]&0xFF;	//y2
//		  transmit_receive_data[3]+= (RxData[8]&0xFF)<<8;
//		  transmit_receive_data[4]=RxData[9]&0xFF;	//z
//		  transmit_receive_data[4]+= (RxData[10]&0xFF)<<8;
//		  transmit_receive_data[5]=RxData[11]&0xFF;	//a1
//		  transmit_receive_data[5]+= (RxData[12]&0xFF)<<8;
//		  transmit_receive_data[6]=RxData[13]&0xFF;	//a2
//		  transmit_receive_data[6]+= (RxData[14]&0xFF)<<8;
//		  transmit_receive_data[7]=RxData[15]&0xFF;	//xl1
//		  transmit_receive_data[7]+= (RxData[16]&0xFF)<<8;
//		  transmit_receive_data[8]=RxData[17]&0xFF;	//l2
//		  transmit_receive_data[8]+= (RxData[18]&0xFF)<<8;
//
//
//		  machine_coordinate_data[0]=(float)transmit_receive_data[2];	//x2をfloatにする
//		  machine_coordinate_data[1]=(float)transmit_receive_data[3];	//y2をfloatにする
//		  BLDC_IK(machine_coordinate_data[0],machine_coordinate_data[1],&rad_data[0],&rad_data[1]);
//		  transmit_receive_data[5]=transmit_receive_data[5]*10;
//		  transmit_receive_data[6]=transmit_receive_data[6]*10;
//	  }
	  mode_select= RxData[0];
	  if(home_position_state==1){	//初期位置モード
		  switch(mode_select){
		  case 0:
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
//				  transmit_receive_data[0] = RxData[0]&0xFF;
//				  transmit_receive_data[0] += (RxData[1]&0xFF)<<8;
//				  transmit_receive_data[1] = RxData[2]&0xFF;
//				  transmit_receive_data[1] += (RxData[3]&0xFF)<<8;
//				  machine_origin_coordinate[0]=(float)transmit_receive_data[0];
//				  machine_origin_coordinate[1]=(float)transmit_receive_data[1];
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

		  switch(mode_select){
		  printf("command_mode\r\n");
		  case '1':	//モーターON
			  canid=0x01;
			  EnterMotorMode();
			  BLDC_CANTx();
			  canid=0x02;
			  EnterMotorMode();
			  BLDC_CANTx();
			  printf("Enter motor\r\n");

			  break;
		  case '2':	//モーターOFF
			  canid=0x01;
			  ExitMotorMode();
			  BLDC_CANTx();
			  canid=0x02;
			  ExitMotorMode();
			  BLDC_CANTx();
			  break;
		  case '3':	//動作モード
			  if(selectmode_flag==0){
				  t_position[0] = rad_data[0];
				  t_position[1] = rad_data[1];
				  BLDC_MotorRotate();

//				  sent_step=(int)transmit_receive_data[4];
//				  sent_step=(sent_step/10)*3200;
//				  prepareAbsoluteMovement(sent_step);
//				  runAndWait();
			  }
			  break;
		  case '4':
			  t_position[0] = 0;
			  t_position[1] = 0;
			  BLDC_M2Rotate();
			  BLDC_M1Rotate();
			  t_position[0] = machine_origin_rad[0];	//スタンバイ状態
			  t_position[1] = machine_origin_rad[1];
			  BLDC_MotorRotate();
			  break;
		  case '5':	//初期状態へ戻る
			  t_position[0] = 0;
			  t_position[1] = M_PI;
			  BLDC_MotorRotate();
			  break;
		  case '6':	//腕伸ばした状態
			  t_position[0] = 0;
			  t_position[1] = 0;
			  BLDC_MotorRotate();
			  break;
		  case '7':	//ティーチングモード開始
			  canid=0x01;
			  pack_cmd(0.0, 0.0, 0.0, 1.0, 0.0);
			  BLDC_CANTx();
			  canid=0x02;
			  pack_cmd(0.0, 0.0, 0.0, 1.0, 0.0);
			  BLDC_CANTx();
			  break;
		  case '8':	//ティーチング決定
			  canid=0x01;
			  pack_cmd(0.0, 0.0, 0.0, 2.0, 0.0);
			  BLDC_CANTx();
			  unpack_reply();
			  pack_cmd(p_getdata, 0.0, 100.0, 3.0, 0.0);
			  BLDC_CANTx();
			  unpack_reply();
			  rad_data[0]=p_getdata;
			  canid=0x02;
			  pack_cmd(0.0, 0.0, 0.0, 2.0, 0.0);
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
		  case '9':
			  can_data[0] = 0xA4;
			  can_data[2] = 0x2C;	//speed low
			  can_data[3] = 0x01;	//speed high
			  can_data[4] =(transmit_receive_data[5]&0xFF);	//poslow
			  can_data[5] =(transmit_receive_data[5]>>8)&0xFF;
			  RMDTx();
			  sent_step=(int)transmit_receive_data[4];
			  sent_step=(sent_step/10)*3200;

			  prepareAbsoluteMovement(sent_step);
			  runAndWait();
			  break;
		  case 'a':
			  next_RMDpos+=3600;

			  RMDTx_can_data[0] = 0xA4;
			  RMDTx_can_data[1] = 0x00;
			  RMDTx_can_data[2] = 0x2C;	//speed low
			  RMDTx_can_data[3] = 0x01;	//speed high
			  RMDTx_can_data[4] =next_RMDpos&0xFF;	//poslow
			  RMDTx_can_data[5] =next_RMDpos>>8;
			  RMDTx_can_data[6] = 0x00;
			  RMDTx_can_data[7] = 0x00;
			  RMDTx();
			  sent_step+=10;
			  execution_step=(sent_step/10)*3200;
			  printf("sent_step=%d\r\n",execution_step);
			  prepareAbsoluteMovement(execution_step);
			  runAndWait();
			  break;
		  case 'b':
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
			  HAL_UART_Transmit(&huart2,TxData,sizeof(TxData),100);

			  break;
		  case 'c':
			  sent_step=10;
			  sent_step=(sent_step/10)*3200;
			  printf("sent_step=%d\r\n",sent_step);
			  prepareAbsoluteMovement(sent_step);
			  runAndWait();
			  HAL_Delay(100);
			  break;
		  case 'd':
			  sent_step=0;
			  sent_step=(sent_step/10)*3200;
			  printf("sent_step=%d\r\n",sent_step);
			  prepareAbsoluteMovement(sent_step);
			  runAndWait();
			  break;
		  case 's':
			  printf("TMCsetup\r\n");

			  TMCsetup();
			  break;
		  default:
				  get_can_flag=1;
				  break;
			  }

		  }
	  home_position_state=0;

	  get_uart_flag=0;
	  while(!get_can_flag){}
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
  htim3.Init.Prescaler = 160-1;
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
  huart1.Init.BaudRate = 500000;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
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
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

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

  /*Configure GPIO pins : PA7 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9;
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
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void BLDC_FK(float s1,float s2,float* xp,float* yp){
	float x=0.0;
	float y=0.0;
	x=l1*cos(s1)+l2*cos(s1+s2);
	y=l1*sin(s1)+l2*sin(s1+s2);
	*xp=x;
	*yp=y;
}
void BLDC_IK(float x,float y,float* s1p,float* s2p){
	float s1=0.0;
	float s2=0.0;
	s2=-acos((x*x+y*y-l1*l1-l2*l2)/(2*l1*l2));
	s1=atan2((-l2*sin(s2)*x+(l1+l2*cos(s2))*y),((l1+l2*cos(s2))*x+l2*sin(s2)*y));
	*s1p=s1;
	*s2p=s2;
}
int _write(int file, char *ptr, int len)	//printfに必要
{
  HAL_UART_Transmit(&huart2,(uint8_t *)ptr,len,10);
  return len;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)
{
	get_uart_flag=1;
//	HAL_UART_Transmit(&huart2, (uint8_t *)RxData, buffersize,10);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)RxData, buffersize);
//	command_judgment();
//	if(RxData[1]==0x63&&selectmode_flag==1){
//		motion_stop_flag=1;
//	}

}
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
//	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1);
	  nextcan_flag=1;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	//バッファー設けてどのcanidから送られて来たのかを判定
	get_can_flag=1;
	nextcan_flag=1;
//	printf("RxCan\r\n");
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0, &RxHeader, Rx_can_data);
//	printf("StdId:%ld\r\n",RxHeader.StdId);
//	printf("Rx_can_data[0]:%d\r\n",Rx_can_data[0]);

	if(Rx_can_data[0]==0x01){
//		printf("m1can_get\r\n");
		memcpy(m1Rx_can_data,Rx_can_data,sizeof(Rx_can_data));
	}else if(Rx_can_data[0]==0x02){
//		printf("m2can_get\r\n");
		memcpy(m2Rx_can_data,Rx_can_data,sizeof(Rx_can_data));
	}else if(RxHeader.StdId==0x141){
//		printf("RMDcan_get\r\n");
		memcpy(RMDRx_can_data,Rx_can_data,sizeof(Rx_can_data));
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//Arm home position
	if(GPIO_Pin == GPIO_PIN_10 ){
		if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)){	//end stop ONの時フラグを立てる
			home_position_state |= 1;
		}
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)){
			home_position_state &= ~1;
		}
	}
	//stepper 0 end stop sensor
	if(GPIO_Pin == GPIO_PIN_6 ){
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){
			zhome_endstop_state |= 1;
				}
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){
			zhome_endstop_state &= ~1;
				}
			}
	//stall guard
//	if(GPIO_Pin == GPIO_PIN_7 ){
//		if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)){
//			current_stall|= 0;
//		}
//		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)){
//			current_stall = 1;
////			stallguard_error();
//		}
//	}
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
