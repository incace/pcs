#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    UART_MODE_INTERRUPT = 0,
    UART_MODE_POLLING
} UartMode_t;

void UartDriver_Init(void);
void UartDriver_SetMode(UartMode_t mode);
UartMode_t UartDriver_GetMode(void);

bool UartDriver_ReceiveByte(uint8_t *byte);
void UartDriver_SendByte(uint8_t byte);
void UartDriver_SendString(const char *str);

#endif
