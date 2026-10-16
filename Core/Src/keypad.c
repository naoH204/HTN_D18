#include "keypad.h"
// bien noi bo
uint8_t current_key = 0;
uint8_t confirmed_key = 0;
uint16_t debounce_tick = 0;

uint8_t current_ctrl = 0;
uint8_t confirmed_ctrl = 0;
uint16_t debounce_ctrl_tick = 0;

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

		uint8_t read_ctrl = ~((GPIOA->IDR >> 5) & 0x03); 
    if (read_ctrl != current_ctrl) 
    {
        current_ctrl = read_ctrl;
        debounce_ctrl_tick = 0;
    }
    else 
    {
        debounce_ctrl_tick++;
        if (debounce_ctrl_tick == 20) confirmed_ctrl = current_ctrl; // Chốt 20ms
    }
}

// Hàm API để trả giá trị phím ra cho hệ thống dùng
uint8_t Keypad_GetConfirmedKey(void)
{
    return confirmed_key;
}
uint8_t Keypad_GetControlKey(void) {
    return confirmed_ctrl;
}