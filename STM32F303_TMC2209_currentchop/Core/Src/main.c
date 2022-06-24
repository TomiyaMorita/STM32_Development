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
}stepperInfo;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TMC2209_CRC(data, length) tmc_CRC8(data, length, 1)
#define NUM_STEPPERS 1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//static TMC2209TypeDef TMC2209; //TMC2209の設定などの定義
//static ConfigurationTypeDef *TMC2209_config; //TMC2209の設定
TMC2209TypeDef TMC2209; //TMC2209の設定などの定義
ConfigurationTypeDef TMC2209_config; //TMC2209の設定
volatile stepperInfo steppers[NUM_STEPPERS];
uint8_t end_stop_state = 0;
uint8_t homing_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
static void periodicJob(uint32_t tick);
static uint8_t reset(void);
static uint8_t restore(void);
//static TMC2209TypeDef TMC2209;


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim3){ //htim3に設定したタイマー割り込みの時間で発火する
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);//ピンの出力を切り替え
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

static void periodicJob(uint32_t tick)
{

	tmc2209_periodicJob(&TMC2209, tick);

	//StepDir_periodicJob(0);
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
	if(GPIO_Pin == GPIO_PIN_10 ){
		if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)){
			end_stop_state |= 1;
		}
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)){
			end_stop_state &= ~1;
		}
	}
}

void sensorStateInit(){							//割り込みリセット
	if( ! HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)){	//割り込みピンが真でない(1でない=0なら)
		end_stop_state |= 1;					//end_stop_stateに左辺か右辺が1なら1を代入
	}
}



void Step0(){
	//pull+

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	/*
	 * pull-
	 * HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	 * HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	 */
}

void Dir0(int dir){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, dir);
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
	printf("prepareMovement:\r\n");
}

void prepareAbsoluteMovement(int whichMotor, int absolute_steps){
	printf("absolute_steps:%d\r\n",absolute_steps);
	volatile stepperInfo* si = &steppers[whichMotor];
	int steps = absolute_steps - si->stepPosition;
	if(steps == 0){
		return;
	}
	si->dirFunc(steps < 0 ? 1 : 0);
	si->dir = steps > 0 ? 1:-1;
	si->totalSteps = abs(steps);
	resetStepper(si);
	remainingSteppersFlag |= (1 << whichMotor);
	printf("prepareAbsoluteMovement:\r\n");
	printf("steps:%d\r\n",steps);
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
	//printf("mind:%d\r\n",mind);
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	//HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_1);
	unsigned int tmpCtr = __HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1);		//現在のコンペアの値を入れている
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 65500);


	for (int i = 0; i < NUM_STEPPERS; i++){
		if ( ! ( (1 << i) & remainingSteppersFlag )){
			continue;
		}

		if( homing_flag & (1 << i)){
			if(end_stop_state & (1 << i)){
				remainingSteppersFlag &= ~(1 << i);
				continue;
			}
		}

		if ( ! (nextStepperFlag & (1 << i)) ){
			steppers[i].di -= tmpCtr;
			continue;
		}

		volatile stepperInfo* s = &steppers[i];
		//printf("callback\n\r");


		if( (s->stepCount) < (s->totalSteps) ){
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
		//printf("s->di:%ld \r\n",s->di);
	}

	setNextInterruptInterval();

	__HAL_TIM_SET_COUNTER(&htim3, 0);


}


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
	printf("homing:\r\n");

}


void TMCsetup(){
	int32_t mstep_value = 0;
	int32_t toff_value = 0;
	int32_t microstep_value = 0;
	int32_t stanby_value = 0;
	int32_t max_value = 0;
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

	stanby_value=TMC2209_FIELD_READ(&TMC2209,TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT);	//待機電流の設定
	printf("stanbycurrent_before: %ld\r\n", stanby_value);
	stanby_value = 10;
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT, stanby_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(200);
	stanby_value=TMC2209_FIELD_READ(&TMC2209,TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT);
	printf("stanbycurrent_after: %ld\r\n", stanby_value);

	max_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT);	//最大電流の設定
	printf("maxumum_before: %ld\r\n", max_value);
	max_value = 10;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT, max_value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(200);
	max_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT);
	printf("maxumum_after: %ld\r\n", max_value);

}

int uartflag1=0;
char rxbuf[1];
void motorrotate()
{
	char motorrotate[] ="motorrotate\r\n";
	HAL_UART_Transmit(&huart2,(uint8_t *)motorrotate,sizeof(motorrotate),3000);

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
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  steppers[0].dirFunc = Dir0;
  steppers[0].stepFunc = Step0;
  steppers[0].acceleration = 5000;
  steppers[0].minStepInterval = 100;
  steppers[0].homing = 0;
  steppers[0].dir_inv = 1;
  steppers[0].seeking_vel = 100;
  steppers[0].homing_vel = 200;
  steppers[0].pull_off = 500;
     //Stepper 1 enable
    //pull+
  //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
    //pull-
    //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);


  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);
  //int getserial=0;
  TMC2209_INIT();
  TMCsetup();

  printf("Hello\r\n");

  int i;
   char rxbuf[1];
   char start[] ="1:front,2:back\r\n";
   char front[] ="front turn\r\n";
   char back[] ="back turn\r\n";
   char nonans[] ="Not Understand \r\n";



   //HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);


  //printf("Start \r\n");


  //uint8_t rxbuf[1];
   //int32_t *value=3;

   //TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT, 3 );

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //example read register value
	  //int32_t value = 0;
	  HAL_UART_Receive(&huart2,(uint8_t *)rxbuf, sizeof(rxbuf), 100);
	  //tmc2209_writeInt(&TMC2209, TMC2209_TSTEP, 16);

	  	  //tmc2209_writeInt(&TMC2209, TMC2209_ENN_MASK, 0);


	  if (rxbuf[0] != 0){
	  		printf("rxbuf : %d\r\n",rxbuf[0]);

	  	if(rxbuf[0] == '1'){
	  		//HAL_UART_Transmit_IT(&huart2,(uint8_t *)front, sizeof(front));
	  		printf("1:start");
	  		prepareAbsoluteMovement(0,5000);
	  		runAndWait();
	  		HAL_Delay(100);
	  		prepareAbsoluteMovement(0,-5000);
	  		runAndWait();
	  		HAL_Delay(100);
	  		printf("1:end");


	  		rxbuf[0]=0;

	  		}
	  	else if(rxbuf[0] == '2'){

	  		//HAL_UART_Transmit_IT(&huart2,(uint8_t *)back, sizeof(back));
	  		runAndWait();
	  		HAL_Delay(100);
	  		//HAL_Delay(100);

	  		rxbuf[0]=0;
	  	}
	  	else{
	  		HAL_UART_Transmit_IT(&huart2,(uint8_t *)nonans, sizeof(nonans));
	  		rxbuf[0]=0;
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
  htim3.Init.Prescaler = 1280-1;
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
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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

  /*Configure GPIO pin : PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart2,(uint8_t *)ptr,len,10);
  return len;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	motorrotate();

	HAL_UART_Receive_IT(&huart2,(uint8_t *)rxbuf, 1);
	uartflag1 = 1;

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
