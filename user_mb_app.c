#include "mb.h"
#include "mbport.h"
#include "SEGGER_RTT.h"
#include <stdio.h>
#include <string.h>

// Определения регистров
#define REG_HOLDING_START     1
#define REG_HOLDING_NREGS    32

// Буфер регистров
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

// Отладочный UART
extern UART_HandleTypeDef huart3;

// Функция отладочной печати
void DebugPrint(const char* str)
{
	SEGGER_RTT_WriteString(0, str);
}

void DebugPrintBuffer(const char* prefix, UCHAR* buffer, int length)
{
    char debug_buf[128];
    snprintf(debug_buf, sizeof(debug_buf), "%s [%d]: ", prefix, length);
    SEGGER_RTT_WriteString(0, debug_buf);

    for(int i = 0; i < length; i++)
    {
        snprintf(debug_buf, sizeof(debug_buf), "%02X ", buffer[i]);
        SEGGER_RTT_WriteString(0, debug_buf);
    }
    SEGGER_RTT_WriteString(0, "\r\n");
}

// Расширенная функция обработки регистров
eMBErrorCode eMBRegHoldingCB(UCHAR* pucRegBuffer, USHORT usAddress,
                            USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;
    char debug_buf[64];

    // Отладка входных параметров
    SEGGER_RTT_WriteString(0, "\r\n=== MODBUS Register Access ===\r\n");
    snprintf(debug_buf, sizeof(debug_buf),
             "Mode: %s, Address: %d, Count: %d\r\n",
             (eMode == MB_REG_READ) ? "READ" : "WRITE",
             usAddress, usNRegs);
    SEGGER_RTT_WriteString(0, debug_buf);

    if((usAddress >= REG_HOLDING_START) &&
       (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
        iRegIndex = (int)(usAddress - REG_HOLDING_START);

        switch(eMode)
        {
            case MB_REG_READ:
                while(usNRegs > 0)
                {
                    *pucRegBuffer++ = (UCHAR)(usRegHoldingBuf[iRegIndex] >> 8);
                    *pucRegBuffer++ = (UCHAR)(usRegHoldingBuf[iRegIndex] & 0xFF);

                    snprintf(debug_buf, sizeof(debug_buf),
                           "Read Reg[%d] = 0x%04X\r\n",
                           iRegIndex, usRegHoldingBuf[iRegIndex]);
                    SEGGER_RTT_WriteString(0, debug_buf);

                    iRegIndex++;
                    usNRegs--;
                }
                break;

            case MB_REG_WRITE:
                while(usNRegs > 0)
                {
                    USHORT oldValue = usRegHoldingBuf[iRegIndex];
                    usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                    usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;

                    snprintf(debug_buf, sizeof(debug_buf),
                           "Write Reg[%d]: 0x%04X -> 0x%04X\r\n",
                           iRegIndex, oldValue, usRegHoldingBuf[iRegIndex]);
                    SEGGER_RTT_WriteString(0, debug_buf);

                    if(iRegIndex == 0)
                    {
                        HAL_GPIO_WritePin(LED_MODBUS_GPIO_Port, LED_MODBUS_Pin,
                            (usRegHoldingBuf[0] > 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
                        SEGGER_RTT_WriteString(0, "LED State Updated\r\n");
                    }

                    iRegIndex++;
                    usNRegs--;
                }
                break;
        }
    }
    else
    {
        snprintf(debug_buf, sizeof(debug_buf),
                "Error: Invalid address range [%d-%d]\r\n",
                usAddress, usAddress + usNRegs - 1);
        SEGGER_RTT_WriteString(0, debug_buf);
        eStatus = MB_ENOREG;
    }

    SEGGER_RTT_WriteString(0, "=== End of Register Access ===\r\n");
    return eStatus;
}


// Функция инициализации регистров
void vMBRegisterInit(void)
{
    // Очистка всех регистров
    for(int i = 0; i < REG_HOLDING_NREGS; i++)
    {
        usRegHoldingBuf[i] = 0;
    }

    DebugPrint("Registers initialized\r\n");
}

// Функция получения значения регистра (для отладки)
USHORT usGetHoldingRegister(USHORT usAddress)
{
    if(usAddress >= REG_HOLDING_START &&
       usAddress < REG_HOLDING_START + REG_HOLDING_NREGS)
    {
        return usRegHoldingBuf[usAddress - REG_HOLDING_START];
    }
    return 0;
}


// Для дискретных входов
eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress,
                             USHORT usNDiscrete)
{
    return MB_ENOREG;
}

// Для входных регистров
eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress,
                          USHORT usNRegs)
{
    return MB_ENOREG;
}

// Для катушек (coils)
eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress,
                          USHORT usNCoils, eMBRegisterMode eMode)
{
    return MB_ENOREG;
}
