
#include <SDL2/SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "chip8.h"
#include "sdl_helpers.h"

int main(int argc, const char * argv[]) {
    
    if(argc < 2)
    {
        printf("Usage: chip8c <game>\n");
        return 1;
    }

    window screen;
    
    event tracker;
    
    initialize(&tracker, argv[1]);
    sdl_init(&screen);
    
    
    while(!tracker.quit){
        
        //check the events
        ePoll(&tracker);
        
        emulateCycle();
        
        if(tracker.sound)
            SDL_PauseAudio(0);
        else
            SDL_PauseAudio(1);
        
        if(tracker.draw_flag){
            
            drawScreen(&screen, display);
            
            tracker.draw_flag = false;
            
        }
        
        SDL_Delay(3);
        
    }
    
    cclose(&screen);
    return 0;
}







