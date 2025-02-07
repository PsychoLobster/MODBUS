#ifndef USER_MB_APP_H
#define USER_MB_APP_H

#include "mb.h"
#include "mbport.h"

/* Modbus Protocol Definitions */
#define MB_ADDRESS_BROADCAST    0   // Broadcast address
#define MB_ADDRESS_MIN         1    // Minimum node address
#define MB_ADDRESS_MAX         48   // Maximum node address

/* Register Definitions */
// Address 0 registers (Display Control)
#define REG_DIRECTION_ARROWS       0x0000  // Bit0-Bit1
#define REG_DISPLAY_CONTROL        0x0002  // Bit2
#define REG_CABINET_OVERLOAD      0x0003  // Bit3
#define REG_DISPLAY_CODE_MID      0x0004  // Bit4-Bit9
#define REG_DISPLAY_CODE_LOW      0x000A  // Bit10-Bit15

// Address 100 registers
#define REG_FRAME_HEADER          0x0064  // 0x64 (100 decimal)
#define REG_EXTERNAL_CALL         0x0014  // Bit20
#define REG_CALL_LIGHT_DELAY      0x0015  // Bit21
#define REG_BUTTON_DIMMING        0x0017  // Bit23

/* Bit Definitions */
// Direction Arrows
#define DIRECTION_UP              0x00
#define DIRECTION_DOWN            0x01
#define DIRECTION_STEADY          0x00
#define DIRECTION_FLASHING        0x01

// Display Control
#define DISPLAY_NORMAL            0x00
#define DISPLAY_STATIONARY        0x01

// Cabinet Status
#define CABINET_NORMAL            0x00
#define CABINET_OVERLOAD          0x01

/* Function Prototypes */
eMBErrorCode eMBRegInputCB(UCHAR* pucRegBuffer, USHORT usAddress,
                          USHORT usNRegs);
eMBErrorCode eMBRegHoldingCB(UCHAR* pucRegBuffer, USHORT usAddress,
                            USHORT usNRegs, eMBRegisterMode eMode);
eMBErrorCode eMBRegCoilsCB(UCHAR* pucRegBuffer, USHORT usAddress,
                          USHORT usNCoils, eMBRegisterMode eMode);
eMBErrorCode eMBRegDiscreteCB(UCHAR* pucRegBuffer, USHORT usAddress,
                             USHORT usNDiscrete);

/* Display Code Definitions */
typedef enum {
    DISP_0 = 0x00,    // "0"
    DISP_1 = 0x01,    // "1"
    DISP_2 = 0x02,    // "2"
    DISP_3 = 0x03,    // "3"
    DISP_4 = 0x04,    // "4"
    DISP_5 = 0x05,    // "5"
    DISP_6 = 0x06,    // "6"
    DISP_7 = 0x07,    // "7"
    DISP_8 = 0x08,    // "8"
    DISP_9 = 0x09,    // "9"
    DISP_A = 0x0A,    // "A"
    DISP_B = 0x0B,    // "B"
    DISP_C = 0x0C,    // "C"
    DISP_D = 0x0D,    // "D"
    DISP_E = 0x0E,    // "E"
    DISP_F = 0x0F,    // "F"
    DISP_NONE = 0x13  // No display
} DisplayCode_t;

/* Elevator Status Definitions */
typedef enum {
    ELEVATOR_INSPECTION = 0,
    ELEVATOR_FIRE_RETURN = 3,
    ELEVATOR_FAULT = 5,
    ELEVATOR_LOCKED = 8,
    ELEVATOR_EMERGENCY = 11,
    ELEVATOR_VIP = 15
} ElevatorStatus_t;

#endif /* USER_MB_APP_H */
