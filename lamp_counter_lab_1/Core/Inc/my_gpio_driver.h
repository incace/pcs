#ifndef MY_GPIO_DRIVER_H
#define MY_GPIO_DRIVER_H

#include "stm32f4xx.h"

typedef enum {
    MY_GPIO_MODE_INPUT = 0x00,
    MY_GPIO_MODE_OUTPUT = 0x01
} MyGPIO_Mode;

void MyGPIO_Init(GPIO_TypeDef* port, uint16_t pin, MyGPIO_Mode mode);
void MyGPIO_Write(GPIO_TypeDef* port, uint16_t pin, uint8_t state);
uint8_t MyGPIO_Read(GPIO_TypeDef* port, uint16_t pin);
void Timer_Init(void);
uint32_t Get_Ticks(void);
extern volatile uint32_t ticks;

#endif
