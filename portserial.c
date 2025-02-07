#include "mb.h"
#include "mbport.h"
#include "SEGGER_RTT.h"

static uint8_t ucBuffer;
extern UART_HandleTypeDef huart3;

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    if(xRxEnable) {
        // Включаем режим приема
        HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET);
        HAL_Delay(1); // Задержка для стабилизации линии

        // Включаем прерывание по приему
        __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
        HAL_UART_Receive_IT(&huart3, &ucBuffer, 1);

        SEGGER_RTT_WriteString(0, "RS485: RX Mode\r\n");
    }
    else {
        // Выключаем прерывание по приему
        __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
    }

    if(xTxEnable) {
        // Включаем режим передачи
        HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET);
        HAL_Delay(1); // Задержка для стабилизации линии

        // Включаем прерывание по окончанию передачи
        __HAL_UART_ENABLE_IT(&huart3, UART_IT_TC);

        SEGGER_RTT_WriteString(0, "RS485: TX Mode\r\n");
    }
    else {
        // Выключаем прерывание по окончанию передачи
        __HAL_UART_DISABLE_IT(&huart3, UART_IT_TC);
    }
}

BOOL xMBPortSerialInit(UCHAR ucPort, ULONG ulBaudRate,
                      UCHAR ucDataBits, eMBParity eParity)
{
    // UART уже инициализирован в CubeMX

    // Устанавливаем режим приема по умолчанию
    HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET);

    SEGGER_RTT_WriteString(0, "RS485: Initialized\r\n");
    return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
    HAL_UART_Transmit(&huart3, (uint8_t*)&ucByte, 1, 10);
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR* pucByte)
{
    *pucByte = ucBuffer;
    return TRUE;
}
