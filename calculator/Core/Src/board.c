#include "board.h"
#include "main.h"

#define RED_LED_PIN_EN    GPIO_PIN_15
#define RED_LED_PIN_CAT   GPIO_PIN_14
#define RED_LED_PORT      GPIOD

#define USER_BTN_PORT     GPIOC
#define USER_BTN_PIN      GPIO_PIN_15
#define DEBOUNCE_DELAY_MS 250

void Board_Init(void) {
    Board_RedLed_Set(false);
}

void Board_RedLed_Set(bool state) {
    if (state) {
        HAL_GPIO_WritePin(RED_LED_PORT, RED_LED_PIN_CAT, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RED_LED_PORT, RED_LED_PIN_EN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(RED_LED_PORT, RED_LED_PIN_CAT, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RED_LED_PORT, RED_LED_PIN_EN, GPIO_PIN_RESET);
    }
}

bool Board_Button_IsPressed(void) {
    static uint32_t last_press_time = 0;
    if (HAL_GPIO_ReadPin(USER_BTN_PORT, USER_BTN_PIN) == GPIO_PIN_RESET) {
        uint32_t current_time = HAL_GetTick();
        if (current_time - last_press_time > DEBOUNCE_DELAY_MS) {
            last_press_time = current_time;
            return true;
        }
    }
    return false;
}
