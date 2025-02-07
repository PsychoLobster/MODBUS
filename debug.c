#include "SEGGER_RTT.h"
#include "mb.h"
#include "mbport.h"

extern UART_HandleTypeDef huart3;

// Определения типов пакетов Modbus
typedef enum {
    MODBUS_READ_COILS = 1,
    MODBUS_READ_DISCRETE_INPUTS = 2,
    MODBUS_READ_HOLDING_REGISTERS = 3,
    MODBUS_READ_INPUT_REGISTERS = 4,
    MODBUS_WRITE_SINGLE_COIL = 5,
    MODBUS_WRITE_SINGLE_REGISTER = 6,
    MODBUS_WRITE_MULTIPLE_COILS = 15,
    MODBUS_WRITE_MULTIPLE_REGISTERS = 16
} ModbusFunctionCode;

// Функция получения названия команды Modbus
const char* GetModbusCommandName(uint8_t functionCode)
{
    switch(functionCode) {
        case MODBUS_READ_COILS: return "Read Coils";
        case MODBUS_READ_DISCRETE_INPUTS: return "Read Discrete Inputs";
        case MODBUS_READ_HOLDING_REGISTERS: return "Read Holding Registers";
        case MODBUS_READ_INPUT_REGISTERS: return "Read Input Registers";
        case MODBUS_WRITE_SINGLE_COIL: return "Write Single Coil";
        case MODBUS_WRITE_SINGLE_REGISTER: return "Write Single Register";
        case MODBUS_WRITE_MULTIPLE_COILS: return "Write Multiple Coils";
        case MODBUS_WRITE_MULTIPLE_REGISTERS: return "Write Multiple Registers";
        default: return "Unknown Command";
    }
}

// Основная функция отладки пакетов
void DebugModbusFrame(const char* prefix, UCHAR* buffer, USHORT length)
{
    char debug_buf[512];
    int offset = 0;
    uint16_t crc;

    // Заголовок отладочного сообщения
    SEGGER_RTT_WriteString(0, "\r\n=== MODBUS FRAME DEBUG ===\r\n");
    SEGGER_RTT_WriteString(0, prefix);
    SEGGER_RTT_WriteString(0, "\r\n");

    // Базовая информация о пакете
    snprintf(debug_buf, sizeof(debug_buf),
             "Length: %d bytes\r\n"
             "Slave Address: %d (0x%02X)\r\n"
             "Function: %s (0x%02X)\r\n",
             length,
             buffer[0], buffer[0],
             GetModbusCommandName(buffer[1]), buffer[1]);
    SEGGER_RTT_WriteString(0, debug_buf);

    // Детальный разбор данных в зависимости от функции
    switch(buffer[1]) {
        case MODBUS_READ_HOLDING_REGISTERS:
        case MODBUS_READ_INPUT_REGISTERS:
            snprintf(debug_buf, sizeof(debug_buf),
                     "Start Address: %d (0x%02X%02X)\r\n"
                     "Quantity: %d (0x%02X%02X)\r\n",
                     (buffer[2] << 8) | buffer[3],
                     buffer[2], buffer[3],
                     (buffer[4] << 8) | buffer[5],
                     buffer[4], buffer[5]);
            SEGGER_RTT_WriteString(0, debug_buf);
            break;

        case MODBUS_WRITE_SINGLE_REGISTER:
            snprintf(debug_buf, sizeof(debug_buf),
                     "Register Address: %d (0x%02X%02X)\r\n"
                     "Value: %d (0x%02X%02X)\r\n",
                     (buffer[2] << 8) | buffer[3],
                     buffer[2], buffer[3],
                     (buffer[4] << 8) | buffer[5],
                     buffer[4], buffer[5]);
            SEGGER_RTT_WriteString(0, debug_buf);
            break;
    }

    // Вывод всех байтов пакета
    offset = 0;
    offset += snprintf(debug_buf + offset, sizeof(debug_buf) - offset,
                      "Raw Data: ");

    for(int i = 0; i < length && offset < sizeof(debug_buf) - 5; i++) {
        offset += snprintf(debug_buf + offset, sizeof(debug_buf) - offset,
                          "%02X ", buffer[i]);
        if((i + 1) % 8 == 0 && i != length - 1) {
            offset += snprintf(debug_buf + offset, sizeof(debug_buf) - offset,
                             "\r\n         ");
        }
    }

    snprintf(debug_buf + offset, sizeof(debug_buf) - offset, "\r\n");
    SEGGER_RTT_WriteString(0, debug_buf);

    // Проверка CRC
    if(length >= 4) {
        uint16_t received_crc = (buffer[length-1] << 8) | buffer[length-2];
        uint16_t calculated_crc = usMBCRC16(buffer, length-2);

        snprintf(debug_buf, sizeof(debug_buf),
                 "CRC Check:\r\n"
                 "Received: 0x%04X\r\n"
                 "Calculated: 0x%04X\r\n"
                 "Status: %s\r\n",
                 received_crc,
                 calculated_crc,
                 (received_crc == calculated_crc) ? "OK" : "ERROR");
        SEGGER_RTT_WriteString(0, debug_buf);
    }

    SEGGER_RTT_WriteString(0, "=== END OF FRAME ===\r\n");
}

