#ifndef SPEAKER_H
#define SPEAKER_H

#include "stm32f1xx_hal.h"
/**
  ******************************************************************************
  * @file           : speaker.h
  * @brief          : cau hinh tan so not nhac va bai hat

  ******************************************************************************
  */
#define C4   262
#define D4   294
#define E4   330
#define F4   349
#define G4   392
#define A4   440
#define B4   494
#define C5   523
#define D5   587
#define E5   659
#define F5   698
#define G5   784
#define REST 0

typedef struct {
    uint16_t note;      // tan so not nhac
    uint16_t duration;  // truong do 
} SongNote_t;

// Cac che do
typedef enum {
    SONG_STOPPED = 0,
    SONG_1 = 1,
    SONG_2 = 2,
    SONG_3 = 3
} SongSelect_t;

//================================ API ===========================
void Speaker_Init(ADC_HandleTypeDef *hadc);
void Speaker_SelectSong(SongSelect_t song);
uint16_t Speaker_ReadTempo(void);
void Speaker_Process_FSM(void);
const char* Speaker_GetSongName(SongSelect_t song);
void UI_Process_Buttons(void);
#endif 