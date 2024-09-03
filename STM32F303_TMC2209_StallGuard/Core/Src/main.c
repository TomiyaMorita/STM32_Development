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

//#define NUM_STEPPERS 1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//static TMC2209TypeDef TMC2209; //TMC2209の設定などの定義
//static ConfigurationTypeDef *TMC2209_config; //TMC2209の設定
TMC2209TypeDef TMC2209; //TMC2209の設定などの定義
ConfigurationTypeDef TMC2209_config; //TMC2209の設定

volatile stepperInfo steppers[1];    //stepperInfo型の配列stepperにNUM_STEPPERSで定義した値を代入
uint8_t end_stop_state = 0;
uint8_t homing_flag = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
//static void periodicJob(uint32_t tick);
static uint8_t reset(void);
static uint8_t restore(void);
//static TMC2209TypeDef TMC2209;


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim3){ //htim3に設定したタイマー割り込みの時間で発火する
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);//ピンの出力を切り替え
    }
}*/
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
}


void Step0(){                                       //step pin
	//pull+

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
}

void Dir0(int dir){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, dir); //Dir0関数にdirピンを指定して格納
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

			if( (remainingSteppersFlag) && steppers[0].di < mind ){
				mind = steppers[0].di;
			}
		nextStepperFlag = 0;
			if( 1 && steppers[0].di == mind ){
				nextStepperFlag |= 1;
			}


			if (remainingSteppersFlag == 0){
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65500);
			}

		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, mind);
		//printf("nownompare:%d\r\n",mind);

}

void currentcheck(){
	int32_t current_value = 0;
	int32_t DIAG_value = 0;
	int32_t tstep_value = 0;
	int32_t SGTHRS_value = 0;

//
//	tstep_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_TSTEP, TMC2209_TSTEP_MASK, TMC2209_TSTEP_SHIFT);
//	printf("tstep: %ld\r\n", tstep_value);
//	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

//	SGTHRS_value = tmc2209_readInt(&TMC2209, TMC2209_SGTHRS);
//	printf("SGTHRS: %d\r\n", SGTHRS_value);

	current_value=tmc2209_readInt(&TMC2209, TMC2209_SG_RESULT);	//保持トルクの設定
	printf("SG_RESULT : %ld\r\n", current_value);

//	current_value = 3;
//	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_DIAG_MASK, TMC2209_DIAG_SHIFT,toff_value);
//	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

//	DIAG_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_DIAG_MASK, TMC2209_DIAG_SHIFT);
//	printf("DIAG : %ld\r\n", DIAG_value);
//	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

}


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){      //指定したコンペアを超えた時発火
	//HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_1);
	if(htim == &htim3){
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65500);			//コンペアを65500にセット
				//nextStepperFlagが真でない(0なら)
		//printf("steppers:%ld \r\n",steppers[0].di);
		volatile stepperInfo* s = &steppers[0];
		if( (s->stepCount) < (s->totalSteps) ){		//1周ごとにカウントしたstepCountがabs(steps)より小さかったら==動作が終了するまで
			s->stepFunc();
			s->stepCount++;							//カウントし続ける
			s->stepPosition += s->dir;		//dir(+もしくはー(dirの向き）を加算してstepPositionに格納
			if ( (s->stepCount) >= (s->totalSteps) ){	//stepCountがtotalStepsを超えたとき
				s->movementDone = 1;	//movementDoneを1にする
				remainingSteppersFlag &= ~1;

			}
		}
		setNextInterruptInterval();
		__HAL_TIM_SET_COUNTER(&htim3, 0);


	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2){
//		currentcheck();

    }
}


void sensorStateInit(){							//割り込みリセット
	if( ! HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){	//割り込みピンが真でない(1でない=0なら)
		end_stop_state |= 1;					//end_stop_stateに左辺か右辺が1なら1を代入
	}
}

void runAndWait(){
	setNextInterruptInterval();
	while(remainingSteppersFlag);
}



void TMCsetup(){
	int32_t mstep_value = 0;
	int32_t toff_value = 0;
	int32_t microstep_value = 0;
	int32_t IRUN_value = 0;
	int32_t IHOLD_value = 0;
	int32_t IHOLDDELAY_value= 0;
	int32_t buffer = 0;
	int32_t stallgard_setup_value=0;
	int32_t TCOOLTHRS_value=0;
	int32_t buffer_TPWMTHRS=0;
	int32_t TPWMTHRS_value=0;
	int32_t SEMIN_value=0;
	int32_t SEMAX_value=0;
	int32_t SEIMIN_scale_value=0;
//	int32_t FCLKTRIM_scale_value=0;

//	FCLKTRIM_scale_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_FACTORY_CONF, TMC2209_FCLKTRIM_MASK, TMC2209_FCLKTRIM_SHIFT);
//	FCLKTRIM_scale_value=31;
//	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_FACTORY_CONF, TMC2209_FCLKTRIM_MASK, TMC2209_FCLKTRIM_SHIFT, FCLKTRIM_scale_value);
//	printf("FCLKTRIM: %ld\r\n", FCLKTRIM_scale_value);

	mstep_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK,TMC2209_MSTEP_REG_SELECT_SHIFT);	//uartでのmicrostepの有効化
	printf("mstep_before : %ld\r\n", mstep_value);
	mstep_value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK, TMC2209_MSTEP_REG_SELECT_SHIFT,mstep_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	mstep_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK, TMC2209_MSTEP_REG_SELECT_SHIFT);
	printf("mstep_after : %ld\r\n", mstep_value);

	toff_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT);	//保持トルクの設定
	printf("toff_before : %ld\r\n", toff_value);
	toff_value = 4;
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

	IHOLD_value=3;																							//モーター待機時の電流の設定
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT, IHOLD_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	IHOLD_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT);	//保持電流
	printf("IHOLD: %ld\r\n", IHOLD_value);

	IRUN_value = 18;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT, IRUN_value);	//実行電流
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	IHOLD_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT);
	printf("IRUN: %ld\r\n", IRUN_value);

	IHOLDDELAY_value=4;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLDDELAY_MASK, TMC2209_IHOLDDELAY_SHIFT, IHOLDDELAY_value);
	IHOLDDELAY_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLDDELAY_MASK, TMC2209_IHOLDDELAY_SHIFT);
	printf("IHOLDDELAY: %ld\r\n", IHOLDDELAY_value);

	stallgard_setup_value=300;											//SG_RESULTと比較して、
	tmc2209_writeInt(&TMC2209, TMC2209_SGTHRS, stallgard_setup_value);
	stallgard_setup_value = tmc2209_readInt(&TMC2209, TMC2209_SGTHRS);
	printf("SGTHRS: %ld\r\n", stallgard_setup_value);
//	buffer = tmc2209_readInt(&TMC2209, TMC2209_TCOOLTHRS);
	TCOOLTHRS_value=1500;
////	TCOOLTHRS_value = MIN(0xFFFFF, (1<<24) / ((buffer) ? buffer : 1));
////	TCOOLTHRS_value -= (TCOOLTHRS_value) >> 3;
////				if (TCOOLTHRS_value)
////				{
////					TCOOLTHRS_value= MIN(0x000FFFFF, (1<<24) / (TCOOLTHRS_value));
////				}
////				else
////				{
////					TCOOLTHRS_value = 0x000FFFFF;
////				}
	tmc2209_writeInt(&TMC2209, TMC2209_TCOOLTHRS, TCOOLTHRS_value);
	printf("TCOOLTHRS: %ld\r\n", TCOOLTHRS_value);

//	buffer_TPWMTHRS=0;
//	buffer_TPWMTHRS = tmc2209_readInt(&TMC2209, TMC2209_TPWMTHRS);
//	TPWMTHRS_value = MIN(0xFFFFF, (1<<24) / ((buffer_TPWMTHRS) ? buffer_TPWMTHRS : 1));
//
//	TPWMTHRS_value = MIN(0xFFFFF, (1<<24) / ((TPWMTHRS_value) ? TPWMTHRS_value : 1));

	TPWMTHRS_value=100;
	tmc2209_writeInt(&TMC2209, TMC2209_TPWMTHRS, TPWMTHRS_value);
	printf("TPWMTHRS: %ld\r\n", TPWMTHRS_value);

	//SEMIN_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_COOLCONF, TMC2209_SEIMIN_MASK, TMC2209_SEIMIN_SHIFT);

	SEMIN_value=1;	//IRUNのスケールを1/4
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEIMIN_MASK, TMC2209_SEIMIN_SHIFT, SEMIN_value);
	printf("SEMIN: %ld\r\n", SEMIN_value);
//
//	//SEMAX_value = TMC2209_FIELD_READ(&TMC2209, TMC2209_COOLCONF, TMC2209_SEMAX_MASK, TMC2209_SEMAX_SHIFT);
	SEMAX_value = 10;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEIMIN_MASK, TMC2209_SEIMIN_SHIFT, SEMIN_value);
	printf("SEMAX: %ld\r\n", SEMAX_value);

	SEIMIN_scale_value=1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEIMIN_MASK, TMC2209_SEIMIN_SHIFT, SEIMIN_scale_value);
	printf("SEIMIN_scal: %ld\r\n", SEIMIN_scale_value);





}

uint8_t uartflag1=0;
void motorrotate()
{
	//char motorrotate_TM[] ="motorrotate_TM\r\n";
	//HAL_UART_Transmit(&huart2,(uint8_t *)motorrotate_TM,sizeof(motorrotate_TM),3000);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	char rxbuf[1];
//	HAL_UART_Receive_IT(&huart2,(uint8_t *)rxbuf, 1);
//	HAL_UART_Transmit(&huart2,(uint8_t *)rxbuf,sizeof(rxbuf),3000);
	//motorrotate();
	uartflag1 = 1;
	printf("uart_timer\r\n");


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
	//uint8_t buffer[256];
	//char rxbuf[1];
	char start[] ="1:front,2:back\r\n";
	char front[] ="front turn\r\n";
	char back[] ="back turn\r\n";
	char nonans[] ="Not Understand \r\n";
	char rxbuf[1];

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
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  //int getserial=0;
  TMC2209_INIT();
  TMCsetup();
  printf("Hello\r\n");
  HAL_UART_Transmit(&huart2,(uint8_t *)start,sizeof(start),3000);

  //printf("Start \r\n");


  //uint8_t rxbuf[1];
   //int32_t *value=3;

   //TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT, 3 );
   //Set stepper 1
   steppers[0].dirFunc = Dir0;
   steppers[0].stepFunc = Step0;
   steppers[0].acceleration = 100-1;
   steppers[0].minStepInterval = 100;
   steppers[0].homing = 0;
   steppers[0].dir_inv = 1;
   steppers[0].seeking_vel = 100;
   steppers[0].homing_vel = 200;
   steppers[0].pull_off = 100;
     //Stepper 1 enable
    //pull+
   //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);  //ena pin
    //pull-w
    //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);

//   HAL_TIM_Base_Start_IT(&htim2);
   HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);

   //HAL_UART_Transmit(&huart2,(uint8_t *)rxbuf,sizeof(rxbuf),3000);

	rxbuf[0]=0;
	uartflag1=0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //example read register value
	  //int32_t value = 0;
	  HAL_UART_Receive_IT(&huart2,(uint8_t *)rxbuf, 1);
//	  while(uartflag1==0){
//		  ;
//	  }
	  HAL_UART_Transmit(&huart2,(uint8_t *)rxbuf,sizeof(rxbuf),3000);
	  if(uartflag1==1){
		  if (rxbuf[0] != 0){
			  if(rxbuf[0] == '1'){
				  //HAL_UART_Transmit_IT(&huart2,(uint8_t *)front, sizeof(front));
				  printf("1:start\r\n");
				  prepareAbsoluteMovement(10000);
				  runAndWait();
				  HAL_Delay(100);
				  prepareAbsoluteMovement(0);
				  runAndWait();
				  HAL_Delay(100);
				  printf("1:end\r\n");
				  rxbuf[0]=0;

			  }
			  else if(rxbuf[0] == '2'){
				  //HAL_UART_Transmit_IT(&huart2,(uint8_t *)back, sizeof(back));
				  runAndWait();
				  HAL_Delay(100);

				  rxbuf[0]=0;
			  }
			  else{
				  HAL_UART_Transmit_IT(&huart2,(uint8_t *)nonans, sizeof(nonans));
				  rxbuf[0]=0;
			  }
			  uartflag1=0;
		  }
	  }

	  //HAL_Delay(100);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
  htim2.Init.Prescaler = 640-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 500-1;
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
  htim3.Init.Prescaler = 640-1;
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
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart2,(uint8_t *)ptr,len,10);
  return len;
}



//int _write(int file, char *ptr, int len)
//{
//  int DataIdx;
//  for(DataIdx=0; DataIdx<len; DataIdx++)
//  {
//    ITM_SendChar(*ptr++);
//  }
//  return len;
//}

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
