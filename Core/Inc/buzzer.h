#ifndef BUZZER_H
#define BUZZER_H

#include "main.h"

// Gọi biến htim1 từ main.c sang để file này dùng được
extern TIM_HandleTypeDef htim1;

// --- API CỦA MODULE 1 (SV1) ---
void Buzzer_Init(void);
void Buzzer_PlayNote(uint16_t freq);

#endif /* BUZZER_H */
