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
#include "tmc/ic/TMC2209/TMC2209.h"
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
CAN_HandleTypeDef hcan2;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//SteppingMotorの設定
TMC2209TypeDef TMC2209; //TMC2209の設定などの定義
ConfigurationTypeDef TMC2209_config; //TMC2209の設定
volatile stepperInfo steppers[1];    //stepperInfo型の配列stepperにNUM_STEPPERSで定義した値を代入
volatile checkerInfo currentget[1];
uint8_t end_stop_state = 0;
uint8_t homing_flag = 0;
uint8_t current_stall = 0;
//RMDの設定
CAN_FilterTypeDef sFilterConfig;
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint8_t buffersize=10;
uint8_t TxData[10];
uint8_t RxData[10];
uint8_t can1_data[10];
uint8_t can2_data[10];
uint8_t UART1_Data[10];
uint32_t canid;
uint32_t TxMailbox;
uint8_t cmd_data = 0x00;
uint8_t get_uart_flag = 0;
uint8_t home_position_state =0;
uint8_t end_flag = 0; //canを受信したかどうかのflag
uint8_t nextcan_flag = 0;
uint8_t get_can1_flag=0;
uint8_t get_can2_flag=0;
uint8_t selectmode_flag=0;
uint16_t p_getintdata=0;
int Writecom=0x00;
int Readcom=0x00;
char RMD_cmd;
float t_position[2];
int step_position[4];
float p_getdata=0.0;
float t_position[2];
float l1=200.0;
float l2=200.0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_CAN2_Init(void);
/* USER CODE BEGIN PFP */
static uint8_t reset(void);
static uint8_t restore(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
uint8_t uart_data;
char tx_data[]="get_data\r\n";
int flag = 1;

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


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//stepper 0 end stop senssor
	if(GPIO_Pin == GPIO_PIN_6 ){
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){
			end_stop_state |= 1;
		}
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){
			end_stop_state &= ~1;
		}
	}
	if(GPIO_Pin == GPIO_PIN_7 ){
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)){
			current_stall|= 0;
		}
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)){
			current_stall = 1;
		}
	}
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
	printf("absolute_steps:%d\r\n",absolute_steps);
	volatile stepperInfo* si = &steppers[0];	//steppers[0]のアドレスをstepperInfo* si に代入
	int steps = absolute_steps - si->stepPosition;	//目標値からstepPositionを引いた値（stepposition=毎ステップカウントされるdirの値)
	printf("steps:%d\r\n",steps);
	if(steps == 0){
					return;
				}
	si->dirFunc( steps < 0 ? 1 : 0);
	si->dir = steps > 0 ? 1:-1;
	//printf("prepareAbsoluteMovement\r\n");
	si->totalSteps = abs(steps);	//stepsの値をアブソリュートでtotalstepsに格納
	printf("steps:%d\r\n",si->totalSteps);

	resetStepper(si);
	printf("steps:%d\r\n",steps);
	remainingSteppersFlag |= (1 << 0);
}
void currentcheck(){
	uint32_t value=0;
	volatile checkerInfo* t = &currentget[0];
		t->checkcount++;
		if(t->checkcount==2000){
			value=tmc2209_readInt(&TMC2209, TMC2209_SG_RESULT);
			printf("SG_RESULT  = %ld\r\n",value);

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
				if(end_stop_state & (1 << 0)){
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
		//printf("stepPosition:%ld \r\n",s->stepPosition);
		setNextInterruptInterval();
		__HAL_TIM_SET_COUNTER(&htim3, 0);
	}
}

void sensorStateInit(){							//割り込みリセット
	if( ! HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)){	//割り込みピンが真でない(1でない=0なら)
		end_stop_state |= 1;					//end_stop_stateに左辺か右辺が1なら1を代入
	}
}

void stallguard_back(){
	prepareMovement(steppers[0].stall_off);	//stall_off
	current_stall=0;
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
	printf("RestOK\r\n");
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

	homing_flag &= ~(1 << 0);								//homing_flagが反応したらpull_offだけモータ回す
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
	printf("homing:\r\n");

}
void MotorOFF(){
	int32_t value = 0;

	value = 0;
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT);	//保持電流
	printf("IHOLD: %ld\r\n", value);

	value = 0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_FREEWHEEL_MASK, 20);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("Freewheeling: %ld\r\n", value);
	HAL_Delay(100);
	value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_PWMCONF, TMC2209_FREEWHEEL_MASK, 20, value);
	printf("Freewheeling: %ld\r\n", value);

	end_flag=1;


}

void InvertDir(){
	int32_t value = 0;

	value = 0;
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_GCONF, TMC2209_SHAFT_SHIFT, TMC2209_SHAFT_MASK, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_SHAFT_SHIFT, TMC2209_SHAFT_MASK);	//保持電流
	printf("IHOLD: %ld\r\n", value);
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


	IRUN_value = 22;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT, IRUN_value);	//実行電流
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	IRUN_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT);
	printf("IRUN: %ld\r\n", IRUN_value);

	IHOLD_value= 16;																						//モーター待機時の電流の設定
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT, IHOLD_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	IHOLD_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT);	//保持電流
	printf("IHOLD: %ld\r\n", IHOLD_value);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK,TMC2209_PDN_DISABLE_SHIFT);
	printf("pdn_disable : %ld\r\n", value);
	value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK, TMC2209_PDN_DISABLE_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK, TMC2209_PDN_DISABLE_SHIFT);
	printf("pdn_disable : %ld\r\n", value);

	TCOOLTHRS_value=300;
	tmc2209_writeInt(&TMC2209, TMC2209_TCOOLTHRS, TCOOLTHRS_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("TCOOLTHRS: %ld\r\n", TCOOLTHRS_value);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK,TMC2209_EN_SPREADCYCLE_SHIFT);
	printf("SPREADCYCLE : %ld\r\n", value);
	value = 0;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT ,value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK,TMC2209_EN_SPREADCYCLE_SHIFT);
	printf("SPREADCYCLE : %ld\r\n", value);

	value=10;
	tmc2209_writeInt(&TMC2209, TMC2209_TPWMTHRS, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("TPWMTHRS: %ld\r\n", value);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_INTERNAL_RSENSE_MASK,TMC2209_INTERNAL_RSENSE_SHIFT);
	printf("RSENSE : %ld\r\n", value);
	value = 0;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_INTERNAL_RSENSE_MASK, TMC2209_INTERNAL_RSENSE_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_INTERNAL_RSENSE_MASK, TMC2209_INTERNAL_RSENSE_SHIFT);
	printf("RSENSE : %ld\r\n", value);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_FREQ_MASK, TMC2209_PWM_FREQ_SHIFT);
	printf("PWM_FREQ : %ld\r\n", value);
	value=1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_FREQ_MASK, TMC2209_PWM_FREQ_SHIFT, value);
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_FREQ_MASK, TMC2209_PWM_FREQ_SHIFT);
	printf("PWM_FREQ : %ld\r\n", value);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209,TMC2209_COOLCONF, TMC2209_SEIMIN_MASK, TMC2209_SEIMIN_SHIFT);
	printf("SEIMIN : %ld\r\n", value);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_DRVSTATUS, TMC2209_CS_ACTUAL_MASK, TMC2209_CS_ACTUAL_SHIFT);
	printf("DRVSTATUS : %ld\r\n", value);

	mstep_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK,TMC2209_MSTEP_REG_SELECT_SHIFT);	//uartでのmicrostepの有効化
	printf("mstep_before : %ld\r\n", mstep_value);
	mstep_value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK, TMC2209_MSTEP_REG_SELECT_SHIFT,mstep_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	mstep_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK, TMC2209_MSTEP_REG_SELECT_SHIFT);
	printf("mstep_after : %ld\r\n", mstep_value);

	toff_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT);	//保持トルクの設定
	printf("toff_before : %ld\r\n", toff_value);
	toff_value = 3;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT,toff_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	toff_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT);
	printf("toff_after : %ld\r\n", toff_value);

	microstep_value = 256 >> TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT);	//microstepの設定
	printf("microstep_before : %ld\r\n", microstep_value);
	microstep_value = 4;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT,microstep_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	microstep_value = 256 >> TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT);
	printf("microstep_after : %ld\r\n", microstep_value);

	IHOLDDELAY_value=4;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLDDELAY_MASK, TMC2209_IHOLDDELAY_SHIFT, IHOLDDELAY_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	IHOLDDELAY_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLDDELAY_MASK, TMC2209_IHOLDDELAY_SHIFT);
	printf("IHOLDDELAY: %ld\r\n", IHOLDDELAY_value);

	stallgard_setup_value=10;											//SG_RESULTと比較して、
	tmc2209_writeInt(&TMC2209, TMC2209_SGTHRS, stallgard_setup_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	stallgard_setup_value = tmc2209_readInt(&TMC2209, TMC2209_SGTHRS);
	printf("SGTHRS: %ld\r\n", stallgard_setup_value);

	stealthChop_value = 100;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT, stealthChop_value);
	stealthChop_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("PWM_GRAD: %ld\r\n", stealthChop_value);


	value=5;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEMIN_MASK, TMC2209_SEMIN_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("SEMIN: %ld\r\n", value);

	SEMAX_value = 2;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEMAX_MASK, TMC2209_SEMAX_SHIFT, SEMAX_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("SEMAX: %ld\r\n", SEMAX_value);

	TBL_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	TBL_value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT, TBL_value);
	TBL_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT);
	printf("TBL: %ld\r\n", TBL_value);

	stealthChop_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("PWM_GRAD: %ld\r\n", stealthChop_value);

	autoscale_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_AUTOSCALE_MASK, TMC2209_PWM_AUTOSCALE_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("autoscale: %ld\r\n", autoscale_value);

	PWMAuto_value = tmc2209_readInt(&TMC2209,TMC2209_PWM_AUTO);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("PWMAuto: %ld\r\n", PWMAuto_value);

	PWMAuto_scale = tmc2209_readInt(&TMC2209,TMC2209_PWMSCALE);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("PWMSCALE: %ld\r\n", PWMAuto_scale);

	value = 0;
	value = TMC2209_FIELD_READ(&TMC2209, 0x07, TMC2209_FCLKTRIM_MASK, TMC2209_FCLKTRIM_SHIFT);

	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("frequency: %ld\r\n", value);




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
	TxData[1] = 0x00;
	TxData[2] = 0x00;
	TxData[3] = 0x00;
	TxData[4] = 0x00;
	TxData[5] = 0x00;
	TxData[6] = 0x00;
	TxData[7] = 0x00;
	switch(Writecom){
		case 0x31:
			TxData[0] = 0x31;	//PI設定
			TxData[2] = UART1_Data[2];	//anglePidKp
			TxData[3] = UART1_Data[3];	//anglePidKi
			TxData[4] = UART1_Data[4];	//speedPidKp
			TxData[5] = UART1_Data[5];	//speedPidKi
			TxData[6] = UART1_Data[6];	//iqPidKp
			TxData[7] = UART1_Data[7];	//iqPidKi
		case 0x32:
			TxData[0] = 0x32;	//PI設定
			TxData[2] = UART1_Data[2];	//anglePidKp
			TxData[3] = UART1_Data[3];	//anglePidKi
			TxData[4] = UART1_Data[4];	//speedPidKp
			TxData[5] = UART1_Data[5];	//speedPidKi
			TxData[6] = UART1_Data[6];	//iqPidKp
			TxData[7] = UART1_Data[7];	//iqPidKi
			break;
		case 0x34:
			TxData[0] = 0x34;	//accelePI設定
			TxData[4] = UART1_Data[4];	//speedI
			TxData[5] = UART1_Data[5];	//TorqueP
			TxData[6] = UART1_Data[6];	//TorqueI
			TxData[7] = UART1_Data[7];	//PosI
			break;
		case 0xA1:
			TxData[0] = 0xA1;	//Torque設定
			TxData[4] = UART1_Data[4];	//currentlow
			TxData[5] = UART1_Data[5];	//currenthigh
			break;
		case 0xA4:
			TxData[0] = 0xA4;
			TxData[1] = 0x00;
			TxData[2] = 0x2C;	//speed low
			TxData[3] = 0x01;	//speed high
			TxData[4] = UART1_Data[4];	//poslow
			TxData[5] = UART1_Data[5];
			TxData[6] = 0x00;
			TxData[7] = 0x00;	//poshigh
			break;
		default:
			printf("No Data Writecom\r\n");
			end_flag=1;
			break;
		}
	Writecom=0x00;
	switch(Readcom){
		case 0x30:
			TxData[0] = 0x30;
			break;
		case 0x33:
			TxData[0] = 0x33;
			break;
		case 0x9C:
			TxData[0] = 0x9C;
			break;
		case 0xA1:
			TxData[0] = 0xA1;
		case 0x90:
			TxData[0] = 0x90;
			break;
		default:
			printf("No Data Readcom\r\n");
			end_flag=1;
			break;
	}
	Readcom=0x00;
	if(0 < HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) ){
		HAL_CAN_AddTxMessage(&hcan1,&TxHeader,TxData,&TxMailbox);
		HAL_Delay(1);
//		printf("Can Send\r\n");
		while(!nextcan_flag){
//			printf("Mailboxes: %d\r\n",HAL_CAN_GetTxMailboxesFreeLevel(&hcan1));
			HAL_CAN_AddTxMessage(&hcan1,&TxHeader,TxData,&TxMailbox);
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
		end_flag=1;
	}
}
void SetUpRMD(){
	UART1_Data[4] = 0x50;
	UART1_Data[5] = 0x46;
	Writecom = 0xA4;
	RMDCommand();
}
void RMDLeftTurn(){
	UART1_Data[4] = 0xE0;
	UART1_Data[5] = 0x2E;
	Writecom=0xA4;
	RMDCommand();
}
void RMDRightTurn(){
	UART1_Data[4] = 0xC0;
	UART1_Data[5] = 0x5D;
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
	printf("0x30\r\n");
	printf("Position loop Kp :%d \r\n",RxData[2]);
	printf("Position loop Ki :%d \r\n",RxData[3]);
	printf("Speed loop Kp :%d \r\n",RxData[4]);
	printf("Speed loop Ki :%d \r\n",RxData[5]);
	printf("Torque loop Kp :%d \r\n",RxData[6]);
	printf("Torque loop Ki :%d \r\n",RxData[7]);

	Readcom=0x33;
	RMDCommand();
	printf("0x33\r\n");
	printf("Position loop P:%d \r\n",RxData[2]);
	printf("Position loop I :%d \r\n",RxData[3]);
	printf("Speed loop P :%d \r\n",RxData[4]);
	printf("Speed loop I :%d \r\n",RxData[5]);
	printf("Torque loop P :%d \r\n",RxData[6]);
	printf("Torque loop I :%d \r\n",RxData[7]);

	Readcom=0x9C;
	RMDCommand();
	printf("0x9C\r\n");
	RxBitshift[0]=RxData[2] & 0xFF;
	RxBitshift[0]+=(RxData[3] & 0xFF) << 8;
	RxBitshift[1]=RxData[4] & 0xFF;
	RxBitshift[1]+=(RxData[5] & 0xFF) << 8;
	RxBitshift[2]=RxData[6] & 0xFF;
	RxBitshift[2]+=(RxData[7] & 0xFF) << 8;
	printf("Motor temperature:%d \r\n",RxData[1]);
	printf("Torque current :%d \r\n",RxBitshift[0]);
	printf("Speed :%d \r\n",RxBitshift[1]);
	printf("Encoder position:%d \r\n",RxBitshift[2]);
}
void RMDPIDSetting(){
	UART1_Data[2] = 0x64;
	UART1_Data[3] = 0x64;
	UART1_Data[4] = 0x28;
	UART1_Data[5] = 0xE;
	UART1_Data[6] = 0x14;
	UART1_Data[7] = 0x14;
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

void command_judgment(uint8_t *ModeSelect){
	int modeflag_count=0;
	for(uint8_t i=0; i<10 ;i++){
		ModeSelect[i]= ~(ModeSelect[i]);
		if(ModeSelect[i]==0){
			modeflag_count++;
			}
		}
	if(modeflag_count<=8){	//動作モード
		selectmode_flag=0;
	}
	else if(modeflag_count==9){	//コマンドモード,9byte ModeSelectが0だったら
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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_CAN2_Init();
  /* USER CODE BEGIN 2 */
  TMC2209_INIT();
  //rxbufを受信したらフラグを
  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
  printf("Hello\r\n");
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
sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;
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

printf("RMD Start\r\n");
uint8_t judgment[10];
uint8_t modeflag=0;
uint8_t checkdata[10];
uint8_t mode_select;
int16_t receive_xy_coordinate[2];
int16_t sent_data[10];

char Mode_Type[4];
float rad_data[2];
float machine_coordinate_data[2];
float tip_coordinate_data[2];
float machine_origin_rad[10];
float machine_origin_coordinate[10];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //受信するまで待つ
	 	  while(!get_uart_flag){}
	 	  get_can1_flag=0;//canデータ受信用のフラグを0に
	 	  for(int i=0;i<10;i++){
	 		  judgment[i]=RxData[i];
	 	  }
	 	  command_judgment(judgment);
	 	  if(selectmode_flag==1){
	 		  mode_select=RxData[0];	//モード変更
	 		  }
	 	  else if(selectmode_flag==0){
	 		  receive_xy_coordinate[0]=RxData[0]&0xFF;	//受け取った座標データをビットシフトしてx,yの値に直す
	 		  receive_xy_coordinate[0]+= (RxData[1]&0xFF)<<8;
	 		  receive_xy_coordinate[1]=RxData[2]&0xFF;
	 		  receive_xy_coordinate[1]+= (RxData[3]&0xFF)<<8;

//	 		  tip_coordinate_data[0]=(float)receive_xy_coordinate[0];
//	 		  tip_coordinate_data[1]=(float)receive_xy_coordinate[1];
//	 		  machine_coordinate_data[0]=machine_origin_coordinate[0]+tip_coordinate_data[1];	//手先座標系から機械座標系への変換
//	 		  machine_coordinate_data[1]=machine_origin_coordinate[1]-tip_coordinate_data[0];
	 		  HAL_UART_Transmit_IT(&huart2,RxData,sizeof(RxData));
//	 		  BLDC_IK(machine_coordinate_data[0],machine_coordinate_data[1],&rad_data[0],&rad_data[1]);
	 	  }
	 	  if(home_position_state==1){	//初期位置モード
	 		  switch(mode_select){
	 		  case 0x0A:
//	 			  canid=0x01;
//	 			  EnterMotorMode();
//	 			  BLDC_CANTx();
//	 			  canid=0x02;
//	 			  EnterMotorMode();
//	 			  BLDC_CANTx();
	 			  RxData[0]=0;
	 			  break;
	 		  case 0x1A:
	 			 stepperHoming();
	 			 runAndWait();
	 			 HAL_Delay(10);

	 			  RxData[0]=0;
	 			  break;
	 		  case 0x2A:
	 			  if(selectmode_flag==0){	//初期位置の変更
//	 				  receive_xy_coordinate[0] = RxData[0]&0xFF;
//	 				  receive_xy_coordinate[0] += (RxData[1]&0xFF)<<8;
//	 				  receive_xy_coordinate[1] = RxData[2]&0xFF;
//	 				  receive_xy_coordinate[1] += (RxData[3]&0xFF)<<8;
//	 				  machine_origin_coordinate[0]=(float)receive_xy_coordinate[0];
//	 				  machine_origin_coordinate[1]=(float)receive_xy_coordinate[1];
//	 				  BLDC_IK(machine_origin_coordinate[0],machine_origin_coordinate[1],&rad_data[0],&rad_data[1]);
//	 				  machine_origin_rad[0]=rad_data[0];
//	 				  machine_origin_rad[1]=rad_data[1];
		 			  RxData[0]=0;
	 			  }
	 			  break;
	 		  }
	 	  }
	 	  else if(home_position_state==0){
	 		  switch(mode_select){	//
	 		  case 0xA0:	//動作モード
	 			  if(selectmode_flag==0){
//	 				  t_position[0] = rad_data[0];
//	 				  t_position[1] = rad_data[1];
//	 				  BLDC_MotorRotate();
	 				  prepareAbsoluteMovement(20000);
	 				  runAndWait();
	 			  }
	 			  RxData[0]=0;
	 			  break;
	 		  case 0xA1:
//	 			  t_position[0] = machine_origin_rad[0];	//アーム初期位置
//	 			  t_position[1] = machine_origin_rad[1];
//	 			  BLDC_MotorRotate();
	 			  RxData[0]=0;
	 			  break;
	 		  case 0xA2:
//	 			  t_position[0] = 0;
//	 			  t_position[1] = 0;
//	 			  BLDC_MotorRotate();
	 			  RxData[0]=0;
	 			  break;
	 		  case 0xB0:	//ティーチングモード開始
//	 			  canid=0x01;
//	 			  pack_cmd(0.0, 0.0, 0.0, 1.0, 0.0);
//	 			  BLDC_CANTx();
//	 			  canid=0x02;
//	 			  pack_cmd(0.0, 0.0, 0.0, 1.0, 0.0);
//	 			  BLDC_CANTx();
	 			  RxData[0]=0;
	 			  break;
	 		  case 0xB1:	//ティーチング決定
//	 			  canid=0x01;
//	 			  pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);
//	 			  BLDC_CANTx();
//	 			  unpack_reply();
//	 			  canid=0x02;
//	 			  pack_cmd(0.0, 0.0, 0.0, 4.0, 0.0);
//	 			  BLDC_CANTx();
//	 			  unpack_reply();
//	 			  rad_data[1]=p_getdata;
//
//	 			  BLDC_FK(rad_data[0],rad_data[1],&machine_coordinate_data[0],&machine_coordinate_data[1]);
//	 			  sent_data[0]=round(machine_origin_coordinate[1]-machine_coordinate_data[1]);	//手先座標系から機械座標系への変換
//	 			  sent_data[1]=round(machine_coordinate_data[0]-machine_origin_coordinate[0]);
//	 			  TxData[0]=sent_data[0]&0xFF;
//	 			  TxData[1]=(sent_data[0]>>8)&0xFF;
//	 		   	  TxData[2]=sent_data[1]&0xFF;
//	 			  TxData[3]=(sent_data[1]>>8)&0xFF;
	 			  HAL_UART_Transmit_IT(&huart2,TxData,sizeof(TxData));
	 			  RxData[0]=0;
	 			  break;
	 		  case 0x01:	//モーターON
//	 			  canid=0x01;
//	 			  EnterMotorMode();
//	 			  BLDC_CANTx();
//	 			  canid=0x02;
//	 			  EnterMotorMode();
//	 			  BLDC_CANTx();
	 			  RxData[0]=0;
	 			  break;
	 		  case 0x02:	//モーターOFF
//	 			  canid=0x01;
//	 			  ExitMotorMode();
//	 			  BLDC_CANTx();
//	 			  canid=0x02;
//	 			  ExitMotorMode();
//	 			  BLDC_CANTx();
	 			  RxData[0]=0;
	 			  break;
	 		 case 0xD0:
	 			 MotorOFF();
	 			 HAL_Delay(10);
	 			 break;
	 		case 0xD1:
	 			stepperHoming();
	 			runAndWait();
	 			HAL_Delay(10);
	 			break;
	 		 default:
	 			 get_can1_flag=1;
	 			 get_can2_flag=1;
	 			 break;
	 		  }
	 		  get_uart_flag=0;
	 		  while(!get_can1_flag){}
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

  /*Configure GPIO pin : PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)	//printfに必要
{
  HAL_UART_Transmit(&huart2,(uint8_t *)ptr,len,10);
  return len;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	get_uart_flag = 1;
	HAL_UART_Receive_IT(&huart2, (uint8_t *)RxData, buffersize);
}
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
//	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1);
	  printf("TxCan\n\r");
	  nextcan_flag=1;

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan_)
{
	get_can2_flag=1;
	//	printf("RxCan\r\n");
	HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0, &RxHeader,can2_data);
}
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan_)
{
	get_can1_flag=1;

	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO1, &RxHeader,can1_data);

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
