#include "speaker.h"
#include <stddef.h>
#include "buzzer.h"

static ADC_HandleTypeDef *p_hadc;

//// ================= Du lieu  3 bai hat  =================
////        1.Jingle Bells
static const SongNote_t song1_data[] = {
    {E4, 2}, {E4, 2}, {E4, 4},  // Jin-gle bells,
    {E4, 2}, {E4, 2}, {E4, 4},  // jin-gle bells,
    {E4, 2}, {G4, 2}, {C4, 3}, {D4, 1}, {E4, 8},  // jin-gle all the way,
    {F4, 2}, {F4, 2}, {F4, 2}, {F4, 2},           // Oh, what fun it is
    {F4, 2}, {E4, 2}, {E4, 2},                    // to ride in a
    {E4, 2}, {E4, 2}, {0, 2}                      // one-horse... [Nốt nghỉ]
};

// 		2.Happy Birthday
static const SongNote_t song2_data[] = {
    {D4, 1}, {D4, 1}, {E4, 2}, {D4, 2}, {G4, 2}, {F4, 4}, // Hap-py birth-day to you
    {D4, 1}, {D4, 1}, {E4, 2}, {D4, 2}, {A4, 2}, {G4, 4}, // Hap-py birth-day to you
    {D4, 1}, {D4, 1}, {B4, 2}, {G4, 2}, {F4, 2}, {E4, 2}, // Hap-py birth-day dear...
    {C4, 1}, {C4, 1}, {0, 2}                                    // ...[Name]...
};

//		3.Twinkle Star
static const SongNote_t song3_data[] = {
    {C4, 2}, {C4, 2}, {G4, 2}, {G4, 2}, {A4, 2}, {A4, 2}, {G4, 4}, // Twin-kle, twin-kle, lit-tle star,
    {F4, 2}, {F4, 2}, {E4, 2}, {E4, 2}, {D4, 2}, {D4, 2}, {C4, 4}, // How I won-der what you are.
    {G4, 2}, {G4, 2}, {F4, 2}, {F4, 2}, {E4, 2}, {E4, 2}, {0, 2}   // Up a-bove the world so...
};



//// Bài hát: Twinkle Twinkle Little Star (Quãng 5 - Trầm ấm)
//static const SongNote_t song_octave5_data[] = {
//    {C5, 2}, {C5, 2}, {G5, 2}, {G5, 2}, {A5, 2}, {A5, 2}, {G5, 4}, // Twin-kle, twin-kle, lit-tle star,
//    {F5, 2}, {F5, 2}, {E5, 2}, {E5, 2}, {D5, 2}, {D5, 2}, {C5, 4}, // How I won-der what you are.
//    {G5, 2}, {G5, 2}, {F5, 2}, {F5, 2}, {E5, 2}, {E5, 2}, {D5, 4}, // Up a-bove the world so high,
//    {G5, 2}, {G5, 2}, {F5, 2}, {F5, 2}, {E5, 2}, {E5, 2}, {D5, 4}, // Like a dia-mond in the sky.
//    {C5, 2}, {C5, 2}, {G5, 2}, {G5, 2}, {A5, 2}, {A5, 2}, {G5, 4}, // Twin-kle, twin-kle, lit-tle star,
//    {F5, 2}, {F5, 2}, {E5, 2}, {E5, 2}, {D5, 2}, {D5, 2}, {C5, 4}, // How I won-der what you are.
//    {0, 2}                                                         // Nốt nghỉ kết thúc bài
//};
static const SongNote_t* song_list[] = { NULL, song1_data, song2_data, song3_data };
//static const SongNote_t* song_list[] = { NULL, song_octave5_data};

static const uint16_t song_lengths[] = {
    0,
    sizeof(song1_data) / sizeof(SongNote_t),
    sizeof(song2_data) / sizeof(SongNote_t),
    sizeof(song3_data) / sizeof(SongNote_t)
//		sizeof(song_octave5_data) / sizeof(SongNote_t)

};

static const char* song_names[] = {
    "0. STOPPED     ",
    "1. Jingle Bells",
    "2. Happy Birthday",
    "3. Twinkle Star "
};


static SongSelect_t current_song = SONG_STOPPED;
static uint16_t current_note_idx = 0;
static uint32_t last_note_tick = 0;

typedef enum {
    STATE_NOTE_PLAY,
    STATE_NOTE_PAUSE
} PlaySubState_t;

static PlaySubState_t sub_state = STATE_NOTE_PLAY;


extern void Buzzer_PlayNote(uint16_t freq); 

void Speaker_Init(ADC_HandleTypeDef *hadc) {
    p_hadc = hadc;
    
    // Ép buộc trạng thái dừng hoàn toàn khi khởi động
    current_song = SONG_STOPPED;
    current_note_idx = 0;
    sub_state = STATE_NOTE_PLAY;
    Buzzer_PlayNote(0); // Đảm bảo PWM dừng phát
}
// =========================== ReadTempo ==============================
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
// ===================== Chon bai hat =================================
void Speaker_SelectSong(SongSelect_t song) {
    current_song = song;
    current_note_idx = 0;
    last_note_tick = HAL_GetTick();
    sub_state = STATE_NOTE_PLAY;
    if (song == SONG_STOPPED) {
        Buzzer_PlayNote(0);
    }
}

const char* Speaker_GetSongName(SongSelect_t song) {
    if (song <= SONG_3) {
        return song_names[song];
    }
    return "Unknown Mode   ";
}

// ====================== FSM =========================================
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

            Buzzer_PlayNote(freq);

            if (now - last_note_tick >= play_duration) {
                Buzzer_PlayNote(0); 
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
