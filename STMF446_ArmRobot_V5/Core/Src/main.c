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
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<string.h>
#include "math_ops.h"
#include "leg_message.h"
#include "tmc/ic/TMC2209/TMC2209.h"
#include "core_cm4.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct{
	uint8_t coordinate_state;	//座標入力管理
	uint8_t coordinate_run;	//座標入力した値が実行されたかどうか
	uint8_t mode_state;	//コマンド管理
	uint8_t error_state;	//エラー管理
	uint8_t mode_wait_flag;	//そのモードに入力される座標待機
	uint8_t run_state;	//モーターONかOFFか（動作中か停止中か）のステータス
	uint8_t stanby_state;	//待機位置にいるかどうかのステータス
	uint8_t fc;
	uint8_t lc;
	uint8_t cmd;
	int16_t x1;
	int16_t x2;
	int16_t y1;
	int16_t y2;
	uint16_t z;
	uint16_t a1;
	uint16_t a2;
	uint16_t l1;
	uint16_t l2;
	uint8_t s;
}UART_data;

typedef struct bldc_motor{
	uint32_t can_id; //motor can id
	float position; //motor angle
	float velocity;
	float target_position; // target motor angle
	float target_val;
	float temporary_position ;
	float target_velocity;
	float kp;
	float kd;
	float max_velocity; //最大速度
	float reply_position;
	float distance;
	float next_position;
	float dir;
	float first_position;
	float count_distance;
	uint8_t state_change;
	uint8_t state;
	uint8_t txcan_data[8];
	uint8_t flag;
	uint8_t end_state;
}BLDC_Motor;

typedef struct{
	uint32_t can_id; //motor can id
	int position; //motor angle
	int target_position; //target motor angle
	int max_velocity;
	int offset;
	int zero;
	int a1;
	int a2;
	float firstpos;
	float rmdtilt;
	uint8_t state;
	uint8_t motion_state;
	uint8_t offset_state;
	uint8_t PID_state;
	uint8_t txcan_data[8];
	uint16_t reply;
	uint16_t comparison;
	uint16_t target_val;
	uint8_t end_state;
}RMD_Motor;

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

	volatile int sending_step;
}stepperInfo;

typedef struct robot_arm{
	BLDC_Motor axis[2];	//アームBLDC
	RMD_Motor axis2;	//RMD
	//各モータの送信角度を目標角度と最大角度から計算して送信する
	void (*SendAll)(struct robot_arm*);

}RobotArm;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NUM_STEPPERS 1
#define NUM_BLDCS 2
#define TMC2209_CRC(data, length) tmc_CRC8(data, length, 1)
#define CAN_LOOP_PERIOD_MS 10.0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//通信の設定//
volatile UART_data data;
uint8_t RxData[20] = {0};
uint8_t TxData[20] = {0};
uint8_t canRxdata[8] = {0};
uint8_t BLDC_canRxdata[6] = {0};
uint8_t get_uart_flag = 0;
uint8_t buffersize = 20;
uint8_t motion_stop_flag = 0;
uint8_t flag_count = 0;

//BLDCの設定//
CAN_FilterTypeDef sFilterConfig;
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint32_t TxMailbox;
BLDC_Motor BLDCS[NUM_BLDCS];
BLDC_Motor rb;
float l1=250.0;
float l2=350.0;
uint8_t count_flag=0;
float bldc_counter=0.0;
float pid_count=0.0;
//RMDの設定//

uint8_t RMDRx_can_data[8]={0}; //RMDの受信データ
RMD_Motor rd;
//ステッピングモータの設定//
volatile stepperInfo steppers[NUM_STEPPERS];
uint8_t zhome_endstop_state = 0;
uint8_t homing_flag = 0;
TMC2209TypeDef TMC2209; //TMC2209の設定などの定義
ConfigurationTypeDef TMC2209_config; //TMC2209の設定
//ロボットアームデータ//
RobotArm ra;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_CAN2_Init(void);
static void MX_TIM2_Init(void);
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
//BLDCの設定//
#define Motion_KP 50.0
#define MAX_KP 250.0
#define Motion_KI 3.0
#define count_rad 0.05
#define origin_x 0.0
#define origin_y 300.0
void SendAll(RobotArm* robot_arm);
void EnterMotorMode(uint8_t *can_data);
void ExitMotorMode(uint8_t *can_data);
void pack_cmd(BLDC_Motor* bldc,float _p_des, float _v_des, float _kp, float _kd, float _t_ff);
void unpack_reply(BLDC_Motor* bldc);
void BLDC_FK(float s1,float s2,float* xp,float* yp);
void BLDC_IK(float x,float y,float* s1p,float* s2p);
void sent_position(RobotArm* robot_arm);
//ステッピングモータの設定//
static uint8_t reset(void);
static uint8_t restore(void);
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
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//ここからUART//
void sent_position(RobotArm* robot_arm){
	float current_coordinate[2];
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;
	BLDC_FK(-(robot_arm->axis[0].position),-((robot_arm->axis[1].position)-M_PI),&current_coordinate[0],&current_coordinate[1]);

	x1=round(l1*cos(-robot_arm->axis[0].position));
	y1=round(l1*sin(-robot_arm->axis[0].position));
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
//	printf("x1:%d\r\n",x1);
//	printf("y1:%d\r\n",y1);
//	printf("x2:%d\r\n",x2);
//	printf("y2:%d\r\n",y2);
}
//ここからBLDC//
void BLDC_CANTx(BLDC_Motor* bldc){	//BLDCにcanを送る
	TxHeader.StdId=bldc->can_id;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 0x08;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	//HAL_CAN_GetTxMailboxesFreeLevelがあくまで待機
//	printf("StdId:%ld\r\n",robot_arm->axis[whichBLDC].can_id);
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan2)<3){}
	HAL_CAN_AddTxMessage(&hcan2,&TxHeader,bldc->txcan_data,&TxMailbox);
	HAL_Delay(10);
}

//ここからRMD//
void RMD_CANTx(RMD_Motor* rmd){	//BLDCにcanを送る
	uint8_t complete_flag=0;
	TxHeader.StdId=0x141;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 0x08;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)<3){}
	HAL_CAN_AddTxMessage(&hcan1,&TxHeader,rmd->txcan_data,&TxMailbox);
	HAL_Delay(10);
//	printf("txcan_data[0]:%x\r\n",robot_arm->axis2.txcan_data[0]);
//		printf("RMD_Still_start!\r\n");
}
void BLDC_calculatuin(RobotArm* robot_arm){
	robot_arm->axis[0].target_val  = -(robot_arm->axis[0].target_position);
	robot_arm->axis[1].target_val=(-(robot_arm->axis[1].target_position-M_PI));
	robot_arm->axis[0].first_position=robot_arm->axis[0].position;
	robot_arm->axis[1].first_position=robot_arm->axis[1].position;
	robot_arm->axis[0].distance = fabsf(robot_arm->axis[0].target_val - robot_arm->axis[0].first_position);
	robot_arm->axis[1].distance = fabsf(robot_arm->axis[1].target_val - robot_arm->axis[1].first_position);
	robot_arm->axis[0].dir = ((robot_arm->axis[0].target_val - robot_arm->axis[0].position)>0)?1:-1;
	robot_arm->axis[1].dir = ((robot_arm->axis[1].target_val - robot_arm->axis[1].position)>0)?1:-1;
	robot_arm->axis[0].flag=0;
	robot_arm->axis[1].flag=0;
	robot_arm->axis[0].end_state = 0;
	robot_arm->axis[1].end_state = 0;

	bldc_counter=0;
	pid_count=0;
	printf("axis[0]first_position : %lf\r\n",robot_arm->axis[0].first_position);
	printf("axis[1]first_position : %lf\r\n",robot_arm->axis[1].first_position);
	printf("axis[0]target_position : %lf\r\n",robot_arm->axis[0].target_val);
	printf("axis[1]target_position : %lf\r\n",robot_arm->axis[1].target_val);
	robot_arm->axis[0].state=1;
	robot_arm->axis[1].state=1;
}
void RMD_calculatuin(RobotArm* robot_arm){
	robot_arm->axis2.state = 1;
	robot_arm->axis2.end_state = 0;
	if(robot_arm->axis2.motion_state==1){
		int motor_deg;
//		int axis2_l;
		motor_deg=(int)(((-(robot_arm->axis[0].position)+(-(robot_arm->axis[1].position)+M_PI))*180)/M_PI);	//RMDをどれだけ回せば垂直になるか
		robot_arm->axis2.zero=(int)round(robot_arm->axis2.firstpos+100*motor_deg);	//RMDの垂直ポディション
		printf("motor_deg:%d\r\n",motor_deg);
		printf("RMDzero:%d\r\n",robot_arm->axis2.zero);
//		robot_arm->axis2.zero=9000;
		printf("input_target_position:%d\r\n",robot_arm->axis2.target_position);
		robot_arm->axis2.target_position = robot_arm->axis2.zero+(-90*100) + robot_arm->axis2.target_position;	//垂直から90deg引いた値、水平を0degとしてそこから入力された角度だけRMDを回転させる
		printf("target_position:%d\r\n",robot_arm->axis2.target_position);
		robot_arm->axis2.txcan_data[0] = 0xA4;
		robot_arm->axis2.txcan_data[2] = 0x64;	//speed low
		robot_arm->axis2.txcan_data[3] = 0x00;
		robot_arm->axis2.txcan_data[4] =(robot_arm->axis2.target_position&0xFF);	//poslow
		robot_arm->axis2.txcan_data[5] =(robot_arm->axis2.target_position>>8)&0xFF;
		robot_arm->axis2.motion_state=0;
	}else if(robot_arm->axis2.PID_state==1){
		robot_arm->axis2.txcan_data[0] = 0x32;	//PI設定
		robot_arm->axis2.txcan_data[2] = 0x64;	//anglePidKp
		robot_arm->axis2.txcan_data[3] = 0xC8;	//anglePidKi
		robot_arm->axis2.txcan_data[4] = 0x64;	//speedPidKp
		robot_arm->axis2.txcan_data[5] = 0x14;	//speedPidKi
		robot_arm->axis2.txcan_data[6] = 0x50;	//iqPidKp
		robot_arm->axis2.txcan_data[7] = 0x1E;	//iqPidKi
		robot_arm->axis2.PID_state=0;
	}else if(robot_arm->axis2.offset_state==1){
		robot_arm->axis2.txcan_data[0] = 0x91;
		robot_arm->axis2.txcan_data[6] = robot_arm->axis2.offset&0xFF;	//position_offset
		robot_arm->axis2.txcan_data[7] = robot_arm->axis2.offset>>8;
		robot_arm->axis2.offset_state=0;
	}
}
//canにて全ての関節のデータを送信//
void SendAll(RobotArm* robot_arm)
{
	if(!(motion_stop_flag)){
		if(robot_arm->axis[0].state_change == 1 && robot_arm->axis[1].state_change == 1){	//モーターONorOFF時に使用
			if(robot_arm->axis[0].state == 1){	//アームモーター1のON/OFF
				EnterMotorMode(robot_arm->axis[0].txcan_data);
			}else{
				ExitMotorMode(robot_arm->axis[0].txcan_data);
			}
			robot_arm->axis[0].can_id=0x01;
			BLDC_CANTx(&robot_arm->axis[0]);
			if(robot_arm->axis[1].state == 1){	//アームモーターのON/OFF
				EnterMotorMode(robot_arm->axis[1].txcan_data);
			}else{
				ExitMotorMode(robot_arm->axis[1].txcan_data);
			}
			robot_arm->axis[1].can_id=0x02;
			BLDC_CANTx(&robot_arm->axis[1]);
			robot_arm->axis[0].state = 0;
			robot_arm->axis[1].state = 0;
			robot_arm->axis[0].state_change = 0;
			robot_arm->axis[1].state_change = 0;
			robot_arm->axis[0].end_state = 1;
			robot_arm->axis[1].end_state = 1;
		}else if(robot_arm->axis[0].state == 1 && robot_arm->axis[1].state == 1){	//M1とM2を動かす時に使用
			float far_dis,near_dis;
			float far_firstpos,near_firstpos;
			uint32_t BLDC_select_far,BLDC_select_near;
			float far_dis_dir,near_dis_dir;
			if(robot_arm->axis[0].distance  > robot_arm->axis[1].distance ){
				BLDC_select_far=0;
				BLDC_select_near=1;
			}else if(robot_arm->axis[0].distance  < robot_arm->axis[1].distance ){
				BLDC_select_far=1;
				BLDC_select_near=0;
			}
			far_dis=robot_arm->axis[BLDC_select_far].distance;
			near_dis=robot_arm->axis[BLDC_select_near].distance;
			far_dis_dir=robot_arm->axis[BLDC_select_far].dir;
			near_dis_dir=robot_arm->axis[BLDC_select_near].dir;
			far_firstpos=robot_arm->axis[BLDC_select_far].first_position;
			near_firstpos=robot_arm->axis[BLDC_select_near].first_position;
			if (far_dis>(float)count_rad){
				bldc_counter+=(far_dis_dir*(float)count_rad);
				if(fabsf(bldc_counter)<far_dis){
					pack_cmd(&robot_arm->axis[BLDC_select_far],far_firstpos+bldc_counter, 0.0, Motion_KP, Motion_KI, 0.0);	//現在の場所の所得
					BLDC_CANTx(&robot_arm->axis[BLDC_select_far]);
					unpack_reply(&robot_arm->axis[BLDC_select_far]);
					pack_cmd(&robot_arm->axis[BLDC_select_near],near_firstpos+fabsf(bldc_counter)*(near_dis/far_dis)*near_dis_dir, 0.0, Motion_KP, Motion_KI, 0.0);	//現在の場所の所得
					BLDC_CANTx(&robot_arm->axis[BLDC_select_near]);
					unpack_reply(&robot_arm->axis[BLDC_select_near]);
					sent_position(robot_arm);
				}else{
					robot_arm->axis[0].flag= 1;
					robot_arm->axis[1].flag= 1;
				}
			}else{
				robot_arm->axis[0].flag= 1;
				robot_arm->axis[1].flag= 1;
			}
			if(robot_arm->axis[0].flag==1&&robot_arm->axis[1].flag==1){
				pid_count++;
				if((Motion_KP+pid_count*10 <= MAX_KP) && !motion_stop_flag){
					pack_cmd(&robot_arm->axis[0],robot_arm->axis[0].target_val, 0.0, Motion_KP+pid_count*10, Motion_KI, 0.0);	//現在の場所の所得
					BLDC_CANTx(&robot_arm->axis[0]);
					unpack_reply(&robot_arm->axis[0]);
					pack_cmd(&robot_arm->axis[1],robot_arm->axis[1].target_val, 0.0, Motion_KP+pid_count*10, Motion_KI, 0.0);	//現在の場所の所得
					BLDC_CANTx(&robot_arm->axis[1]);
					unpack_reply(&robot_arm->axis[1]);
				}else{
					sent_position(robot_arm);
					robot_arm->axis[0].state = 0;
					robot_arm->axis[1].state = 0;
					robot_arm->axis[0].end_state = 1;
					robot_arm->axis[1].end_state = 1;
				}
			}
		}else if(robot_arm->axis2.state==1){	//RMDの動作
			robot_arm->axis2.reply=((RMDRx_can_data[6] & 0xFF)|((RMDRx_can_data[7] & 0xFF) << 8));
			if(robot_arm->axis2.txcan_data[0]==0xA4){
				robot_arm->axis2.comparison = (int)round((robot_arm->axis2.reply/65535.0)*36000.0);	//返ってきたエンコーダー値をdegに変換
				robot_arm->axis2.target_val = robot_arm->axis2.target_position;	//0x91使用する場合
				if(robot_arm->axis2.target_val >= 36000){
					robot_arm->axis2.comparison=robot_arm->axis2.comparison+36000;
				}
	//			robot_arm->axis2.target_val=robot_arm->axis2.target_position + robot_arm->axis2.firstpos + robot_arm->axis2.offset;	//0x91使用しない場合
		//		printf("rmddis:%d\r\n",robot_arm->axis2.target_val-robot_arm->axis2.comparison);
//				printf("motion_target_val : %d\r\n",robot_arm->axis2.target_val);
//				printf("motion_comparison : %d\r\n",robot_arm->axis2.comparison);
			}else if(robot_arm->axis2.txcan_data[0]==0x91){
				robot_arm->axis2.comparison = robot_arm->axis2.reply;
				robot_arm->axis2.target_val = robot_arm->axis2.offset;
				printf("offset_target_val : %d\r\n",robot_arm->axis2.target_val);
				printf("offset_comparison : %d\r\n",robot_arm->axis2.comparison);
			}
			RMD_CANTx(&robot_arm->axis2);
			if((abs(robot_arm->axis2.target_val - robot_arm->axis2.comparison)<10)){
				robot_arm->axis2.state=0;
				robot_arm->axis2.end_state=1;
				robot_arm->axis2.motion_state=0;
				robot_arm->axis2.offset_state=0;
				memset(robot_arm->axis2.txcan_data, 0, sizeof(robot_arm->axis2.txcan_data));
				printf("RMD_OK!\r\n");
			}else{
	//			printf("RMD_running!\r\n");

			}
		}
	}else{
		robot_arm->axis[0].state = 0;
		robot_arm->axis[1].state = 0;
		robot_arm->axis2.state=0;
		robot_arm->axis[0].end_state = 1;
		robot_arm->axis[1].end_state = 1;
		robot_arm->axis2.end_state=1;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2){
    	RobotArm *robot_arm= &ra;
    	SendAll(robot_arm);
    }
}

//BLDC ON
void EnterMotorMode(uint8_t *can_data){
	can_data[0] = 0xFF;
	can_data[1] = 0xFF;
	can_data[2] = 0xFF;
	can_data[3] = 0xFF;
	can_data[4] = 0xFF;
	can_data[5] = 0xFF;
	can_data[6] = 0xFF;
	can_data[7] = 0xFC;
}

//BLDC OFF
void ExitMotorMode(uint8_t *can_data){
	can_data[0] = 0xFF;
	can_data[1] = 0xFF;
	can_data[2] = 0xFF;
	can_data[3] = 0xFF;
	can_data[4] = 0xFF;
	can_data[5] = 0xFF;
	can_data[6] = 0xFF;
	can_data[7] = 0xFD;
}
//void pack_cmd(CANMessage * msg, float _p_des, float _v_des, float _kp, float _kd, float _t_ff){
void pack_cmd(BLDC_Motor* bldc,float _p_des, float _v_des, float _kp, float _kd, float _t_ff){
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
	bldc->txcan_data[0] = p_int>>8;
	bldc->txcan_data[1] = p_int&0xFF;
	bldc->txcan_data[2] = v_int>>4;
	bldc->txcan_data[3] = ((v_int&0xF)<<4)|(kp_int>>8);
	bldc->txcan_data[4] = kp_int&0xFF;
	bldc->txcan_data[5] = kd_int>>4;
	bldc->txcan_data[6] = ((kd_int&0xF)<<4)|(t_int>>8);
	bldc->txcan_data[7] = t_int&0xff;

//	if(whichBLDC==0){
//		si->can_id=0x01;
//	}else if(whichBLDC==1){
//		si->can_id=0x02;
//	}
}

void unpack_reply(BLDC_Motor* bldc){
//	RobotArm* robot_arm;
    /// unpack ints from can buffer ///
    uint16_t id = BLDC_canRxdata[0];
    uint16_t p_int = (BLDC_canRxdata[1]<<8)|BLDC_canRxdata[2];
    uint16_t v_int = (BLDC_canRxdata[3]<<4)|(BLDC_canRxdata[4]>>4);
    uint16_t i_int = ((BLDC_canRxdata[4]&0xF)<<8)|BLDC_canRxdata[5];
    /// convert uints to floats ///
    float p = uint_to_float(p_int, P_MIN, P_MAX, 16);
    float v = uint_to_float(v_int, V_MIN, V_MAX, 12);
    float t = uint_to_float(i_int, -T_MAX, T_MAX, 12);

    bldc->position=p;
}

//ここからステッピングモータ//


void Step0(){
	//pull+
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	/*
	 * pull-
	 * HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
	 * HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	 */
}

void Dir0(int dir){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, dir);
}

void TMCsetup(){	//TMCレジスタ設定
	int32_t value = 0;
	value = 26;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT, value);	//実行電流
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value= 20;																						//モーター待機時の電流の設定
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK,TMC2209_PDN_DISABLE_SHIFT);
	value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK, TMC2209_PDN_DISABLE_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value=500;											//SG_RESULTと比較して、
	tmc2209_writeInt(&TMC2209, TMC2209_SGTHRS, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value=2000;
	tmc2209_writeInt(&TMC2209, TMC2209_TCOOLTHRS, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value = 0;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT ,value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

//	value=10;
//	tmc2209_writeInt(&TMC2209, TMC2209_TPWMTHRS, value);
//	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value=0;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_INTERNAL_RSENSE_MASK, TMC2209_INTERNAL_RSENSE_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value=1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_FREQ_MASK, TMC2209_PWM_FREQ_SHIFT, value);

	value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK, TMC2209_MSTEP_REG_SELECT_SHIFT,value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value = 3;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT,value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value = 4;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT,value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value=4;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLDDELAY_MASK, TMC2209_IHOLDDELAY_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value = 100;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value=5;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEMIN_MASK, TMC2209_SEMIN_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value = 2;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEMAX_MASK, TMC2209_SEMAX_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

	printf("TMC_setup_OK\r\n");

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
void resetStepperInfo(stepperInfo * si){
	si->n = 0;
	si->d = 0;
	si->di = 0;
	si->stepCount = 0;
	si->rampUpStepCount = 0;
	si->totalSteps = 0;
	si->stepPosition = 0;
	si->movementDone = 0;
}
void resetStepper(volatile stepperInfo* si){
	si->c0 = si->acceleration;
	si->d = si->c0;
	si->di = si->d;
	si->stepCount = 0;
	si->n = 0;
	si->rampUpStepCount = 0;
	si->movementDone = 0;
}

volatile uint8_t remainingSteppersFlag = 0;

void prepareMovement(int whichMotor, int steps){
	if(steps == 0){
		return;
	}
	volatile stepperInfo* si = &steppers[whichMotor];
	if(si->dir_inv){
		si->dirFunc( steps < 0 ? 0 : 1);
		si->dir = steps > 0 ? -1:1;
	}else{
		si->dirFunc( steps < 0 ? 1 : 0);
		si->dir = steps > 0 ? 1:-1;
	}
	si->totalSteps = abs(steps);
	resetStepper(si);
	remainingSteppersFlag |= (1 << whichMotor);
}

void prepareAbsoluteMovement(int whichMotor, int absolute_steps){
	volatile stepperInfo* si = &steppers[whichMotor];
	int steps = absolute_steps - si->stepPosition;
	if(steps == 0){
		return;
	}
	si->dirFunc( steps < 0 ? 1 : 0);
	si->dir = steps > 0 ? 1:-1;
	si->totalSteps = abs(steps);
	resetStepper(si);
	remainingSteppersFlag |= (1 << whichMotor);
}


volatile uint8_t nextStepperFlag = 0;

void setNextInterruptInterval(){

	unsigned int mind = 999999;
	for (int i = 0; i < NUM_STEPPERS; i++){
		if( ((1 << i) & remainingSteppersFlag) && steppers[i].di < mind ){
			mind = steppers[i].di;
		}
	}

	nextStepperFlag = 0;
	for (int i = 0; i < NUM_STEPPERS; i ++){
		if( ((1 << i) & remainingSteppersFlag) && steppers[i].di == mind )
			nextStepperFlag |= (1 << i);
	}

	if (remainingSteppersFlag == 0){
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65500);
	}

	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, mind);

}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	//HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_1);
	unsigned int tmpCtr = __HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65500);


	for (int i = 0; i < NUM_STEPPERS; i++){
		if ( ! ( (1 << i) & remainingSteppersFlag )){
			continue;
		}

		if( homing_flag & (1 << i)){
			if(zhome_endstop_state & (1 << i)){
				remainingSteppersFlag &= ~(1 << i);
				continue;
			}
		}

		if ( ! (nextStepperFlag & (1 << i)) ){
			steppers[i].di -= tmpCtr;
			continue;
		}

		volatile stepperInfo* s = &steppers[i];

		if( (s->stepCount) < (s->totalSteps)){
			s->stepFunc();
			s->stepCount++;
			s->stepPosition += s->dir;
			if ( (s->stepCount) >= (s->totalSteps) ){
				s->movementDone = 1;
				remainingSteppersFlag &= ~(1 << i);
			}
		}

		if (s->rampUpStepCount == 0){
			s->n++;
			s->d = s->d - (2*s->d) / (4*s->n +1);
			if (s->d <= s->minStepInterval ){
				s->d = s->minStepInterval;
				s->rampUpStepCount = s->stepCount;
			}
			if (s->stepCount >= (s->totalSteps / 2) ){
				s->rampUpStepCount = s->stepCount;
			}
		} else if ( s->stepCount >= s->totalSteps - s->rampUpStepCount) {
			s->d = (s->d * (4 * s->n + 1)) / (4 * s->n + 1 -2);
			s->n--;
		}
		s->di = s->d;
	}

	setNextInterruptInterval();

	__HAL_TIM_SET_COUNTER(&htim3, 0);
}
//
//void sensorStateInit(){
//	if( ! HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9)){
//		zhome_endstop_state |= 1;
//	}
//}

void runAndWait(){
	setNextInterruptInterval();
	while(remainingSteppersFlag);
}

void stepperHoming(int whichMotor){
	homing_flag |= (1 << whichMotor);
	//seeking
	steppers[whichMotor].minStepInterval = steppers[whichMotor].seeking_vel;
	prepareMovement(whichMotor, -100000000);
	runAndWait();
	HAL_Delay(500);
	//pull-off

	homing_flag &= ~(1 << whichMotor);
	prepareMovement(whichMotor, steppers[whichMotor].pull_off);
	runAndWait();
	HAL_Delay(500);
	//homing

	homing_flag |= (1 << whichMotor);
	steppers[whichMotor].minStepInterval = steppers[whichMotor].homing_vel;
	prepareMovement(whichMotor, -100000000);
	runAndWait();
	HAL_Delay(500);
	//pull-off
	homing_flag &= ~(1 << whichMotor);
	prepareMovement(whichMotor, steppers[whichMotor].pull_off);
	runAndWait();
	HAL_Delay(500);

	steppers[whichMotor].stepPosition = 0;
	steppers[whichMotor].homing = 1;
	steppers[whichMotor].minStepInterval = 100;
}

//ここからコミュニケーション//
void ReceiveData_Transmit(uint8_t *rd){
	volatile UART_data* ud = &data;
	volatile stepperInfo* si = &steppers[0];
	RobotArm *robot_arm = &ra;
	uint8_t com_select[20];
	int count=0;
	 for(int i=0;i<buffersize;i++){
		 com_select[i]=~(rd[i]);
		  	}
	for(uint8_t i=0; i<buffersize ;i++){
		if(com_select[i]==0){
			count++;
		}
	}
	if(rd[0]!=0xAA && rd[(buffersize-1)]!=0xAA){
		ud->error_state=1;
	}
	else if(count<(buffersize-3)){	//動作モード,各パラメータに数値を入れる
		ud->fc=rd[0]&0xFF;
		ud->x1=(rd[1]&0xFF)|((rd[2]&0xFF)<<8);
		ud->y1=(rd[3]&0xFF)|((rd[4]&0xFF)<<8);
		ud->x2=(rd[5]&0xFF)|((rd[6]&0xFF)<<8);
		ud->y2=(rd[7]&0xFF)|((rd[8]&0xFF)<<8);
		ud->z=(rd[9]&0xFF)|((rd[10]&0xFF)<<8);
		ud->a1=(rd[11]&0xFF)|((rd[12]&0xFF)<<8);
		ud->a2=(rd[13]&0xFF)|((rd[14]&0xFF)<<8);
		ud->l1=(rd[15]&0xFF)|((rd[16]&0xFF)<<8);
		ud->l2=(rd[17]&0xFF)|((rd[18]&0xFF)<<8);
		ud->lc=rd[19]&0xFF;
		ud->coordinate_state=1;
		BLDC_IK(ud->x2,ud->y2,&robot_arm->axis[0].target_position,&robot_arm->axis[1].target_position);
		si->sending_step=(ud->z/10)*1600;	//1/10してmm/revに変換
		robot_arm->axis2.a1=ud->a1*10;	//10倍して36000deg/revに変換
		robot_arm->axis2.a2=ud->a2*10;	//10倍して36000deg/revに変換
		printf("x: %d\r\n",ud->x2);
		printf("y: %d\r\n",ud->y2);
		printf("a1: %d\r\n",robot_arm->axis2.a1);
		printf("a2: %d\r\n",robot_arm->axis2.a2);
	}
	else if(count==(buffersize-3)){	//コマンドモード,9byte com_selectが0だったら
		ud->cmd=rd[1]&0xFF;
		ud->mode_state=1;
	}
	switch(ud->cmd){
	case 0x01:	//モーターON
		printf("case : 0x01\r\n");
		if(ud->mode_state==1 && ud->run_state==0){
			robot_arm->axis[0].state_change = 1;
			robot_arm->axis[1].state_change = 1;
			robot_arm->axis[0].state = 1;
			robot_arm->axis[1].state = 1;
			while(!(robot_arm->axis[0].end_state) && !(robot_arm->axis[1].end_state)){}
			pack_cmd(&robot_arm->axis[0],0.0, 0.0, 0.0, Motion_KI, 0.0);
			BLDC_CANTx(&robot_arm->axis[0]);
			unpack_reply(&robot_arm->axis[0]);
			printf("axis[0]_getpos : %lf\r\n",robot_arm->axis[0].position);
			pack_cmd(&robot_arm->axis[0],robot_arm->axis[0].position, 0.0, MAX_KP, Motion_KI, 0.0);
			BLDC_CANTx(&robot_arm->axis[0]);
			pack_cmd(&robot_arm->axis[1],0.0, 0.0, 0.0, Motion_KI, 0.0);
			BLDC_CANTx(&robot_arm->axis[1]);
			unpack_reply(&robot_arm->axis[1]);
			printf("axis[1]_getpos : %lf\r\n",robot_arm->axis[1].position);
			pack_cmd(&robot_arm->axis[1],robot_arm->axis[1].position, 0.0, MAX_KP, Motion_KI, 0.0);
			BLDC_CANTx(&robot_arm->axis[1]);
			HAL_Delay(100);
			robot_arm->axis2.firstpos=180*100;
			robot_arm->axis2.target_position=90*100;
			robot_arm->axis2.motion_state=1;
			RMD_calculatuin(robot_arm);
			while(!(robot_arm->axis2.end_state)){}
			printf("MotionOK\r\n");
			robot_arm->axis2.offset=(int)round((100.0/360.0)*65535.0);
			robot_arm->axis2.offset_state = 1;
			RMD_calculatuin(robot_arm);
			ud->stanby_state = 1;
			ud->run_state = 1;
		}
		break;
	case 0x02:	//モーターOFF
		if(ud->mode_state==1 && ud->stanby_state==1){
			printf("case : 0x02\r\n");
			robot_arm->axis[0].state_change = 1;
			robot_arm->axis[1].state_change = 1;
			robot_arm->axis[0].state = 0;
			robot_arm->axis[1].state = 0;
			ud->run_state = 0;
		}
		break;
	case 0x03:	//動作モード
		printf("case : 0x03\r\n");
		if(ud->coordinate_state==1 && ud->mode_wait_flag==1 && ud->run_state==1 && ud->stanby_state==0){
			printf("arm_running\r\n");
			BLDC_calculatuin(robot_arm);
			while(!(robot_arm->axis[0].end_state) && !(robot_arm->axis[1].end_state)){}
			robot_arm->axis2.target_position=90*100;
			robot_arm->axis2.motion_state = 1;
			RMD_calculatuin(robot_arm);
			ud->coordinate_run = 1;
		}
		printf("motion_state : %d\r\n",ud->coordinate_state);
		printf("mode_wait_flag : %d\r\n",ud->mode_wait_flag);
		printf("run_state : %d\r\n",ud->run_state);
		printf("stanby_state : %d\r\n",ud->stanby_state);

		ud->mode_wait_flag=1;
		break;
	case 0x04:	//オリジナルポディションへ移動
		if(ud->mode_state==1 && ud->run_state==1){
			printf("case : 0x04\r\n");
			if(ud->stanby_state==1){
				//腕伸ばした状態にする//
				robot_arm->axis[0].target_position=(-robot_arm->axis[0].position);
				robot_arm->axis[1].target_position=0.0;

				printf("axis[0]target_position : %lf\r\n",robot_arm->axis[0].target_position);
				printf("axis[1]target_position : %lf\r\n",robot_arm->axis[1].target_position);
				printf("axis[0]position : %lf\r\n",robot_arm->axis[0].position);
				printf("axis[1]position : %lf\r\n",robot_arm->axis[1].position);
				BLDC_calculatuin(robot_arm);
				while(!(robot_arm->axis[0].end_state) && !(robot_arm->axis[1].end_state)){}
			}
			BLDC_IK((float)origin_x,(float)origin_y,&robot_arm->axis[0].target_position,&robot_arm->axis[1].target_position);
			BLDC_calculatuin(robot_arm);
			printf("axis[0]target_position : %lf\r\n",robot_arm->axis[0].target_position);
			printf("axis[1]target_position : %lf\r\n",robot_arm->axis[1].target_position);
			printf("axis[0]position : %lf\r\n",robot_arm->axis[0].position);
			printf("axis[1]position : %lf\r\n",robot_arm->axis[1].position);
			while(!(robot_arm->axis[0].end_state) && !(robot_arm->axis[1].end_state)){}
			robot_arm->axis2.target_position=90*100;
			robot_arm->axis2.motion_state=1;
			RMD_calculatuin(robot_arm);
			ud->stanby_state = 0;
		}
		break;
	case 0x05:		//待機状態へ戻る
		if(ud->mode_state==1 && ud->run_state==1){
			printf("case : 0x05\r\n");
			robot_arm->axis[0].target_position=0.0;
			robot_arm->axis[1].target_position=M_PI;
			BLDC_calculatuin(robot_arm);
			while(!(robot_arm->axis[0].end_state) && !(robot_arm->axis[1].end_state)){}
			robot_arm->axis2.target_position=90*100;
			robot_arm->axis2.motion_state=1;
			RMD_calculatuin(robot_arm);
			printf("axis[0]target_position : %lf\r\n",robot_arm->axis[0].target_position);
			printf("axis[1]target_position : %lf\r\n",robot_arm->axis[1].target_position);
			ud->stanby_state = 1;
		}
		break;
	case 0x06:	//腕伸ばした状態
		if(ud->mode_state==1 && ud->run_state==1 && ud->stanby_state == 1){
			printf("case : 0x06\r\n");
			robot_arm->axis[0].target_position=0.0;
			robot_arm->axis[1].target_position=0.0;
			BLDC_calculatuin(robot_arm);
			while(!(robot_arm->axis[0].end_state) && !(robot_arm->axis[1].end_state)){}
			robot_arm->axis2.target_position=90*100;
			robot_arm->axis2.motion_state=1;
			RMD_calculatuin(robot_arm);
			printf("axis[0]target_position : %lf\r\n",robot_arm->axis[0].target_position);
			printf("axis[1]target_position : %lf\r\n",robot_arm->axis[1].target_position);
			ud->stanby_state = 0;
		}
		break;
	case 0x07:	//ティーチングモード開始
		if(ud->mode_state==1 && ud->run_state==1){
			ud->stanby_state = 0;
		}
		break;
	case 0x08:	//ティーチング決定
		if(ud->mode_state==1 && ud->run_state==1){
			ud->stanby_state = 0;
		}
		break;
	case 0x09:	//スタンバイ位置に戻る
		if(ud->mode_state==1 && ud->run_state==1){
			BLDC_IK((float)origin_x,(float)origin_y,&robot_arm->axis[0].target_position,&robot_arm->axis[1].target_position);
			BLDC_calculatuin(robot_arm);
			ud->stanby_state = 1;
		}
		break;

	case 0x0A:	//push&pull auto
		if(ud->coordinate_state==1 && ud->mode_wait_flag==1 && ud->run_state==1){
			printf("case : 0x0A\r\n");
			BLDC_calculatuin(robot_arm);
			while(!(robot_arm->axis[0].end_state) && !(robot_arm->axis[1].end_state)){}
			prepareAbsoluteMovement(0, si->sending_step);
			runAndWait();
			stepperHoming(0);
			ud->stanby_state = 0;
			ud->coordinate_run = 1;
		}
		ud->mode_wait_flag=1;
		break;
	case 0x0B:	//push&pull&twist auto
		if(ud->coordinate_state==1 && ud->mode_wait_flag==1 && ud->run_state==1){
			printf("case : 0x0B\r\n");
			BLDC_calculatuin(robot_arm);
			while(!(robot_arm->axis[0].end_state) && !(robot_arm->axis[1].end_state)){}
			robot_arm->axis2.target_position=robot_arm->axis2.a1;
			robot_arm->axis2.motion_state=1;
			RMD_calculatuin(robot_arm);
			while(!(robot_arm->axis2.end_state)){}
			prepareAbsoluteMovement(0, si->sending_step);
			runAndWait();
			robot_arm->axis2.target_position=robot_arm->axis2.a2;
			robot_arm->axis2.motion_state=1;
			RMD_calculatuin(robot_arm);
			while(!(robot_arm->axis2.end_state)){}
			stepperHoming(0);
			runAndWait();
			ud->stanby_state = 0;
			ud->coordinate_run = 1;
		}
		ud->mode_wait_flag=1;
		break;
	case 0x0C:	//keep push
		if(ud->mode_state==1 && ud->run_state==1){
			printf("case : 0x0C\r\n");
			prepareAbsoluteMovement(0, si->sending_step);
			runAndWait();
			ud->stanby_state = 0;
		}
		break;
	case 0x0D:	//leave button
		if(ud->mode_state==1 && ud->run_state==1){
			printf("case : 0x0D\r\n");
			stepperHoming(0);
			ud->stanby_state = 0;
		}
		break;
	case 0xC1:
		if(ud->mode_state==1 && ud->run_state==1){
			printf("case : 0xC1\r\n");
			float fkrd[2];
			BLDC_IK((float)1000.0,(float)1000.0,&robot_arm->axis[0].target_position,&robot_arm->axis[1].target_position);
			printf("axis[0]target_position : %lf\r\n",robot_arm->axis[0].target_position);
			printf("axis[1]target_position : %lf\r\n",robot_arm->axis[1].target_position);
			BLDC_FK(robot_arm->axis[0].target_position,robot_arm->axis[1].target_position,&fkrd[0],&fkrd[1]);
			printf("fkrd[0] : %lf\r\n",fkrd[0]);
			printf("fkrd[1] : %lf\r\n",fkrd[1]);
			BLDC_calculatuin(robot_arm);
//			robot_arm->axis[0].target_position=(-robot_arm->axis[0].position);
//			robot_arm->axis[1].target_position=(-(robot_arm->axis[1].position-M_PI));
//		printf("axis0_position:%lf\r\n",robot_arm->axis[0].position);
//		printf("axis1_position:%lf\r\n",robot_arm->axis[1].position);
//			SendAll(robot_arm);
		}
		break;
	case 0xC2:
		if(ud->mode_state==1 && ud->run_state==1){
			printf("case : 0xC2\r\n");
//			prepareAbsoluteMovement(0, 10000);
//			runAndWait();
//			HAL_Delay(100);
			prepareAbsoluteMovement(0, -10000);
			runAndWait();
			HAL_Delay(100);
		}
		break;
	case 0xC3:
		if(ud->mode_state==1 && ud->run_state==1){
			printf("case : 0xC3\r\n");
			printf("case:1\r\n");
			stepperHoming(0);
		}
		break;
	case 0xC4:
		if(ud->mode_state==1 && ud->run_state==1){
			printf("case : 0xC4\r\n");
			robot_arm->axis2.target_position=0;
			robot_arm->axis2.motion_state=1;
			RMD_calculatuin(robot_arm);
		}
		break;
	case 0xC5:
		if(ud->mode_state==1 && ud->run_state==1){
			printf("case : 0xC5\r\n");
			robot_arm->axis2.target_position=10000;
			robot_arm->axis2.motion_state=1;
			RMD_calculatuin(robot_arm);
		}
		break;
	case 0xC6:
		if(ud->mode_state==1 && ud->run_state==1){
			printf("case : 0xC6\r\n");
			robot_arm->axis2.motion_state=1;
			robot_arm->axis2.PID_state=1;
			RMD_calculatuin(robot_arm);
		}
		break;

	default:
		break;
	}
	if(ud->mode_wait_flag!=1){	//コマンドモードだけでの実行
		ud->mode_state=0;
	}else if(ud->coordinate_run == 1){	//コマンド＋座標での実行
		ud->mode_wait_flag=0;
		ud->mode_state=0;
		ud->coordinate_state=0;
		ud->coordinate_run=0;
	}else if(ud->coordinate_run != 1){
		ud->coordinate_state=0;	//座標への動作を実行する前に送られた座標は無視する
	}
//	SendAll(robot_arm);

}
void SentData_Transmit(uint8_t sd[20]){
	volatile UART_data* ud = &data;
	sd[0]=0xAA;
	sd[1]=ud->cmd;
	sd[buffersize-1]=0xAA;
	for(int i=2;i<buffersize-1;i++){
		sd[i]=0xFF;
	}
//	  return sd;
}
void keycommand(uint8_t *com){
	if(com!=0){
		switch(com[0]){
		case '1':
			RxData[0]=0xAA;
			RxData[1]=0x01;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case '2':
			RxData[0]=0xAA;
			RxData[1]=0x02;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case '3':
			RxData[0]=0xAA;
			RxData[1]=0x03;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case '4':
			RxData[0]=0xAA;
			RxData[1]=0x04;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case '5':
			RxData[0]=0xAA;
			RxData[1]=0x05;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case '6':
			RxData[0]=0xAA;
			RxData[1]=0x06;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case'a':
			RxData[0]=0xAA;
			RxData[1]=0x0A;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case'b':
			RxData[0]=0xAA;
			RxData[1]=0x0B;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case'c':
			RxData[0]=0xAA;
			RxData[1]=0x0C;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case'd':
			RxData[0]=0xAA;
			RxData[1]=0x0D;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case'e':
			RxData[0]=0xAA;
			RxData[1]=0x06;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case 'n':
			RxData[0]=0xAA;
			RxData[1]=0xC1;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case 'm':
			RxData[0]=0xAA;
			RxData[1]=0xC2;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case 'h':
			RxData[0]=0xAA;
			RxData[1]=0xC3;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case 'j':
			RxData[0]=0xAA;
			RxData[1]=0xC4;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case 'k':
			RxData[0]=0xAA;
			RxData[1]=0xC5;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case 'l':
			RxData[0]=0xAA;
			RxData[1]=0xC6;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
		case 'r':
			RxData[0]=0xAA;
			RxData[1]=0x62;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case 't':
			RxData[0]=0xAA;
			RxData[1]=0x63;
			RxData[19]=0xAA;
			for(int i=2;i<19;i++){
				RxData[i]=0xFF;
			}
			break;
		case 'p':
			printf("cordinate input\r\n");
			RxData[0]=0xAA;
			RxData[1]=0xFF;	//x1
			RxData[2]=0xFF;
			RxData[3]=0xFF;	//y1
			RxData[4]=0xFF;
			RxData[5]=0x2C;	//x2
			RxData[6]=0x01;
			RxData[7]=0x2C;	//y2
			RxData[8]=0x01;
			RxData[9]=0x20;	//z
			RxData[10]=0x00;
			RxData[11]=0x00;	//a1
			RxData[12]=0x00;
			RxData[13]=0x84;	//a2
			RxData[14]=0x03;
			RxData[15]=0xFF;	//l1
			RxData[16]=0xFF;
			RxData[17]=0xFF;	//l2
			RxData[18]=0xFF;
			RxData[19]=0xAA;
			break;
		case 'o':
			printf("error cordinate\r\n");
			RxData[0]=0xAA;
			RxData[1]=0xFF;	//x1
			RxData[2]=0xFF;
			RxData[3]=0xFF;	//y1
			RxData[4]=0xFF;
			RxData[5]=0xC1;	//x2
			RxData[6]=0x02;
			RxData[7]=0xC1;	//y2
			RxData[8]=0x02;
			RxData[9]=0x20;	//z
			RxData[10]=0x00;
			RxData[11]=0x00;	//a1
			RxData[12]=0x00;
			RxData[13]=0x84;	//a2
			RxData[14]=0x03;
			RxData[15]=0xFF;	//l1
			RxData[16]=0xFF;
			RxData[17]=0xFF;	//l2
			RxData[18]=0xFF;
			RxData[19]=0xAA;
			break;
		default:
			break;
		}
		com[0]=0;
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
//	setbuf(stdout, NULL);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_CAN1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_CAN2_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(500);
  //Set stepper 1

  steppers[0].dirFunc = Dir0;
  steppers[0].stepFunc = Step0;
  steppers[0].acceleration = 3000;
  steppers[0].minStepInterval = 100;
  steppers[0].homing = 0;
  steppers[0].dir_inv = 0;
  steppers[0].seeking_vel = 100;
  steppers[0].homing_vel = 100;
  steppers[0].pull_off = 200;

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
  TMC2209_INIT();
//  TMCsetup();
  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim2);
  printf("init_OK\r\n");
  HAL_UART_Receive_IT(&huart2, (uint8_t *)RxData, buffersize);
//  HAL_UART_Receive_IT(&huart2, (uint8_t *)RxData, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  get_uart_flag=0;
	 	  while(!get_uart_flag){}
//	 	  keycommand(RxData);
	 	  ReceiveData_Transmit(RxData);
	 	  SentData_Transmit(TxData);
	 //	  for(int i=0;i<buffersize;i++){
	 //		  printf("Txdata : %x\r\n",TxData[i]);
	 //
	 //	  	}

	 	  HAL_UART_Transmit(&huart2,(uint8_t *)TxData,sizeof(TxData),100);
	 	  memset(RxData, 0, sizeof(RxData));
	 	  memset(TxData, 0, sizeof(TxData));
	 //	  printf("command :\r\n");

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
  hcan1.Init.TimeSeg1 = CAN_BS1_6TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_3TQ;
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
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_6TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = ENABLE;
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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 8000-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 400-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

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

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
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
	*xp=x;
	*yp=y;
}
void BLDC_IK(float x,float y,float* s1p,float* s2p){
	float s1=0.0;
	float s2=0.0;
	float ns=0.0;
	float nx=0.0;
	float ny=0.0;
	float al=590.0;
	s2=-acos((x*x+y*y-l1*l1-l2*l2)/(2*l1*l2));
	s1=atan2((-l2*sin(s2)*x+(l1+l2*cos(s2))*y),((l1+l2*cos(s2))*x+l2*sin(s2)*y));
	if((-M_PI<=s2 && s2<=0) && (0<=s1 && s1<=M_PI)){
		*s1p=s1;
		*s2p=s2;
		printf("coordinate OK!!\r\n");
	}
	else if(sqrt(fabsf(x*x+y+y))>=al){
		if(x >= 0){
			ns=atan(y/x);
			nx=al*cos(ns);
			ny=al*sin(ns);
		}else if(x < 0){
			ns=atan(y/x);
			nx=-al*cos(ns);
			ny=al*sin(ns);
		}
			s2=-acos((nx*nx+ny*ny-l1*l1-l2*l2)/(2*l1*l2));
			s1=atan2((-l2*sin(s2)*nx+(l1+l2*cos(s2))*ny),((l1+l2*cos(s2))*nx+l2*sin(s2)*ny));
			*s1p=s1;
			*s2p=s2;
			printf("coordinate hosei x :%f \r\n",nx);
			printf("coordinate hosei y :%f \r\n",ny);
		}
	else{
		TxData[0]=0xAA;
		TxData[1]=0x61;
		TxData[buffersize-1]=0xAA;
		for(int i=2;i<buffersize-1;i++){
			TxData[i]=0xFF;
		}
		printf("coordinate error!!\r\n");
		HAL_UART_Transmit_IT(&huart2,(uint8_t *)TxData,sizeof(TxData));
	}
}
//int _write(int file, char *ptr, int len)	//printfに必要
//{
//  HAL_UART_Transmit(&huart2,(uint8_t *)ptr,len,10);
//  return len;
//}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)
{
	get_uart_flag=1;
//	HAL_UART_Receive_IT(&huart2, (uint8_t *)RxData, 1);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)RxData, buffersize);
	flag_count=0;
	motion_stop_flag=0;
	for(int i=0; i<buffersize; i++){
		if(RxData[i]!=0xFF){
			flag_count++;
		}
	}

	if(flag_count==3 && RxData[1]==0x63){
		motion_stop_flag=1;
	}
	else if(flag_count==3 && RxData[1]==0x62){
		HAL_NVIC_SystemReset();
	}
//	if(flag_count==3&&RxData[1]==0x08){
//		teaching_flag=1;
//	}

}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//	//Arm home position
//	if(GPIO_Pin == GPIO_PIN_10 ){
//		if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)){	//end stop ONの時フラグを立てる
//			home_position_state |= 1;
//		}
//		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)){
//			home_position_state &= ~1;
//		}
//	}
	//stepper 0 end stop sensor
	if(GPIO_Pin == GPIO_PIN_6 ){
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){
			zhome_endstop_state = 0;
		}else{
			zhome_endstop_state = 1;
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
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan_)
{
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0, &RxHeader, canRxdata);
	if(RxHeader.StdId==0x141){
		memcpy(RMDRx_can_data, canRxdata, 8);
	}

	HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0, &RxHeader, canRxdata);
	if(RxHeader.StdId==0x00){
			//memcpy(m1Rx_can_data,Rx_can_data,sizeof(Rx_can_data));
		memcpy(BLDC_canRxdata, canRxdata, 6);
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
