
#include <SDL.h>

#include "game.h"
#include "event.h"
#include "audio.h"
#include "level.h"
#include "video.h"

int EventLoop() {
  SDL_Event event;

  /* Check for new events */
  while(SDL_PollEvent(&event)) {
    
    if(event.type == SDL_KEYDOWN) {
      switch(event.key.keysym.sym) {

        /* DEBUG: Level switching */
        case SDLK_PAGEUP:
          if(level < LEVEL_COUNT-1) {
            level++;
            InitLevel();
          }
          break;
        case SDLK_PAGEDOWN:
          if(level > LEVEL_01) {
            level--;
            InitLevel();
          }
          break;
        case SDLK_HOME:
          DrawLevel();
          break;
        case SDLK_END:
          PlayMusic();
          break;

        /* Volume (always available) */
        case SDLK_F7:
          MusicVolumeDown();
          break;
        case SDLK_F8:
          MusicVolumeUp();
          break;
        case SDLK_F9:
          EffectVolumeDown();
          break;
        case SDLK_F10:
          EffectVolumeUp();
          break;

        /* Quit/Menu (always available) */
        case SDLK_ESCAPE:
          return 1;
          break;
      }
    }

    /* If a quit event has been sent */
    if (event.type == SDL_QUIT)
      return 1;
  }

  return 0;
}
