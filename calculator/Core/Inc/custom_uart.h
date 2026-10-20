#ifndef CUSTOM_UART_H
#define CUSTOM_UART_H

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    CUSTOM_UART_MODE_POLLING = 0,
    CUSTOM_UART_MODE_INTERRUPT
} CustomUartMode_t;

void CustomUart_Init(uint32_t baudrate, CustomUartMode_t mode);

void CustomUart_SetMode(CustomUartMode_t mode);
CustomUartMode_t CustomUart_GetMode(void);

bool CustomUart_ReceiveByte(uint8_t *byte);
void CustomUart_SendByte(uint8_t byte);
void CustomUart_SendString(const char *str);

bool CustomUart_IsRxNotEmpty(void);
bool CustomUart_IsTxEmpty(void);

#endif /* CUSTOM_UART_H */
