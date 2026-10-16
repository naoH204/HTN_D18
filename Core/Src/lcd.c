#include "lcd.h"

extern I2C_HandleTypeDef hi2c1;
#define LCD_ADDR (0x27 << 1) // Đổi thành 0x3F<<1 nếu không lên chữ

void LCD_Send_Cmd(uint8_t cmd) {
    uint8_t data_u = (cmd & 0xf0), data_l = ((cmd << 4) & 0xf0);
    uint8_t data_t[4] = {data_u|0x0C, data_u|0x08, data_l|0x0C, data_l|0x08};
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, (uint8_t *)data_t, 4, 100);
}

void LCD_Send_Data(uint8_t data) {
    uint8_t data_u = (data & 0xf0), data_l = ((data << 4) & 0xf0);
    uint8_t data_t[4] = {data_u|0x0D, data_u|0x09, data_l|0x0D, data_l|0x09};
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, (uint8_t *)data_t, 4, 100);
}

void LCD_Init(void) {
    HAL_Delay(50);
    LCD_Send_Cmd(0x30); HAL_Delay(5);
    LCD_Send_Cmd(0x30); HAL_Delay(1);
    LCD_Send_Cmd(0x30); HAL_Delay(10);
    LCD_Send_Cmd(0x20); HAL_Delay(10);
    LCD_Send_Cmd(0x28); HAL_Delay(1);
    LCD_Send_Cmd(0x08); HAL_Delay(1);
    LCD_Send_Cmd(0x01); HAL_Delay(2); // Clear screen
    LCD_Send_Cmd(0x06); HAL_Delay(1);
    LCD_Send_Cmd(0x0C); HAL_Delay(1); // Bật màn hình
}

void LCD_Send_String(char *str) {
    while (*str) LCD_Send_Data(*str++);
}

void LCD_Set_Cursor(uint8_t row, uint8_t col) {
    LCD_Send_Cmd((row == 0 ? 0x80 : 0xC0) | col);
}
