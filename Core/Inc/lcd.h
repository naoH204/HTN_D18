#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32f1xx_hal.h"

void LCD_Init(void);
void LCD_Set_Cursor(uint8_t row, uint8_t col);
void LCD_Send_String(char *str);

#endif /* INC_LCD_H_ */
