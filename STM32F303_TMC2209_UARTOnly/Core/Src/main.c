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

typedef struct{
	volatile int checkcount;
	volatile int countperiod;

}checkerInfo;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TMC2209_CRC(data, length) tmc_CRC8(data, length, 1)

//#define NUM_stepper 1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//static TMC2209TypeDef TMC2209; //TMC2209????????????????????????
//static ConfigurationTypeDef *TMC2209_config; //TMC2209?????????
TMC2209TypeDef TMC2209; //TMC2209????????????????????????
ConfigurationTypeDef TMC2209_config; //TMC2209?????????
volatile stepperInfo stepper[1];    //stepperInfo????????????stepper???NUM_stepper???????????????????????????
uint8_t uartflag1=0;
uint8_t end_stop_state=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
//static void periodicJob(uint32_t tick);
static uint8_t reset(void);
static uint8_t restore(void);
//static TMC2209TypeDef TMC2209;


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void tmc2209_readWriteArray(uint8_t channel, uint8_t *data, size_t writeLength, size_t readLength)
{
	//set your uart read write
	//UART_readWrite(channelToUART(channel), data, writeLength, readLength);
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	HAL_UART_Transmit(&huart1, data, writeLength,3000);
//	HAL_UART_Transmit_IT(&huart1, data, writeLength);


	if(readLength > 0){
		HAL_HalfDuplex_EnableReceiver(&huart1);
		HAL_UART_Receive(&huart1, data, readLength,3000);
//		HAL_UART_Receive_IT(&huart1, data, readLength);
	}
}

uint8_t tmc2209_CRC8(uint8_t *data, size_t length)
{
	return TMC2209_CRC(data, length);
}

static uint8_t reset()
{
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

void currentcheck(){
	int32_t SGTHRS_value = 0;
	int32_t stall_value=0;
	int32_t value=0;

//	stall_value=tmc2209_readInt(&TMC2209,0x06);
//	stall_value=TMC2209_FIELD_READ(&TMC2209, 0x06, TMC2209_DIAG_MASK, TMC2209_DIAG_SHIFT);
//	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

//	printf("stall: %ld \r\n",stall_value);
//	if(stall_value==1){
//		value = 0;
//		TMC2209_FIELD_UPDATE(&TMC2209, 0x22, TMC2209_VACTUAL_MASK, TMC2209_VACTUAL_SHIFT,value);
////		printf("stall:\r\n");
//		stall_value=0;
//		while(uartflag1!=1){
//
//				  }
//			  }
	//
//		tstep_value=TMC2209_FIELD_READ(&TMC2209, TMC2209_TSTEP, TMC2209_TSTEP_MASK, TMC2209_TSTEP_SHIFT);
//		printf("tstep: %ld\r\n", tstep_value);
//		tmc2209_periodicJob(&TMC2209, HAL_GetTick());

//		SGTHRS_value = tmc2209_readInt(&TMC2209, TMC2209_SG_RESULT);
//		printf("SG_RESULT: %d\r\n", SGTHRS_value);
//		tmc2209_periodicJob(&TMC2209, HAL_GetTick());


//		current_value=tmc2209_readInt(&TMC2209, TMC2209_SG_RESULT);	//????????????????????????
//		printf("SG_RESULT : %ld\r\n", current_value);


	//	value = 3;
	//	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_DIAG_MASK, TMC2209_DIAG_SHIFT,value);
	//	tmc2209_periodicJob(&TMC2209, HAL_GetTick());

		value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IOIN, TMC2209_DIAG_MASK, TMC2209_DIAG_SHIFT);
//		printf("DIAG : %ld\r\n", value);
		tmc2209_periodicJob(&TMC2209, HAL_GetTick());

}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//int32_t SGTHRS_value = 0;
	uartflag1 = 1;
	printf("uart_timer\r\n");
	//HAL_UART_Receive_IT(&huart2,(uint8_t *)rxbuf, 1);
	//SGTHRS_value = tmc2209_readInt(&TMC2209, TMC2209_SGTHRS);
	//currentdata();
}
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	//stepper 0 end stop senssor
//	if(GPIO_Pin == GPIO_PIN_6 ){
//		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){
//			end_stop_state |= 1;
//		}
//		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)){
//			end_stop_state &= ~1;
//		}
//	}
//}

void TMCsetup(){
	int32_t value = 0;

	value = 22;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT, value);	//????????????
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IRUN_MASK, TMC2209_IRUN_SHIFT);
	printf("IRUN: %ld\r\n", value);

	value= 16;																						//???????????????????????????????????????
	TMC2209_FIELD_UPDATE(&TMC2209,TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	HAL_Delay(100);
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLD_MASK, TMC2209_IHOLD_SHIFT);	//????????????
	printf("IHOLD: %ld\r\n", value);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK,TMC2209_PDN_DISABLE_SHIFT);
	printf("pdn_disable : %ld\r\n", value);
	value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK, TMC2209_PDN_DISABLE_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_PDN_DISABLE_MASK, TMC2209_PDN_DISABLE_SHIFT);
	printf("pdn_disable : %ld\r\n", value);

	value=300;
	tmc2209_writeInt(&TMC2209, TMC2209_TCOOLTHRS, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("TCOOLTHRS: %ld\r\n", value);

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK,TMC2209_EN_SPREADCYCLE_SHIFT);
	printf("SPREADCYCLE : %ld\r\n", value);
	value = 0;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK, TMC2209_EN_SPREADCYCLE_SHIFT ,value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_EN_SPREADCYCLE_MASK,TMC2209_EN_SPREADCYCLE_SHIFT);
	printf("SPREADCYCLE : %ld\r\n", value);

//	value=10;
//	tmc2209_writeInt(&TMC2209, TMC2209_TPWMTHRS, value);
//	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
//	printf("TPWMTHRS: %ld\r\n", value);

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

	value=0;
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK,TMC2209_MSTEP_REG_SELECT_SHIFT);	//uart??????microstep????????????
	printf("mstep_before : %ld\r\n", value);
	value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK, TMC2209_MSTEP_REG_SELECT_SHIFT,value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_GCONF, TMC2209_MSTEP_REG_SELECT_MASK, TMC2209_MSTEP_REG_SELECT_SHIFT);
	printf("mstep_after : %ld\r\n", value);

	value=0;
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT);	//????????????????????????
	printf("toff_before : %ld\r\n", value);
	value = 3;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT,value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TOFF_MASK, TMC2209_TOFF_SHIFT);
	printf("toff_after : %ld\r\n", value);

	value = 256 >> TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT);	//microstep?????????
	printf("microstep_before : %ld\r\n", value);
	value = 4;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT,value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value = 256 >> TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_MRES_MASK, TMC2209_MRES_SHIFT);
	printf("microstep_after : %ld\r\n", value);

	value=4;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLDDELAY_MASK, TMC2209_IHOLDDELAY_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value=TMC2209_FIELD_READ(&TMC2209, TMC2209_IHOLD_IRUN, TMC2209_IHOLDDELAY_MASK, TMC2209_IHOLDDELAY_SHIFT);
	printf("IHOLDDELAY: %ld\r\n", value);

	value=100;											//SG_RESULT??????????????????
	tmc2209_writeInt(&TMC2209, TMC2209_SGTHRS, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value = tmc2209_readInt(&TMC2209, TMC2209_SGTHRS);
	printf("SGTHRS: %ld\r\n", value);

	value = 100;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT, value);
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("PWM_GRAD: %ld\r\n", value);


	value=5;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEMIN_MASK, TMC2209_SEMIN_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("SEMIN: %ld\r\n", value);

	value = 2;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_COOLCONF, TMC2209_SEMAX_MASK, TMC2209_SEMAX_SHIFT, value);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("SEMAX: %ld\r\n", value);

	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	value = 1;
	TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT, value);
	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_CHOPCONF, TMC2209_TBL_MASK, TMC2209_TBL_SHIFT);
	printf("TBL: %ld\r\n", value);

	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_GRAD_MASK, TMC2209_PWM_GRAD_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("PWM_GRAD: %ld\r\n", value);

	value = TMC2209_FIELD_READ(&TMC2209, TMC2209_PWMCONF, TMC2209_PWM_AUTOSCALE_MASK, TMC2209_PWM_AUTOSCALE_SHIFT);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("autoscale: %ld\r\n", value);

	value = tmc2209_readInt(&TMC2209,TMC2209_PWM_AUTO);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("PWMAuto: %ld\r\n", value);

	value = tmc2209_readInt(&TMC2209,TMC2209_PWMSCALE);
	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("PWMSCALE: %ld\r\n", value);

	value = 0;
	value = TMC2209_FIELD_READ(&TMC2209, 0x07, TMC2209_FCLKTRIM_MASK, TMC2209_FCLKTRIM_SHIFT);

	tmc2209_periodicJob(&TMC2209, HAL_GetTick());
	printf("frequency: %ld\r\n", value);

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
//	char front[] ="front turn\r\n";
//	char back[] ="back turn\r\n";
	char nonans[] ="Not Understand \r\n";
	char rxbuf[1];
	uint32_t value=0;

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
  /* USER CODE BEGIN 2 */
  //int getserial=0;
  TMC2209_INIT();
//  TMCsetup();
  printf("Hello\r\n");
  HAL_UART_Transmit(&huart2,(uint8_t *)start,sizeof(start),3000);
  //HAL_TIM_Base_Start_IT(&htim2);
//  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);

  rxbuf[0]=0;
  uartflag1=0;
  uint8_t stall_value=0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  currentcheck();
	  //example read register value
	  //int32_t value = 0;
	  HAL_UART_Receive_IT(&huart2,(uint8_t *)rxbuf, 1);
//	  while(uartflag1==0){
//		  ;
//	  }
	  HAL_UART_Transmit(&huart2,(uint8_t *)rxbuf,sizeof(rxbuf),100);
//	  stall_value=0;
	  stall_value=TMC2209_FIELD_READ(&TMC2209, 0x06, TMC2209_DIAG_MASK, TMC2209_DIAG_SHIFT);
	  HAL_Delay(10);
//	  printf("stall: %d \r\n",stall_value);
	  if(stall_value==1){
		  value = 0;
		  TMC2209_FIELD_UPDATE(&TMC2209, 0x22, TMC2209_VACTUAL_MASK, TMC2209_VACTUAL_SHIFT,value);
		  printf("stall:\r\n");
		  stall_value=0;
		  while(uartflag1!=1){

		  }
	  }

	  if(rxbuf[0]!=0 && uartflag1==1){
		  switch(rxbuf[0]){
		  case's':
			  TMCsetup();
			  rxbuf[0]=0;
			  break;
		  case'a':
			  value = 0;
			  TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_SHAFT_MASK, TMC2209_SHAFT_SHIFT ,value);
			  rxbuf[0]=0;
			  break;
		  case'd':
			  value = 1;
			  TMC2209_FIELD_UPDATE(&TMC2209, TMC2209_GCONF, TMC2209_SHAFT_MASK, TMC2209_SHAFT_SHIFT ,value);
			  rxbuf[0]=0;
			  break;

		  case'1':
			  value = 10000;
			  TMC2209_FIELD_UPDATE(&TMC2209, 0x22, TMC2209_VACTUAL_MASK, TMC2209_VACTUAL_SHIFT,value);
			  rxbuf[0]=0;
			  break;

		  case'2':
			  value = 0;
			  TMC2209_FIELD_UPDATE(&TMC2209, 0x22, TMC2209_VACTUAL_MASK, TMC2209_VACTUAL_SHIFT,value);
			  rxbuf[0]=0;
			  break;

		  default:
			  HAL_UART_Transmit(&huart2,(uint8_t *)nonans, sizeof(nonans),3000);
			  rxbuf[0]=0;
			  break;

		  }
		  uartflag1=0;
	  }
	  if(end_stop_state==1){
		  value = 0;
		  TMC2209_FIELD_UPDATE(&TMC2209, 0x22, TMC2209_VACTUAL_MASK, TMC2209_VACTUAL_SHIFT,value);
		  end_stop_state=0;
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
//  HAL_UART_Transmit_IT(&huart2,(uint8_t *)ptr,len);

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
