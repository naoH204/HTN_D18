#include "buzzer.h"

// Hàm khởi tạo còi
void Buzzer_Init(void) 
{
    // Kích hoạt băm xung PWM trên Timer 1 Kênh 1 (Chân PA8)
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

// Hàm phát nốt nhạc không méo tiếng
void Buzzer_PlayNote(uint16_t freq) 
{
    if (freq == 0) 
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0); 
        return;
    }
    
    uint32_t arr_value = (1000000 / freq) - 1;
    uint32_t ccr_value = arr_value / 2; // Giữ Duty Cycle 50%
    
    __HAL_TIM_SET_AUTORELOAD(&htim1, arr_value);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr_value);
    __HAL_TIM_SET_COUNTER(&htim1, 0);   // Chống méo tiếng
}
