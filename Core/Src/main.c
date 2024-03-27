/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "lcd.h"
#include "math.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int ch)
{
    if (ch == '\n') {
        uint8_t ch2 = '\r';
        HAL_UART_Transmit(&huart2, &ch2, 1, HAL_MAX_DELAY);
    }
    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
    return 1;
}

int __io_getchar(void) {
    uint8_t ch;
        HAL_UART_Receive(&huart2, &ch, 1, HAL_MAX_DELAY);
        if (ch == '\r\n') {
            HAL_UART_Receive(&huart2, &ch, 1, HAL_MAX_DELAY);
        }
        HAL_UART_Transmit(&huart2, &ch, 1, HAL_MAX_DELAY);
        return ch;
}

int _read( char *buffor, int buffor_size)
{
    int count = 0;
    char ch;
    while (1)
    {
        ch = __io_getchar();
        if (ch == '\r')
        {
            break;
        }
        buffor[count++] = ch;
    }
    buffor[count] = '\0';
    if(count >= buffor_size){
    	count =0;
    }
    return count;
}

uint8_t customChar[7][8] = {
		{		  0b00000,
				  0b00000,
				  0b00000,
				  0b00000,
				  0b00100,
				  0b00110,
				  0b10101,
				  0b01110},

		{		  0b00100,
				  0b00110,
				  0b10101,
				  0b01110,
				  0b00000,
				  0b00000,
				  0b00000,
				  0b00000},

		{		  0b00000,
				  0b00000,
				  0b00000,
				  0b00000,
				  0b00000,
				  0b01110,
				  0b01110,
				  0b00000},

		{		  0b00000,
				  0b01110,
				  0b01110,
				  0b00000,
				  0b00000,
				  0b00000,
				  0b00000,
				  0b00000},

		{		  0b00000,
				  0b01110,
				  0b01110,
				  0b00000,
				  0b00100,
				  0b00110,
				  0b10101,
				  0b01110},

		{		  0b00100,
				  0b00110,
				  0b10101,
				  0b01110,
				  0b00000,
				  0b01110,
				  0b01110,
				  0b00000},

		{		  0b00000,
				  0b01110,
				  0b01110,
				  0b00000,
				  0b00000,
				  0b01110,
				  0b01110,
				  0b00000},
};

void create_char(uint8_t start_location, uint8_t char_table[][8], uint8_t num_chars)
{
    for (uint8_t j = 0; j < num_chars; j++) {
        uint8_t location = start_location + j;
        lcd_cmd(0x40 | (location << 3));
        for (int i = 0; i < 8; i++)
        {
            lcd_char_cp(char_table[j][i]);
        }
    }
}


char lcdOut[16];
char char_table [5][17];
int position_y = 1;
int position_x = 1;
int line=1;
int counter = 0;
int notzero = 0;
int win = 0;
int pud = 0;
int winning_condition_counter=12;
int shoot_counter = 0;

void Print_in_lcd()
{
	if(position_y>=3){
			line=2;
		}else{
			line=1;
		}
			if(!notzero){
				lcd_clear();
			}
	    	if(position_y%2==0)
	    	{
	    		lcd_char(line,position_x,0x00);
	    	}
	    	else
	    	{
	        	lcd_char(line,position_x,0x01);
	    	}
			HAL_Delay(500);
}

void positioning(){
	if(position_y>=3){
		line=2;
	}else{
		line=1;
	}
	if(pud == 4){
		lcd_char(line,position_x,0x03);
	}else if(pud == 5){
		lcd_char(line,position_x,0x02);
	}else{
	lcd_print(line,position_x," ");
	}
	pud = 0;
}

int tab[4][16];
void init_tab(){
	for(int i=0 ; i<4 ; i++){
		for(int j=0 ; j<16 ; j++){
				tab[i][j]=0;
			}
	}
}

void printtab(){
    printf(" ");
    for(int i=0 ; i<16 ; i++){
        printf("%3d", i+1);
    }
    printf("\n");
    for(int i=0 ; i<4 ; i++){
        printf("%d", i+1);
        for(int j=0 ; j<16 ; j++){
                if(tab[i][j]==0 || tab[i][j]==1)
                {
                    printf("%3c", '-');
                }
                else if(tab[i][j]==2)
                {
                    printf("%3c", 'X');
                }
            }
        printf("\n");
    }
}

void menu(){
	printf("\r\n\r\n");
	printf("  ____       _      _____   _____   _       _____  \r\n");
	printf(" | __ )     / \    |_   _| |_   _| | |     | ____| \r\n");
	printf(" |  _ \    / _ \     | |     | |   | |     |  _|   \r\n");
	printf(" | |_) |  / ___ \    | |     | |   | |___  | |___  \r\n");
	printf(" |____/  /_/   \_\   |_|     |_|   |_____| |_____| \r\n\r\n");
	lcd_print(1, 6, "BattleShip");
	printf("Press SELECT on LCD to start the game\r\n\r\n");
}

int check(){
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	HAL_ADC_Stop(&hadc1);
	uint32_t val = HAL_ADC_GetValue(&hadc1);
	float volt = (float)val * 3.3 / (pow(2,12)-1);
	//Select
	if(volt > 2.2 && volt < 2.5){
		return 0;
	}else{
		return 1;
	}
}

int Keyboard()
{
	HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    HAL_ADC_Stop(&hadc1);
    uint32_t val = HAL_ADC_GetValue(&hadc1);
    float volt = (float)val * 3.3 / (pow(2,12)-1);
    if(tab[position_y][position_x-1] == 2 && position_y%2==1){
    	pud = 5;
    }else if(tab[position_y-2][position_x-1] == 2 && position_y%2==0){
    	pud = 4;
    }
    //Select
    if(volt > 2.2 && volt < 2.5){
    	return 3;
    }
    //Left
    else if(volt > 1.5 && volt < 1.9 && position_x>1 && tab[position_y-1][position_x-2] != 2){
    	if(notzero==1){
    		positioning();
    		--position_x;
    		if(tab[position_y-2][position_x-1] == 2 && position_y%2==0){
    			lcd_char(line,position_x,0x04);
    			pud = 4;
    			HAL_Delay(500);
    		}else if(tab[position_y][position_x-1] == 2 && position_y%2==1){
    			lcd_char(line,position_x,0x05);
    			pud = 5;
    			HAL_Delay(500);
    		}else{
    			Print_in_lcd();
    		}
    		counter++;
    		}else{
    			--position_x;
    			Print_in_lcd();
    		}
    }
    //Down
    else if(volt > 0.9 && volt < 1.3 && position_y<4 && tab[position_y][position_x-1] != 2){
    	if(notzero==1){
    		positioning();
        	++position_y;
        	if(tab[position_y][position_x-1] == 2 && position_y==3){
        		lcd_char(line+1,position_x,0x05);
        		pud = 5;
        		HAL_Delay(500);
        	}else{
        		Print_in_lcd();
        	}
        	counter++;
    	}else{
        	++position_y;
        	Print_in_lcd();
    	}

    }
    //Up
    else if(volt > 0.2 && volt < 0.6   && position_y>1 && tab[position_y-2][position_x-1] != 2){
    	if(notzero==1){
    		positioning();
        	--position_y;
        	if(tab[position_y-2][position_x-1] == 2 && position_y==2){
        		lcd_char(line-1,position_x,0x04);
        		pud = 4;
        		HAL_Delay(500);
        	}else{
        		Print_in_lcd();
        	}
        	counter++;
    	}else{
        	--position_y;
        	Print_in_lcd();
    	}
    }
    //Right
    else if(volt < 0.2  && position_x<16 && tab[position_y-1][position_x] != 2)
    {
    	if(notzero==1){
    		positioning();
    		++position_x;
    		if(tab[position_y-2][position_x-1] == 2 && position_y%2==0){
    			lcd_char(line,position_x,0x04);
    			pud = 5;
    			HAL_Delay(500);
    		}else if(tab[position_y][position_x-1] == 2 && position_y%2==1){
    			lcd_char(line,position_x,0x05);
    			pud = 5;
    			HAL_Delay(500);
    		}else{
    			Print_in_lcd();
    		}
    		counter++;
    		}else{
    			++position_x;
    			Print_in_lcd();
    		}
    }
    else{ return 0;}
	return 1;
}

void Starting_position()
{
	if(!notzero){
		lcd_char(line,position_x,0x01);
	}
	int Selected = Keyboard();
	while(Selected != 3)
	{
		Selected = Keyboard();
	}
	notzero=1;
}

char buffor[32];

int System_Interface(const char *command)
{
	if(strstr(command, "shoot") == command)
	{
		uint32_t y, x;
	    if (sscanf(command, "shoot %d %d", &y, &x) == 2)
	    {
	    	shoot_counter++;
			printf("Fire! on position %d x %d \n", y, x);
			if(y>4 || x>16)
			{
				printf("Outside the area !");
				return 0;
			}
			else if(tab[y-1][x-1] == 1)
			{
				for(int i = 1; i <= 2 ; i++)
				{
					for(int j = 1; j <= 7 ; j++)
					{
						lcd_clear();
						lcd_print(i,j,"Loss!");
						printf("Win! \n");
						win=1;
						HAL_Delay(500);
					}
				}
				lcd_clear();
				lcd_print(1,3,"GAME OVER !");
				printf("GAME OVER !\n");
			}
			else
			{
				if(x>position_x)
				{
					printf("The ship is on the left \n");
				}
				else if(x<position_x)
				{
					printf("The ship is on the right \n");
				}
				else
				{
					printf("Is in the same column \n");
				}


				if(y>=3)
				{
					line=2;
				}
				else
				{
					line=1;
				}
				if(y%2==0)
				{
					if(tab[y-2][x-1] == 2){
						lcd_char(line,x,0x06);
					}
					else if(tab[y-2][x-1] == 1){
						lcd_char(line,x,0x05);
					}
					else{
					lcd_char(line,x,0x02);
					}
				}
				else
				{
					if(tab[y][x-1] == 2){
						lcd_char(line,x,0x06);
					}
					else if(tab[y][x-1] == 1){
						lcd_char(line,x,0x04);
					}
					else{
						lcd_char(line,x,0x03);
					}

				}
				tab[y-1][x-1]=2;
				printtab();
				return 1;
			}
		}
	    else
	    {
	        printf("Wrong arguments !\n");
		    return 0;
	    }
	}
	else
	{
		printf("Wrong command !\n");
	    return 0;
	}
    printf("\n");
    return 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	  init_tab();
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
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  lcd_init(_LCD_4BIT, _LCD_FONT_5x8, _LCD_2LINE);
  lcd_clear();
  //MENU
  menu();
  while (check()){
	  check();
  }
  HAL_Delay(500);
  lcd_clear();

  //Round 0
  printf("Round 0\r\n");
  printf("Player LCD place starting position of his ship\r\n");
  create_char(0,customChar,7);
  Starting_position();
  printf("Ship placed! Now you can use 'shoot x y' to destroy the ship.\r\n");
  tab[position_y-1][position_x-1]=1;
  printtab();
  HAL_Delay(500);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int attempt = 1;
  int round = 1;

  while (1)
  {
	  //Round X
	  if (shoot_counter == winning_condition_counter)
	  {
			for(int i = 1; i <= 2 ; i++)
					{
						for(int j = 1; j <= 7 ; j++)
						{
							lcd_clear();
							lcd_print(i,j,"Loss!");
							printf("Win! \n");
							win=1;
							HAL_Delay(500);
						}
					}
			break;
	  }
		  while(attempt<4)
		  {
			  if(win)
			  {
				  break;
			  }
			  printf("Round  %d\r\n",round);
		  	  printf("Player UART guess where the ship is (3 tries)\r\n");
		  	  printf("Round %d Attempt %d\r\n",round,attempt);
		  	  printf("> ");
		  	  fflush(stdin);
		  	  fflush(stdout);
		  	  if ( _read(buffor, 32) > 0)
		  	  {
		  		attempt+=System_Interface(buffor);
		  	  }
		  }
		  if (!win)
		  {
			  tab[position_y-1][position_x-1]=0;
			  printf("Player LCD place new position of his ship\r\n");
			  Starting_position();
			  tab[position_y-1][position_x-1]=1;
			  attempt = 1;
			  round++;
			  counter=0;
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_D6_Pin|LCD_D5_Pin|LCD_D4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_D7_Pin|LCD_RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_D6_Pin LCD_D5_Pin LCD_D4_Pin */
  GPIO_InitStruct.Pin = LCD_D6_Pin|LCD_D5_Pin|LCD_D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_EN_Pin */
  GPIO_InitStruct.Pin = LCD_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_D7_Pin LCD_RS_Pin */
  GPIO_InitStruct.Pin = LCD_D7_Pin|LCD_RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
