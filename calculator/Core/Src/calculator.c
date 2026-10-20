#include "calculator.h"
#include "uart_driver.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_DIGITS 5
#define SHORT_MAX  32767
#define SHORT_MIN -32768

static char op1_buf[MAX_DIGITS + 1];
static char op2_buf[MAX_DIGITS + 1];
static uint8_t op1_idx = 0;
static uint8_t op2_idx = 0;
static char math_op = 0;
static bool entering_second_operand = false;

static void Report_Error(void) {
    Board_RedLed_Set(true);
    UartDriver_SendString("\r\nerror\r\n");
    Calculator_Reset();
}

static void Calculate_Result(void) {
    long val1 = atol(op1_buf);
    long val2 = atol(op2_buf);

    if (val1 > SHORT_MAX || val2 > SHORT_MAX) {
        Report_Error();
        return;
    }

    long result = 0;
    bool has_error = false;

    switch (math_op) {
        case '+': result = val1 + val2; break;
        case '-': result = val1 - val2; break;
        case '*': result = val1 * val2; break;
        case '/':
            if (val2 == 0) {
                has_error = true;
            } else {
                result = val1 / val2;
            }
            break;
        default:
            has_error = true;
            break;
    }

    if (has_error || result < SHORT_MIN || result > SHORT_MAX) {
        Report_Error();
    } else {
        Board_RedLed_Set(false);
        char out_str[16];
        sprintf(out_str, "\r\n%d\r\n", (int16_t)result);
        UartDriver_SendString(out_str);
        Calculator_Reset();
    }
}

void Calculator_Init(void) {
    Calculator_Reset();
}

void Calculator_Reset(void) {
    op1_idx = 0;
    op2_idx = 0;
    op1_buf[0] = '\0';
    op2_buf[0] = '\0';
    math_op = 0;
    entering_second_operand = false;
}

void Calculator_ProcessChar(uint8_t ch) {
    UartDriver_SendByte(ch);

    if (!entering_second_operand) {
        if (ch >= '0' && ch <= '9') {
            if (op1_idx < MAX_DIGITS) {
                op1_buf[op1_idx++] = (char)ch;
                op1_buf[op1_idx] = '\0';
            } else {
                Report_Error();
            }
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            if (op1_idx == 0) {
                Report_Error();
            } else {
                math_op = (char)ch;
                entering_second_operand = true;
            }
        } else {
            Report_Error();
        }
    }
    else {
        if (ch >= '0' && ch <= '9') {
            if (op2_idx < MAX_DIGITS) {
                op2_buf[op2_idx++] = (char)ch;
                op2_buf[op2_idx] = '\0';
            } else {
                Report_Error();
            }
        } else if (ch == '=') {
            if (op2_idx == 0) {
                Report_Error();
            } else {
                Calculate_Result();
            }
        } else {
            Report_Error();
        }
    }
}
