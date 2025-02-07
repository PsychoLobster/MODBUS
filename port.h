#ifndef _PORT_H
#define _PORT_H

#include "main.h"
#include "stm32f1xx_hal.h"

#define INLINE                      inline
#define PR_BEGIN_EXTERN_C          extern "C" {
#define PR_END_EXTERN_C            }

#define ENTER_CRITICAL_SECTION()    __disable_irq()
#define EXIT_CRITICAL_SECTION()     __enable_irq()

typedef uint8_t BOOL;
typedef unsigned char UCHAR;
typedef char CHAR;
typedef uint16_t USHORT;
typedef int16_t SHORT;
typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE                        1
#endif

#ifndef FALSE
#define FALSE                       0
#endif

// Timer ticks per second
#define MB_TIMER_TICKS             20000

// Maximum size of Modbus frame
#define MB_SER_PDU_SIZE_MAX        256
#define MB_SER_PDU_SIZE_MIN        4
#define MB_SER_PDU_SIZE_CRC        2
#define MB_SER_PDU_ADDR_OFF        0
#define MB_SER_PDU_PDU_OFF         1

// Timer settings for protocol
#define MB_TIMER_PRESCALER         (72 - 1)  // For 72MHz clock
#define MB_TIMER_PERIOD            (100 - 1) // 50us @ 72MHz

// UART settings
#define MB_UART_BAUD               38400
#define MB_UART_DATA_BITS          8
#define MB_UART_STOP_BITS          1
#define MB_UART_PARITY             0

#endif
