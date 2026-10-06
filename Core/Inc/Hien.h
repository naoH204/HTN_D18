#ifndef Hien_h
#define Hien_h
//============================	include 	=============================================
#include "stm32f1xx_hal.h"


//============================ define  =============================================
/**
*@brief cau hinh not nhac va cac che do phat
*
*/
#define C4   262
#define D4   294
#define E4   330
#define F4   350
#define G4   392
#define A4   440
#define B4   494
#define C5   523
#define REST 0

typedef struct {
    uint16_t note;      // Tan so note
    uint16_t duration;  // Do dai cua not
} SongNote_t;

// cac che do phat
typedef enum {
    SONG_STOPPED = 0,
    SONG_1 = 1,
    SONG_2 = 2,
    SONG_3 = 3
} SongSelect_t;
//========================== api ===============================================
void Speaker_Init(TIM_HandleTypeDef *htim, uint32_t channel, ADC_HandleTypeDef *hadc);
void Speaker_PlayTone(uint16_t freq);
uint16_t Speaker_ReadTempo(void);
void Speaker_SelectSong(SongSelect_t song);
void Speaker_Process_FSM(void);

#endif 