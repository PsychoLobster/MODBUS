#include "mb.h"
#include "mbport.h"
#include "SEGGER_RTT.h"
#include <stdio.h>
#include <string.h>

static eMBEventType eQueuedEvent;
static BOOL xEventInQueue;

BOOL xMBPortEventInit(void)
{
    xEventInQueue = FALSE;
    return TRUE;
}

BOOL xMBPortEventGet(eMBEventType * eEvent)
{
    char debug_buf[64];
    BOOL xEventHappened = FALSE;

    if(xEventInQueue) {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;

        switch(*eEvent) {
            case EV_READY:
                SEGGER_RTT_WriteString(0, "MB Event: READY\r\n");
                break;
            case EV_FRAME_RECEIVED:
                SEGGER_RTT_WriteString(0, "MB Event: FRAME_RECEIVED\r\n");
                break;
            case EV_EXECUTE:
                SEGGER_RTT_WriteString(0, "MB Event: EXECUTE\r\n");
                break;
            case EV_FRAME_SENT:
                SEGGER_RTT_WriteString(0, "MB Event: FRAME_SENT\r\n");
                break;
            default:
                snprintf(debug_buf, sizeof(debug_buf),
                        "MB Event: Unknown (%d)\r\n", *eEvent);
                SEGGER_RTT_WriteString(0, debug_buf);
        }
    }

    return xEventHappened;
}

BOOL xMBPortEventPost(eMBEventType eEvent)
{
    xEventInQueue = TRUE;
    eQueuedEvent = eEvent;
    SEGGER_RTT_WriteString(0, "MB Event Posted\r\n");
    return TRUE;
}

