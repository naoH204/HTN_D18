#include "speaker.h"
#include "lcd-i2c.h"
#include <stddef.h>
#include <stdio.h>

static ADC_HandleTypeDef *p_hadc;

// ================= Dữ liệu 3 bài hát =================
static const SongNote_t song1_data[] = {
    {E4, 2}, {E4, 2}, {E4, 4},  // Jin-gle bells,
    {E4, 2}, {E4, 2}, {E4, 4},  // jin-gle bells,
    {E4, 2}, {G4, 2}, {C4, 3}, {D4, 1}, {E4, 8},  // jin-gle all the way,
    {F4, 2}, {F4, 2}, {F4, 2}, {F4, 2},             // Oh, what fun it is
    {F4, 2}, {E4, 2}, {E4, 2},                      // to ride in a
    {E4, 2}, {E4, 2}, {0, 2}                        // one-horse open sleigh
};

static const SongNote_t song2_data[] = {
    {D4, 1}, {D4, 1}, {E4, 2}, {D4, 2}, {G4, 2}, {F4, 4}, // Hap-py birth-day to you
    {D4, 1}, {D4, 1}, {E4, 2}, {D4, 2}, {A4, 2}, {G4, 4}, // Hap-py birth-day to you
    {D4, 1}, {D4, 1}, {B4, 2}, {G4, 2}, {F4, 2}, {E4, 2}, // Hap-py birth-day dear...
    {C4, 1}, {C4, 1}, {0, 2}                             // ...[Name]...
};

static const SongNote_t song3_data[] = {
    {C4, 2}, {C4, 2}, {G4, 2}, {G4, 2}, {A4, 2}, {A4, 2}, {G4, 4}, // Twin-kle, twin-kle, lit-tle star,
    {F4, 2}, {F4, 2}, {E4, 2}, {E4, 2}, {D4, 2}, {D4, 2}, {C4, 4}, // How I won-der what you are.
    {G4, 2}, {G4, 2}, {F4, 2}, {F4, 2}, {E4, 2}, {E4, 2}, {0, 2}    // Up a-bove the world so high...
};

static const SongNote_t* song_list[] = { NULL, song1_data, song2_data, song3_data };

static const uint16_t song_lengths[] = {
    0,
    sizeof(song1_data) / sizeof(SongNote_t),
    sizeof(song2_data) / sizeof(SongNote_t),
    sizeof(song3_data) / sizeof(SongNote_t)
};

static const char* song_names[] = {
    "0. STOPPED       ",
    "1. Jingle Bells  ",
    "2. Happy Birthday",
    "3. Twinkle Star  "
};

static SongSelect_t current_song = SONG_STOPPED;
static SongSelect_t pending_song = SONG_STOPPED; // Bài đang chọn trên LCD trước khi bấm Play
static uint16_t current_note_idx = 0;
static uint32_t last_note_tick = 0;

typedef enum {
    STATE_NOTE_PLAY,
    STATE_NOTE_PAUSE
} PlaySubState_t;

static PlaySubState_t sub_state = STATE_NOTE_PLAY;

extern void Audio_PlayTone(uint16_t freq); 

void Speaker_Init(ADC_HandleTypeDef *hadc) {
    p_hadc = hadc;
    current_song = SONG_STOPPED;
    pending_song = SONG_STOPPED;
    current_note_idx = 0;
    sub_state = STATE_NOTE_PLAY;
    Audio_PlayTone(0); 
}

uint16_t Speaker_ReadTempo(void) {
    if (p_hadc == NULL) return 200;
    
    HAL_ADC_Start(p_hadc);
    if (HAL_ADC_PollForConversion(p_hadc, 5) == HAL_OK) {
        uint32_t adc_val = HAL_ADC_GetValue(p_hadc);
        HAL_ADC_Stop(p_hadc);
        return 100 + ((adc_val * 400) / 4095);
    }
    HAL_ADC_Stop(p_hadc);
    return 200;
}

void Speaker_SelectSong(SongSelect_t song) {
    current_song = song;
    current_note_idx = 0;
    last_note_tick = HAL_GetTick();
    sub_state = STATE_NOTE_PLAY;
    if (song == SONG_STOPPED) {
        Audio_PlayTone(0);
    }
}

const char* Speaker_GetSongName(SongSelect_t song) {
    if (song >= SONG_STOPPED && song <= SONG_3) {
        return song_names[song];
    }
    return "Unknown Mode     ";
}

// ====================== XỬ LÝ NÚT BẤM VÀ LCD ======================
typedef enum {
    MODE_FREE_PLAY = 0, // Mode 0: Đánh đàn tự do
    MODE_RECORDING,     // Mode 1: Bắt đầu lưu nốt
    MODE_SONG_1,        // Mode 2: Chờ phát bài 1
    MODE_SONG_2,        // Mode 3: Chờ phát bài 2
    MODE_SONG_3         // Mode 4: Chờ phát bài 3
} SystemMode_t;

static SystemMode_t current_mode = MODE_FREE_PLAY;
static uint32_t last_press_tick = 0;

void UI_Process_Buttons(void) {
    uint32_t current_tick = HAL_GetTick();
    
    if (current_tick - last_press_tick < 200) {
        return; 
    }

    // --- NÚT PA5 (Chuyển chế độ / Mode) ---
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET) {
        last_press_tick = current_tick;
        
        if (current_song != SONG_STOPPED) {
            Speaker_SelectSong(SONG_STOPPED);
        }
        
        current_mode++;
        if (current_mode > MODE_SONG_3) {
            current_mode = MODE_FREE_PLAY;
        }
        
        LCD_Set_Cursor(0, 0);
        LCD_Send_String("Mode:           "); 
        LCD_Set_Cursor(0, 6);
        
        switch(current_mode) {
            case MODE_FREE_PLAY: 
                LCD_Send_String("FREE PLAY");
                LCD_Set_Cursor(1, 0);
                LCD_Send_String("Status: READY   ");
                break;
            case MODE_RECORDING: 
                LCD_Send_String("RECORDING");
                LCD_Set_Cursor(1, 0);
                LCD_Send_String("Status: REC WAIT");
                break;
            case MODE_SONG_1:    
                LCD_Send_String("SONG 1   ");
                LCD_Set_Cursor(1, 0);
                LCD_Send_String("Jingle Bells    ");
                break;
            case MODE_SONG_2:    
                LCD_Send_String("SONG 2   ");
                LCD_Set_Cursor(1, 0);
                LCD_Send_String("Happy Birthday  ");
                break;
            case MODE_SONG_3:    
                LCD_Send_String("SONG 3   ");
                LCD_Set_Cursor(1, 0);
                LCD_Send_String("Twinkle Star    ");
                break;
        }
    }

    // --- NÚT PA6 (Thực thi / Action) ---
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_RESET) {
        last_press_tick = current_tick;
        
        switch(current_mode) {
            case MODE_FREE_PLAY:
                break;
                
            case MODE_RECORDING:
                LCD_Set_Cursor(1, 0);
                LCD_Send_String("Status: PLAYBACK");
                break;
                
            case MODE_SONG_1:
                if (current_song == SONG_1) {
                    Speaker_SelectSong(SONG_STOPPED);
                    LCD_Set_Cursor(1, 0);
                    LCD_Send_String("Status: STOPPED ");
                } else {
                    Speaker_SelectSong(SONG_1);
                    LCD_Set_Cursor(1, 0);
                    LCD_Send_String("Status: PLAYING ");
                }
                break;
                
            case MODE_SONG_2:
                if (current_song == SONG_2) {
                    Speaker_SelectSong(SONG_STOPPED);
                    LCD_Set_Cursor(1, 0);
                    LCD_Send_String("Status: STOPPED ");
                } else {
                    Speaker_SelectSong(SONG_2);
                    LCD_Set_Cursor(1, 0);
                    LCD_Send_String("Status: PLAYING ");
                }
                break;
                
            case MODE_SONG_3:
                if (current_song == SONG_3) {
                    Speaker_SelectSong(SONG_STOPPED);
                    LCD_Set_Cursor(1, 0);
                    LCD_Send_String("Status: STOPPED ");
                } else {
                    Speaker_SelectSong(SONG_3);
                    LCD_Set_Cursor(1, 0);
                    LCD_Send_String("Status: PLAYING ");
                }
                break;
        }
    }
}

// ====================== FSM PHÁT NHẠC ======================
void Speaker_Process_FSM(void) {
    if (current_song == SONG_STOPPED) {
        return;
    }

    uint32_t now = HAL_GetTick();
    uint16_t base_tempo = Speaker_ReadTempo(); 
    const SongNote_t* p_song = song_list[current_song];
    uint16_t total_notes = song_lengths[current_song];

    switch (sub_state) {
        case STATE_NOTE_PLAY: {
            uint16_t freq = p_song[current_note_idx].note;
            uint32_t play_duration = p_song[current_note_idx].duration * base_tempo;

            Audio_PlayTone(freq);
            
            LCD_Set_Cursor(1, 0);
            if (freq == 0) {
                LCD_Send_String("Note:REST       ");
            } else {
                const char* note_name = "???";
                if (freq == C4) note_name = "C4 ";
                else if (freq == D4) note_name = "D4 ";
                else if (freq == E4) note_name = "E4 ";
                else if (freq == F4) note_name = "F4 ";
                else if (freq == G4) note_name = "G4 ";
                else if (freq == A4) note_name = "A4 ";
                else if (freq == B4) note_name = "B4 ";
                else if (freq == C5) note_name = "C5 ";
                
                LCD_Send_String("Note:");
                LCD_Send_String((char*)note_name);
            }
            
            LCD_Set_Cursor(1, 10);
            char tempo_str[10];
            sprintf(tempo_str, "T:%03d", base_tempo);
            LCD_Send_String(tempo_str);

            if (now - last_note_tick >= play_duration) {
                Audio_PlayTone(0); 
                last_note_tick = now;
                sub_state = STATE_NOTE_PAUSE;
            }
            break;
        }

        case STATE_NOTE_PAUSE: {
            if (now - last_note_tick >= 30) {
                current_note_idx++;
                if (current_note_idx >= total_notes) {
                    current_note_idx = 0; 
                }
                last_note_tick = now;
                sub_state = STATE_NOTE_PLAY;
            }
            break;
        }
    }
}