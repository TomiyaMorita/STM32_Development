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
//SteppingMotorの設定
TMC2209TypeDef TMC2209; //TMC2209の設定などの定義
ConfigurationTypeDef TMC2209_config; //TMC2209の設定
volatile stepperInfo steppers[1];    //stepperInfo型の配列stepperにNUM_STEPPERSで定義した値を代入
uint8_t end_stop_state = 0;
uint8_t homing_flag = 0;
uint8_t current_stall = 0;
uint8_t get_uart_flag=0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
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
volatile uint8_t nextStepperFlag = 0;

void setNextInterruptInterval(){

	unsigned int mind = 999999;

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

void TMCsetup(){
	int32_t mstep_value = 0;
		int32_t toff_value = 0;
		int32_t microstep_value = 0;
		int32_t IRUN_value = 0;
		int32_t IHOLD_value = 0;
		int32_t IHOLDDELAY_value= 0;
		int32_t stallgard_setup_value=0;
		int32_t TCOOLTHRS_value=0;
		int32_t SEMIN_value=0;
		int32_t SEMAX_value=0;
		int32_t TBL_value = 0;
		int32_t stealthChop_value = 0;
		int32_t autoscale_value = 0;
		int32_t PWMAuto_value = 0;
		int32_t PWMAuto_scale = 0;
		int32_t value = 0;


		IRUN_value = 31;
		TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT, IRUN_value);	//実行電流
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		HAL_Delay(100);
		IRUN_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT);
		printf("IRUN: %ld\r\n", IRUN_value);

		IHOLD_value= 31;																						//モーター待機時の電流の設定
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

		value=0;
		value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK,TMC2209_EN_SPREADCYCLE_SHIFT);
		printf("SPREADCYCLE : %ld\r\n", value);
		value = 1;
		TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT ,value);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK,TMC2209_EN_SPREADCYCLE_SHIFT);
		printf("SPREADCYCLE : %ld\r\n", value);

		value=0;
		value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_INTERNAL_RSENSE_MASK,TMC2209_INTERNAL_RSENSE_SHIFT);
		printf("RSENSE : %ld\r\n", value);
		value = 1;
		TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_INTERNAL_RSENSE_MASK, TMC2209_INTERNAL_RSENSE_SHIFT, value);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_INTERNAL_RSENSE_MASK, TMC2209_INTERNAL_RSENSE_SHIFT);
		printf("RSENSE : %ld\r\n", value);

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

		stallgard_setup_value=20;											//SG_RESULTと比較して、
		tmc2209_writeInt(&TMC2209, TMC2209_SGTHRS, stallgard_setup_value);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		stallgard_setup_value = tmc2209_readInt(&TMC2209, TMC2209_SGTHRS);
		printf("SGTHRS: %ld\r\n", stallgard_setup_value);

		TCOOLTHRS_value=1000;

		tmc2209_writeInt(&TMC2209, TMC2209_TCOOLTHRS, TCOOLTHRS_value);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		printf("TCOOLTHRS: %ld\r\n", TCOOLTHRS_value);

		SEMIN_value=5;
		TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEIMIN_MASK, TMC2209_SEIMIN_SHIFT, SEMIN_value);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		printf("SEMIN: %ld\r\n", SEMIN_value);

		SEMAX_value = 2;
		TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEIMIN_MASK, TMC2209_SEIMIN_SHIFT, SEMIN_value);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		printf("SEMAX: %ld\r\n", SEMAX_value);

		TBL_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		printf("TBL: %ld\r\n", TBL_value);

		stealthChop_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		printf("stealthChop: %ld\r\n", stealthChop_value);

		autoscale_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_AUTOSCALE_MASK, TMC2209_PWM_AUTOSCALE_SHIFT);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		printf("autoscale: %ld\r\n", autoscale_value);

		PWMAuto_value = tmc2209_readInt(&TMC2209,TMC2209_PWM_AUTO);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		printf("PWMAuto: %ld\r\n", PWMAuto_value);

		PWMAuto_scale = tmc2209_readInt(&TMC2209,TMC2209_PWMSCALE);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());
		printf("PWMSCALE: %ld\r\n", PWMAuto_scale);

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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
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

char KeyCommand[1];
KeyCommand[0]=0;
printf("RMD Start\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_UART_Receive_IT(&huart2,(uint8_t *)KeyCommand,sizeof(KeyCommand));
	  HAL_UART_Transmit_IT(&huart2,(uint8_t *)KeyCommand,sizeof(KeyCommand));

	  if(get_uart_flag==1){
		  printf("rxbuf 2 %d\r\n",KeyCommand[0]);
		  if(KeyCommand[0]!=0)
		  {
			  switch(KeyCommand[0])
			  {
				  case '0':
					  TMCsetup();
					  KeyCommand[0]=0;
					  break;
				  case '1':
					  //HAL_UART_Transmit_IT(&huart2,(uint8_t *)front, sizeof(front));
					  printf("1:start\r\n");
					  prepareAbsoluteMovement(20000);
					  runAndWait();
					  HAL_Delay(100);
					  printf("1:end\r\n");
					  KeyCommand[0]=0;
					  break;

				  case '2':
					  printf("1:start\r\n");
					  prepareAbsoluteMovement(-5000);
					  runAndWait();
					  HAL_Delay(100);
					  printf("1:end\r\n");
					  KeyCommand[0]=0;
					  break;

				  case '3':
					  stepperHoming();
					  runAndWait();
					  HAL_Delay(100);
					  KeyCommand[0]=0;

				  case '4':
					  AbsoluteReset();
					  KeyCommand[0]=0;
				  default:
					  printf("NoCommand");
					  KeyCommand[0]=0;
					  break;
			  }
			  get_uart_flag=0;
		  }
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

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
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
	printf("uart_timer\r\n");

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
