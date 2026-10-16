#include "keypad.h"

// Các biến nội bộ phục vụ chống dội
uint8_t current_key = 0;
uint8_t confirmed_key = 0;
uint16_t debounce_tick = 0;

void Keypad_ScanCallback(void)
{
    /* Đọc trạng thái Port B (PB8 - PB15) */
    uint8_t read_port = ~((GPIOB->IDR >> 8) & 0xFF); 
    
    // Kiểm tra chống dội
    if (read_port != current_key) 
    {
        current_key = read_port;
        debounce_tick = 0; 
    }
    else 
    {
        debounce_tick++;
        if (debounce_tick == 20) 
        {
            confirmed_key = current_key; 
        }
    }
}

// Hàm API lấy giá trị phím
uint8_t Keypad_GetConfirmedKey(void) 
{
    return confirmed_key;
}