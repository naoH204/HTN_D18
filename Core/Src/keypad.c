#include "keypad.h"
// bien noi bo
uint8_t current_key = 0;
uint8_t confirmed_key = 0;
uint16_t debounce_tick = 0;

// Dung cho ngat TIM3
void Keypad_ScanCallback(void)
{
    uint8_t read_port = ~((GPIOB->IDR >> 8) & 0xFF); 
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

// Hàm API để trả giá trị phím ra cho hệ thống dùng
uint8_t Keypad_GetConfirmedKey(void)
{
    return confirmed_key;
}