// debug.h
#ifndef DEBUG_H
#define DEBUG_H

#include "mb.h"
#include "mbport.h"

void DebugModbusFrame(const char* prefix, UCHAR* buffer, USHORT length);
const char* GetModbusCommandName(uint8_t functionCode);

#endif // DEBUG_H
