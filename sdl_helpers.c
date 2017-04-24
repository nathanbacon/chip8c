#include "sdl_helpers.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

const int WIDTH = 64;
const int HEIGHT = 32;
const int FREQUENCY = 48000;
const Sint16 AMPLITUDE = 20000;

data wave;

bool sdl_init(window* window) {
    
    bool success = true;

    window->gWindow = NULL;
    window->gRenderer = NULL;
    window->gTexture = NULL;
    
    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO))
    {
        printf("Could not initialize audio or video\nERROR: %s", SDL_GetError());
        success = false;
    }
    
    window->gWindow = SDL_CreateWindow("Cat Daughter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH*10, HEIGHT*10, SDL_WINDOW_SHOWN);
    if(window->gWindow == NULL){
        
        printf("Could not initialize SDL\nERROR:%s", SDL_GetError());
        success = false;
        
    } else {
        
        window->gRenderer = SDL_CreateRenderer(window->gWindow, -1, SDL_RENDERER_ACCELERATED);
        SDL_RenderSetLogicalSize(window->gRenderer, WIDTH*10, HEIGHT*10);
        
        if (window->gRenderer == NULL) {
            
            printf("Could not initialize renderer\nERROR:%s", SDL_GetError());
            success = false;
            
        } else {
            
            SDL_SetRenderDrawColor(window->gRenderer, 0x00, 0x00, 0x00, 0x00);
            
            window->gTexture = SDL_CreateTexture(window->gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
            if(window->gTexture == NULL)
            {
                printf("Could not initialize texture\nERROR: %s", SDL_GetError());
                success = false;
            }
            else if(!sdl_audio_init())
            {
                success = false;
            }
            
        }
    }
    

    return success;
}

bool sdl_audio_init()
{
    bool success = true;
    
    wave.freq = 200;
    wave.runningIndex = 0;
    
    SDL_AudioSpec want = {0};
    SDL_AudioSpec have;
    
    want.format = AUDIO_S16LSB;
    want.samples = 2048;
    want.channels = 1;
    want.freq = FREQUENCY;
    want.callback = AudioCallback;
    want.userdata = (void*) &wave;
    
    SDL_OpenAudio(&want, &have);
    
    if(want.format != have.format)
    {
        printf("Couldn't get requested format\n");
        success = false;
    }
    
    return success;
}

void AudioCallback(void* userdata, Uint8* stream, int StreamLength){
    data* wave = (data*) userdata;
    Sint16* sampleStream = (Sint16*) stream;
    
    generateSquare(wave, sampleStream, StreamLength/2);
    
    if(wave->runningIndex >= FREQUENCY/wave->freq*100)
        wave->runningIndex = 0;
}

void generateSquare(data* wave, Sint16* stream, int length)
{
    int halfPeriod = FREQUENCY/wave->freq/2;
    
    for(int i = 0; i < length; i++)
    {
        *stream++ = ((wave->runningIndex++/halfPeriod)%2) ? AMPLITUDE : -AMPLITUDE;
    }
}

void ePoll(event* event){
    
    while(SDL_PollEvent(&event->e)){
        if(event->e.type == SDL_QUIT)
            event->quit = true;
        else if(event->e.type == SDL_KEYUP)
            switch(event->e.key.keysym.sym){
                case SDLK_1:
                    event->key[1] = false;
                    break;
                case SDLK_2:
                    event->key[2] = false;
                    break;
                case SDLK_3:
                    event->key[3] = false;
                    break;
                case SDLK_4:
                    event->key[0xC] = false;
                    break;
                case SDLK_q:
                    event->key[4] = false;
                    break;
                case SDLK_w:
                    event->key[5] = false;
                    break;
                case SDLK_e:
                    event->key[6] = false;
                    break;
                case SDLK_r:
                    event->key[0xD] = false;
                    break;
                case SDLK_a:
                    event->key[7] = false;
                    break;
                case SDLK_s:
                    event->key[8] = false;
                    break;
                case SDLK_d:
                    event->key[9] = false;
                    break;
                case SDLK_f:
                    event->key[0xE] = false;
                    break;
                case SDLK_z:
                    event->key[0xA] = false;
                    break;
                case SDLK_x:
                    event->key[0] = false;
                    break;
                case SDLK_c:
                    event->key[0xB] = false;
                    break;
                case SDLK_v:
                    event->key[0xF] = false;
                    break;
                default:
                    break;
            }
        else if(event->e.type == SDL_KEYDOWN){
            event->is_pressed = true;
            switch(event->e.key.keysym.sym){
                case SDLK_1:
                    event->key[1] = true;
                    break;
                case SDLK_2:
                    event->key[2] = true;
                    break;
                case SDLK_3:
                    event->key[3] = true;
                    break;
                case SDLK_4:
                    event->key[0xC] = true;
                    break;
                case SDLK_q:
                    event->key[4] = true;
                    break;
                case SDLK_w:
                    event->key[5] = true;
                    break;
                case SDLK_e:
                    event->key[6] = true;
                    break;
                case SDLK_r:
                    event->key[0xD] = true;
                    break;
                case SDLK_a:
                    event->key[7] = true;
                    break;
                case SDLK_s:
                    event->key[8] = true;
                    break;
                case SDLK_d:
                    event->key[9] = true;
                    break;
                case SDLK_f:
                    event->key[0xE] = true;
                    break;
                case SDLK_z:
                    event->key[0xA] = true;
                    break;
                case SDLK_x:
                    event->key[0] = true;
                    break;
                case SDLK_c:
                    event->key[0xB] = true;
                    break;
                case SDLK_v:
                    event->key[0xF] = true;
                    break;
                default:
                    break;
            }
        }
    }
    
}

void cclose(window* window){
    
    //Deallocate memory
    SDL_DestroyTexture(window->gTexture);
    window->gTexture = NULL;
    SDL_DestroyRenderer(window->gRenderer);
    SDL_DestroyWindow(window->gWindow);
    window->gRenderer = NULL;
    window->gWindow = NULL;
    SDL_CloseAudio();
    //quit sdl subsystems.
    SDL_Quit();
}

void drawScreen(window* window, uint32_t pixels[32][64])
{
    SDL_UpdateTexture(window->gTexture, NULL, pixels, 64*sizeof(uint32_t));
    
    SDL_RenderClear(window->gRenderer);
    
    SDL_RenderCopy(window->gRenderer, window->gTexture, NULL, NULL);
    
    SDL_RenderPresent(window->gRenderer);
}


