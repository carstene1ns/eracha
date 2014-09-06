
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

#include "game.h"
#include "helpers.h"
#include "audio.h"
#include "video.h"
#include "level.h"

int state = STATE_LEVEL;

int InitSDL() {
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_AUDIO) != 0)
    return 0;

  atexit(SDL_Quit);

  if(!InitVideo())
    return 0;

  if(!InitAudio())
    return 0;

  /* all ok */
  return 1;
}

int main(int argc, char *argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  if(!InitSDL()) {
    printf("Failed to initialize SDL, Error: %s\n", SDL_GetError());
    exit(1);
  }

  level.number = LEVEL_01;

  int quit = 0;
  while(!quit) {
  	switch(state) {
      case STATE_LEVEL:
        LevelState();
        break;
      default:
        quit = 1;
        break;
    }
  }

  /* exit gracefully */
  exit(0);
}
