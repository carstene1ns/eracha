
#include <SDL.h>

#include "event.h"
#include "audio.h"
#include "game.h"
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
          if(level.number < LEVEL_COUNT - 1) {
            level.number++;
            InitLevel();
          }
          break;
        case SDLK_PAGEDOWN:
          if(level.number > LEVEL_01) {
            level.number--;
            InitLevel();
          }
          break;
        case SDLK_HOME:
          DrawLevel();
          break;
        case SDLK_END:
          PlayMusic();
          break;

        /* DEBUG: Level viewport */
        case SDLK_DOWN:
          level.shift_y += STATIC_SCROLL;
          if(level.shift_y + GAME_HEIGHT >= level.max_y)
            level.shift_y = level.max_y - GAME_HEIGHT;
          break;
        case SDLK_UP:
          level.shift_y -= STATIC_SCROLL;
          if(level.shift_y < 0)
            level.shift_y = 0;
          break;
        case SDLK_RIGHT:
          level.shift_x += STATIC_SCROLL;
          if(level.shift_x + GAME_WIDTH >= level.max_x)
            level.shift_x = level.max_x - GAME_WIDTH;
          break;
        case SDLK_LEFT:
          level.shift_x -= STATIC_SCROLL;
          if(level.shift_x < 0)
            level.shift_x = 0;
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
    if(event.type == SDL_QUIT)
      return 1;
  }

  return 0;
}
