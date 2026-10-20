/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Lab 2, Part 2 (Custom Register-level UART Driver)
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "board.h"
#include "custom_uart.h"
#include "calculator.h"
/* USER CODE END Includes */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void Handle_ModeSwitch(void) {
    if (CustomUart_GetMode() == CUSTOM_UART_MODE_INTERRUPT) {
        CustomUart_SetMode(CUSTOM_UART_MODE_POLLING);
        CustomUart_SendString("\r\n[Custom Driver: POLLING (No Interrupts)]\r\n");
    } else {
        CustomUart_SetMode(CUSTOM_UART_MODE_INTERRUPT);
        CustomUart_SendString("\r\n[Custom Driver: INTERRUPTS]\r\n");
    }

    Calculator_Reset();
    CustomUart_SendString("Enter expression (e.g. 12+34=):\r\n");
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  Board_Init();

  CustomUart_Init(19200, CUSTOM_UART_MODE_INTERRUPT);
  Calculator_Init();

  CustomUart_SendString("\r\n--- Stand SDK-1.1M Ready (Custom Register UART Driver) ---\r\n");
  CustomUart_SendString("[Custom Driver: INTERRUPTS]\r\n");
  CustomUart_SendString("Enter expression (e.g. 12+34=):\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      if (Board_Button_IsPressed()) {
          Handle_ModeSwitch();
      }

      uint8_t received_char;
      if (CustomUart_ReceiveByte(&received_char)) {
          if (received_char != '\r' && received_char != '\n') {
              Calculator_ProcessChar(received_char);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
