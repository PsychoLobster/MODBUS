#include "mb.h"
#include "mbport.h"
#include "port.h"

extern TIM_HandleTypeDef htim2;

// Define the timer delta variable
USHORT usTimerOCRADelta;

BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)
{
    usTimerOCRADelta = usTim1Timerout50us;

    // Timer already initialized in MX_TIM2_Init()
    HAL_TIM_Base_Start_IT(&htim2);

    return TRUE;
}

void vMBPortTimersEnable()
{
    TIM2->CNT = 0;
    HAL_TIM_Base_Start_IT(&htim2);
}

void vMBPortTimersDisable()
{
    HAL_TIM_Base_Stop_IT(&htim2);
}
