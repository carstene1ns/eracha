
#include <SDL_image.h>
#include "video.h"
#include "game.h"
#include "file.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_Texture* backgroundTexture = NULL;

int InitVideo() {
  window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    GAME_WIDTH * 3, GAME_HEIGHT * 3, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
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

int LoadBackground(int pic) {
  /* unload previous */
  if (backgroundTexture != NULL)
    SDL_DestroyTexture(backgroundTexture);

  unsigned int size;
  char *buf = LoadBackgroundF(pic, &size);
  backgroundTexture = IMG_LoadTexture_RW(renderer, SDL_RWFromConstMem(buf, size), 1);
  free(buf);
  if(!backgroundTexture) {
    printf("IMG Error: %s\n", IMG_GetError());
    return 0;
  }

  /* all ok */
  return 1;
}

void DrawLevel() {
  /* clear */
  SDL_RenderClear(renderer);

  /* draw background */
  int w, h;
  SDL_QueryTexture(backgroundTexture, NULL, NULL, &w, &h);
  SDL_Rect backgroundRect;
  backgroundRect.x = 0;
  backgroundRect.y = 0;
  backgroundRect.w = w * 3;
  backgroundRect.h = h * 3;
  SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);

  /* draw level layer */

  /* draw enemies and caveman layer */

  /* draw foreground layer */

  /* show finished image */
  SDL_RenderPresent(renderer);
}
