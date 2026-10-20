#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

void Board_Init(void);
void Board_RedLed_Set(bool state);
bool Board_Button_IsPressed(void);

#endif /* BOARD_H */
