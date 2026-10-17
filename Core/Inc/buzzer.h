#ifndef BUZZER_H
#define BUZZER_H

#include "main.h"

extern TIM_HandleTypeDef htim1;

void Buzzer_Init(void);
void Buzzer_PlayNote(uint16_t freq);

#endif /* BUZZER_H */
