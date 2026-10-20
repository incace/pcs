#include "my_gpio_driver.h"

void MyGPIO_Init(GPIO_TypeDef* port, uint16_t pin, MyGPIO_Mode mode) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN;

    for(int i = 0; i < 16; i++) {
        if (pin & (1 << i)) {
            port->MODER &= ~(3U << (i * 2));
            if (mode == MY_GPIO_MODE_OUTPUT) {
                port->MODER |= (1U << (i * 2));
            }
        }
    }
}

void MyGPIO_Write(GPIO_TypeDef* port, uint16_t pin, uint8_t state) {
    if (state) port->BSRR = pin;
    else port->BSRR = (uint32_t)pin << 16;
}

uint8_t MyGPIO_Read(GPIO_TypeDef* port, uint16_t pin) {
    return (port->IDR & pin) ? 1 : 0;
}


volatile uint32_t ticks = 0;

void Timer_Init(void) {
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

uint32_t Get_Ticks(void) {
    return ticks;
}

void Delay_ms(uint32_t ms) {
    uint32_t start = Get_Ticks();
    while ((Get_Ticks() - start) < ms);
}
