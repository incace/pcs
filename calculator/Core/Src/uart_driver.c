#include "uart_driver.h"
#include "ring_buffer.h"
#include "main.h"

extern UART_HandleTypeDef huart6;

static RingBuffer rx_ring_buffer;
static RingBuffer tx_ring_buffer;

static uint8_t it_rx_char;
static volatile bool is_tx_busy = false;
static UartMode_t current_mode = UART_MODE_INTERRUPT;

void UartDriver_Init(void) {
    RingBuffer_Init(&rx_ring_buffer);
    RingBuffer_Init(&tx_ring_buffer);
    is_tx_busy = false;
    UartDriver_SetMode(UART_MODE_INTERRUPT);
}

void UartDriver_SetMode(UartMode_t mode) {
    current_mode = mode;

    if (current_mode == UART_MODE_INTERRUPT) {
        RingBuffer_Clear(&rx_ring_buffer);
        RingBuffer_Clear(&tx_ring_buffer);
        is_tx_busy = false;

        HAL_NVIC_EnableIRQ(USART6_IRQn);
        HAL_UART_Receive_IT(&huart6, &it_rx_char, 1);
    } else {
        HAL_UART_Abort(&huart6);
        HAL_NVIC_DisableIRQ(USART6_IRQn);
    }
}

UartMode_t UartDriver_GetMode(void) {
    return current_mode;
}

bool UartDriver_ReceiveByte(uint8_t *byte) {
    if (current_mode == UART_MODE_INTERRUPT) {
        return RingBuffer_Pop(&rx_ring_buffer, byte);
    } else {
        if (__HAL_UART_GET_FLAG(&huart6, UART_FLAG_RXNE)) {
            if (HAL_UART_Receive(&huart6, byte, 1, 0) == HAL_OK) {
                return true;
            }
        }
        return false;
    }
}

void UartDriver_SendByte(uint8_t byte) {
    if (current_mode == UART_MODE_INTERRUPT) {
        RingBuffer_Push(&tx_ring_buffer, byte);

        uint32_t primask = __get_PRIMASK();
        __disable_irq();

        if (!is_tx_busy) {
            uint8_t tx_char;
            if (RingBuffer_Pop(&tx_ring_buffer, &tx_char)) {
                is_tx_busy = true;
                HAL_UART_Transmit_IT(&huart6, &tx_char, 1);
            }
        }
        __set_PRIMASK(primask);
    } else {
        HAL_UART_Transmit(&huart6, &byte, 1, 100);
    }
}

void UartDriver_SendString(const char *str) {
    while (*str) {
        UartDriver_SendByte((uint8_t)*str++);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART6) {
        RingBuffer_Push(&rx_ring_buffer, it_rx_char);
        HAL_UART_Receive_IT(&huart6, &it_rx_char, 1);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART6) {
        uint8_t next_byte;
        if (RingBuffer_Pop(&tx_ring_buffer, &next_byte)) {
            HAL_UART_Transmit_IT(&huart6, &next_byte, 1);
        } else {
            is_tx_busy = false;
        }
    }
}
