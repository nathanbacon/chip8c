//
//  sdl_helpers.h
//  chip8c
//
//  Created by Nathan Gelman on 3/31/17.
//  Copyright © 2017 Cat Daughters Ltd. All rights reserved.
//

#ifndef sdl_helpers_h
#define sdl_helpers_h

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;
    SDL_Texture* gTexture;
} window;

typedef struct{
    SDL_Event e;
    bool key[16];
    bool quit;
    bool is_pressed;
    unsigned int delay;
    unsigned int sound;
    bool draw_flag;
} event;

typedef struct {
    int freq;
    uint runningIndex;
} data;

extern const int WIDTH;
extern const int HEIGHT;
extern const int FREQUENCY;
extern const Sint16 AMPLITUDE;

void ePoll(event*);
bool sdl_init(window*);
bool sdl_audio_init();
void drawScreen(window*, uint32_t[32][64]);
void AudioCallback(void* userdata, Uint8* stream, int StreamLength);
void generateSquare(data*, Sint16*, int);
void cclose(window*);

#endif /* sdl_helpers_h */
