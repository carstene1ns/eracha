
#include <SDL_image.h>
#include "video.h"
#include "game.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

int InitVideo() {
  window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    GAME_WIDTH, GAME_HEIGHT, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
  if (window == NULL)
    return 0;

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL)
    return 0;

  atexit(DeinitVideo);

  int flags = (IMG_INIT_PNG);
  if (!(IMG_Init(flags) & flags))
    return 0;

  atexit(IMG_Quit);

  /* all ok */
  return 1;
}

void DeinitVideo() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}
