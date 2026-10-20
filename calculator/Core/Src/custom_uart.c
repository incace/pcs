#include "custom_uart.h"
#include "ring_buffer.h"

static RingBuffer rx_ring_buf;
static RingBuffer tx_ring_buf;

static CustomUartMode_t current_driver_mode = CUSTOM_UART_MODE_POLLING;

static void CustomUart_GPIO_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    GPIOC->MODER &= ~((3U << (6 * 2)) | (3U << (7 * 2)));
    GPIOC->MODER |=  ((2U << (6 * 2)) | (2U << (7 * 2)));

    GPIOC->OTYPER &= ~((1U << 6) | (1U << 7));

    GPIOC->OSPEEDR |= ((3U << (6 * 2)) | (3U << (7 * 2)));

    GPIOC->PUPDR &= ~((3U << (6 * 2)) | (3U << (7 * 2)));
    GPIOC->PUPDR |=  ((1U << (6 * 2)) | (1U << (7 * 2)));

    GPIOC->AFR[0] &= ~((0xFU << (6 * 4)) | (0xFU << (7 * 4)));
    GPIOC->AFR[0] |=  ((0x8U << (6 * 4)) | (0x8U << (7 * 4)));
}

void CustomUart_Init(uint32_t baudrate, CustomUartMode_t mode) {
    RingBuffer_Init(&rx_ring_buf);
    RingBuffer_Init(&tx_ring_buf);

    CustomUart_GPIO_Init();

    RCC->APB2ENR |= RCC_APB2ENR_USART6EN;

    USART6->CR1 = 0;
    USART6->CR2 = 0;
    USART6->CR3 = 0;


    uint32_t pclk2_freq = 16000000U;

    USART6->BRR = (pclk2_freq + (baudrate / 2U)) / baudrate;

    USART6->CR1 |= (USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);

    CustomUart_SetMode(mode);
}

void CustomUart_SetMode(CustomUartMode_t mode) {
    current_driver_mode = mode;

    if (mode == CUSTOM_UART_MODE_INTERRUPT) {
        RingBuffer_Clear(&rx_ring_buf);
        RingBuffer_Clear(&tx_ring_buf);

        USART6->CR1 |= USART_CR1_RXNEIE;

        NVIC_SetPriority(USART6_IRQn, 5);
        NVIC_EnableIRQ(USART6_IRQn);
    } else {
        NVIC_DisableIRQ(USART6_IRQn);
        USART6->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_TXEIE);
    }
}

CustomUartMode_t CustomUart_GetMode(void) {
    return current_driver_mode;
}

bool CustomUart_IsRxNotEmpty(void) {
    return (USART6->SR & USART_SR_RXNE) != 0;
}

bool CustomUart_IsTxEmpty(void) {
    return (USART6->SR & USART_SR_TXE) != 0;
}

bool CustomUart_ReceiveByte(uint8_t *byte) {
    if (current_driver_mode == CUSTOM_UART_MODE_INTERRUPT) {
        return RingBuffer_Pop(&rx_ring_buf, byte);
    } else {
        if (CustomUart_IsRxNotEmpty()) {
            *byte = (uint8_t)(USART6->DR & 0xFF);
            return true;
        }
        return false;
    }
}

void CustomUart_SendByte(uint8_t byte) {
    if (current_driver_mode == CUSTOM_UART_MODE_INTERRUPT) {
        RingBuffer_Push(&tx_ring_buf, byte);

        uint32_t primask = __get_PRIMASK();
        __disable_irq();
        USART6->CR1 |= USART_CR1_TXEIE;
        __set_PRIMASK(primask);
    } else {
        while (!CustomUart_IsTxEmpty());
        USART6->DR = byte;
    }
}

void CustomUart_SendString(const char *str) {
    while (*str) {
        CustomUart_SendByte((uint8_t)*str++);
    }
}

void USART6_IRQHandler(void) {
    if ((USART6->SR & USART_SR_RXNE) && (USART6->CR1 & USART_CR1_RXNEIE)) {
        uint8_t rx_data = (uint8_t)(USART6->DR & 0xFF);
        RingBuffer_Push(&rx_ring_buf, rx_data);
    }

    if ((USART6->SR & USART_SR_TXE) && (USART6->CR1 & USART_CR1_TXEIE)) {
        uint8_t tx_data;
        if (RingBuffer_Pop(&tx_ring_buf, &tx_data)) {
            USART6->DR = tx_data;
        } else {
            USART6->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}
