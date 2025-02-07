#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "main.h"
#include "mb.h"
#include "mbconfig.h"
#include "mbport.h"
#include "user_mb_app.h"
#include "mbproto.h"
#include "mbrtu.h"
#include "port.h"
#include "SEGGER_RTT.h"
#include "debug.h"

/* Private variables */
UART_HandleTypeDef huart3;
TIM_HandleTypeDef htim2;
char string[255];

/* Private function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);

/* Private user code */
void systemPrint(const char *str) {
	SEGGER_RTT_WriteString(0, str);
}

int main(void) {
	eMBErrorCode eStatus;

	/* MCU Configuration */
	HAL_Init();
	SystemClock_Config();

	/* Initialize peripherals */
	MX_GPIO_Init();
	MX_USART3_UART_Init();
	MX_TIM2_Init();

	/* Enable UART interrupt */
	NVIC_EnableIRQ(USART3_IRQn);

	/* Initialize SEGGER RTT */
	SEGGER_RTT_Init();
	snprintf(string, sizeof(string), "\r\n--------Initialisation--------\r\n");
	systemPrint(string);

	/* Initialize Modbus */
	eStatus = eMBInit(MB_RTU, 1, 0, 38400UL, MB_PAR_NONE);
	if (eStatus != MB_ENOERR) {
		SEGGER_RTT_WriteString(0, "Modbus Init Failed\r\n");
		Error_Handler();
	}

	/* Enable protocol stack */
	eStatus = eMBEnable();
	if (eStatus != MB_ENOERR) {
		SEGGER_RTT_WriteString(0, "Modbus Enable Failed\r\n");
		Error_Handler();
	}

	/* Start timer for Modbus */
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET);
	HAL_Delay(1); // Задержка для стабилизации линии

	while (1) {
		eMBPoll();
	}
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

static void MX_TIM2_Init(void) {
	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 71;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 50;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
}

static void MX_USART3_UART_Init(void) {
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 38400;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;

	if (HAL_UART_Init(&huart3) != HAL_OK) {
		Error_Handler();
	}
}

static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOA, RS485_DE_Pin | RS485_RE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_MODBUS_GPIO_Port, LED_MODBUS_Pin, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = RS485_DE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(RS485_DE_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = RS485_RE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RS485_RE_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LED_MODBUS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LED_MODBUS_GPIO_Port, &GPIO_InitStruct);
}

void Error_Handler(void) {
	__disable_irq();
	snprintf(string, sizeof(string),
			"\r\n--------Initialisation FAILED--------\r\n");
	systemPrint(string);
	while (1) {
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
