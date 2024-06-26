
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

#include "KEYPAD.h"
#include "ssd1306.h"
#include "ssd1306_font.h"
#include "fingerprint.h"

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
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart5;

/* USER CODE BEGIN PV */


// -----------------------------------
//		CONFIGURATION
//
// -- BUZZER on PD3
//
// -- RELAY on PE6
//
// -- KEYPAD
// -- rows     PB0-PB1-PB2-PB4
// -- columns  PB5-PB6-PB7-PB8

// display SDA PA10  - SCL PA9
//
// -----------------------------------


// -- MAPPING TASTI KEYPAD
#define KEY_1       0
#define KEY_2       1
#define KEY_3       2
#define KEY_A       3
#define KEY_4       4
#define KEY_5       5
#define KEY_6       6
#define KEY_B       7
#define KEY_7       8
#define KEY_8       9
#define KEY_9       10
#define KEY_C       11
#define KEY_S       12
#define KEY_0       13
#define KEY_H       14
#define KEY_D       15

// -- VARIABILI GLOBALI
uint8_t gu8_KeyStatesArr[KEYS] = {0};
uint8_t password[4] = {0};						// password inserita
uint8_t correct_psw[4] = {'1','7','2','8'};		// password corretta
volatile uint8_t counter = 0; 					// contatore char immessi
uint8_t keypress = 0; 							// FLAG button press | 1 se premuto - 0 altrimenti

uint8_t IDFromFinger;
uint8_t CurrentNumberFinger;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_UART5_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

uint8_t FingerResult;

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
  MX_I2C2_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */


  // inizializzazione KEYPAD
  KEYPAD_Init(0, gu8_KeyStatesArr);

  // inizializzazione buzzer e relay
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, 0);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, 0);


  // -- DISPLAY CONFIG --
  ssd1306_init();
  //scritta
  ssd1306_write_string(font11x18, "PASSWORD:");
  ssd1306_enter();
  ssd1306_enter();
  // show
  ssd1306_update_screen();


  CurrentNumberFinger=GetNumberOfFinger();

    if(CurrentNumberFinger>100)
  	{
  		CurrentNumberFinger=1;
  	}


     ProcessRegistryNewFinger();
     HAL_Delay(1000); // Attendi un secondo prima di ripetere la verifica


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

	  FingerResult=CheckFinger();

	  	if(FingerResult==FP_OK)
	  	{
	  		//IMPRONTA TROVATA APRO LA SERRATURA
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, 1);  	// relay su PE6


			  // -- DISPLAY --
			  ssd1306_init();
			  //scritta
			  ssd1306_write_string(font11x18, " ");
			  ssd1306_enter();
			  ssd1306_write_string(font16x26, "CORRECT");
			  // show
			  ssd1306_update_screen();


			  HAL_Delay(1000);

			  // -- DISPLAY CONFIG --
			  ssd1306_init();
			  //scritta
			  ssd1306_write_string(font11x18, "PASSWORD:");
			  ssd1306_enter();
			  ssd1306_enter();
			  // show
			  ssd1306_update_screen();

	  	}
	  	else if(FingerResult==FP_FINGER_NOTFOUND)
	  	{
	  		//IMPRONTA NON TROVATA SUONA ALLARME

	  		// -- INIT DISPLAY --
			  ssd1306_init();
			  //scritta
			  ssd1306_write_string(font11x18, " ");
			  ssd1306_enter();
			  ssd1306_write_string(font16x26, " ERROR");
			  // show
			  ssd1306_update_screen();

			  // PASSWORD SBAGLIATA
			  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_SET); // buzzer su PD3
			  HAL_Delay(100); // suona per 1 second0
			  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
			  HAL_Delay(100);

			  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_SET); // buzzer su PD3
			  HAL_Delay(100); // suona per 1 second0
			  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
			  HAL_Delay(100);

			  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_SET); // buzzer su PD3
			  HAL_Delay(100); // suona per 1 second0
			  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);

			  HAL_Delay(1000);

			  // -- DISPLAY CONFIG --
			  ssd1306_init();
			  //scritta
			  ssd1306_write_string(font11x18, "PASSWORD:");
			  ssd1306_enter();
			  ssd1306_enter();
			  // show
			  ssd1306_update_screen();


	  	}

///CODICE KEYPAD

	  // -- ABILITO LINEE DI INTERRUZIONE --
	  if(HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_5)==1){
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 1);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 1);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);
	  }
	  else if(HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_6)==1){
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 0);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 1);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);
	  }
	  else if(HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_7)==1){
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 1);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);
	  	  }
	  else if(HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_8)==1){
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 1);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 1);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);
	  	  }


	  // incremento contatore ogni bottone premuto e azzero il flag
	  if(keypress == 1){
		  counter++;

		  // -- DISPLAY CONFIG --
		  ssd1306_write_string(font16x26, "* ");
		  // show
		  ssd1306_update_screen();

		  keypress = 0;
	  }


	  // -- CHECK PASSWORD --
	  // entro dopo aver inserito 4 caratteri
	  if(4 == counter){

		  int i = 0; //contatore caratteri confrontati

		  while(i<4){
			  if(password[i] == correct_psw[i]){
				  i++;
			  }
			  else {

				  // -- INIT DISPLAY --
				  ssd1306_init();
				  //scritta
				  ssd1306_write_string(font11x18, " ");
				  ssd1306_enter();
				  ssd1306_write_string(font16x26, " ERROR");
				  // show
				  ssd1306_update_screen();


				  // PASSWORD SBAGLIATA
				  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_SET); // buzzer su PD3
				  HAL_Delay(100); // suona per 1 second0
				  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
				  HAL_Delay(100);

				  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_SET); // buzzer su PD3
				  HAL_Delay(100); // suona per 1 second0
				  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
				  HAL_Delay(100);

				  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_SET); // buzzer su PD3
				  HAL_Delay(100); // suona per 1 second0
				  HAL_GPIO_WritePin (GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
				  break;
			  }
		  }

		  // SBLOCCO - BLOCCO SERRATURA
		  if(i == 4){

			  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);  	// relay su PE6

			  // -- DISPLAY --
			  ssd1306_init();
			  //scritta
			  ssd1306_write_string(font11x18, " ");
			  ssd1306_enter();
			  ssd1306_write_string(font16x26, "CORRECT");
			  // show
			  ssd1306_update_screen();
		  }


		  counter = 0;

		  // -- INIT DISPLAY --
		  HAL_Delay(1700);
		  ssd1306_init();
		  //scritta
		  ssd1306_write_string(font11x18, "PASSWORD:");
		  ssd1306_enter();
		  ssd1306_enter();
		  // show
		  ssd1306_update_screen();


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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_UART5|RCC_PERIPHCLK_I2C2;
  PeriphClkInit.Uart5ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x0000020B;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 57600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin|GPIO_PIN_6|LD4_Pin|LD3_Pin
                          |LD5_Pin|LD7_Pin|LD9_Pin|LD10_Pin
                          |LD8_Pin|LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pins : CS_I2C_SPI_Pin PE6 LD4_Pin LD3_Pin
                           LD5_Pin LD7_Pin LD9_Pin LD10_Pin
                           LD8_Pin LD6_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin|GPIO_PIN_6|LD4_Pin|LD3_Pin
                          |LD5_Pin|LD7_Pin|LD9_Pin|LD10_Pin
                          |LD8_Pin|LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_SCK_Pin SPI1_MISO_Pin SPI1_MISOA7_Pin */
  GPIO_InitStruct.Pin = SPI1_SCK_Pin|SPI1_MISO_Pin|SPI1_MISOA7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DM_Pin DP_Pin */
  GPIO_InitStruct.Pin = DM_Pin|DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF14_USB;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB5 PB6 PB7 PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){


		// controllo LETTURA PIN colonne
		if(GPIO_Pin == (GPIO_PIN_5) || GPIO_Pin == (GPIO_PIN_6) ||  GPIO_Pin == (GPIO_PIN_7) || GPIO_Pin == (GPIO_PIN_8)){

			// inizializzazione keypad
			KEYPAD_Init(0, gu8_KeyStatesArr);

			// leggo il bottone premuto
			KEYPAD_Scan(0);

			// button 1 
			if(gu8_KeyStatesArr[KEY_1] == KEY_PRESSED){
				password[counter]='1';
				keypress = 1;
			}
			
			// button 2
			else if(gu8_KeyStatesArr[KEY_2] == KEY_PRESSED){
				password[counter]='2';
				keypress = 1;
			}

			// button 3
			else if(gu8_KeyStatesArr[KEY_3] == KEY_PRESSED){
				password[counter]='3';
				keypress = 1;
			}

			// button A
			else if(gu8_KeyStatesArr[KEY_A] == KEY_PRESSED){
				password[counter]='A';
				keypress = 1;
			}

			// button 4
			else if(gu8_KeyStatesArr[KEY_4] == KEY_PRESSED){
				password[counter]='4';
				keypress = 1;
			}

			// button 5
			else if(gu8_KeyStatesArr[KEY_5] == KEY_PRESSED){
				password[counter]='5';
				keypress = 1;
			}

			// button 6
			else if(gu8_KeyStatesArr[KEY_6] == KEY_PRESSED){
				password[counter]='6';
				keypress = 1;
			}

			// button B
			else if(gu8_KeyStatesArr[KEY_B] == KEY_PRESSED){
				password[counter]='B';
				keypress = 1;
			}

			// button 7
			else if(gu8_KeyStatesArr[KEY_7] == KEY_PRESSED){
				password[counter]='7';
				keypress = 1;
			}

			// button 8
			else if(gu8_KeyStatesArr[KEY_8] == KEY_PRESSED){
				password[counter]='8';
				keypress = 1;
			}

			// button 9
			else if(gu8_KeyStatesArr[KEY_9] == KEY_PRESSED){
				password[counter]='9';
				keypress = 1;
			}

			// button C
			else if(gu8_KeyStatesArr[KEY_C] == KEY_PRESSED){
				password[counter]='C';
				keypress = 1;
			}

			// button S
			else if(gu8_KeyStatesArr[KEY_S] == KEY_PRESSED){
				password[counter]='S';
				keypress = 1;
			}

			// button 0
			else if(gu8_KeyStatesArr[KEY_0] == KEY_PRESSED){
				password[counter]='0';
				keypress = 1;
			}

			// button H
			else if(gu8_KeyStatesArr[KEY_H] == KEY_PRESSED){
				password[counter]='H';
				keypress = 1;
			}

			// button D
			else if(gu8_KeyStatesArr[KEY_D] == KEY_PRESSED){
				password[counter]='D';
				keypress = 1;
			}

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
