/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "main.h"
#include "stm32f1xx_it.h"
#include "SEGGER_RTT.h"
#include "mb.h"
#include "mbport.h"
#include "debug.h"

/* External variables */
extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim2;
extern USHORT usTimerOCRADelta;

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
void NMI_Handler(void)
{
    while (1)
    {
    }
}

void HardFault_Handler(void)
{
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    while (1)
    {
    }
}

void BusFault_Handler(void)
{
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                     */
/******************************************************************************/

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
    static uint32_t timer_cnt = 0;
    char debug_buf[64];

    if(TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        timer_cnt++;

        // Отладочное сообщение каждые 1000 тиков (50мс)
        if((timer_cnt % 1000) == 0) {
            snprintf(debug_buf, sizeof(debug_buf),
                    "Timer ticks: %lu, Delta: %u\r\n",
                    timer_cnt, usTimerOCRADelta);
            SEGGER_RTT_WriteString(0, debug_buf);
        }

        // Проверка истечения таймера Modbus
        if(timer_cnt >= usTimerOCRADelta) {
            SEGGER_RTT_WriteString(0, "Modbus T35 timeout\r\n");
            timer_cnt = 0;
            pxMBPortCBTimerExpired();
        }
    }

    HAL_TIM_IRQHandler(&htim2);
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
    uint32_t tmp_flag = 0;
    uint32_t tmp_it_source = 0;
    static uint8_t rx_buffer[256];
    static uint8_t rx_index = 0;
    char debug_buf[64];

    // Проверка прерывания по приему
    tmp_flag = __HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE);
    tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE);

    if((tmp_flag != RESET) && (tmp_it_source != RESET)) {
        rx_buffer[rx_index] = (uint8_t)(huart3.Instance->DR & 0xFF);

        snprintf(debug_buf, sizeof(debug_buf),
                "RX[%d]: 0x%02X\r\n",
                rx_index, rx_buffer[rx_index]);
        SEGGER_RTT_WriteString(0, debug_buf);

        rx_index++;

        // Проверка на полный пакет Modbus
        if(rx_index >= 3) {
            if((rx_buffer[1] & 0x80) ||
               (rx_index >= rx_buffer[2] + 5) ||
               (rx_index >= 8))
            {
                DebugModbusFrame("RX Frame", rx_buffer, rx_index);
                pxMBFrameCBByteReceived();
                rx_index = 0;
            }
        }
    }

    // Проверка прерывания по окончанию передачи
    tmp_flag = __HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC);
    tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_TC);

    if((tmp_flag != RESET) && (tmp_it_source != RESET)) {
        SEGGER_RTT_WriteString(0, "TX Complete\r\n");

        // Переключаемся на прием после окончания передачи
        HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET);

        pxMBFrameCBTransmitterEmpty();
    }

    HAL_UART_IRQHandler(&huart3);
}
