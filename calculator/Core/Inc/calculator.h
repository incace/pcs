#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <stdint.h>

void Calculator_Init(void);
void Calculator_Reset(void);
void Calculator_ProcessChar(uint8_t ch);

#endif /* CALCULATOR_H */
