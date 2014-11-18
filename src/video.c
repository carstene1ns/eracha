
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include "video.h"
#include "game.h"
#include "file.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_Texture *backgroundTexture = NULL;
static SDL_Texture *tilesetTexture = NULL;

int InitVideo() {
  window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            GAME_WIDTH * 3, GAME_HEIGHT * 3, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
  if(window == NULL)
    return 0;

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(renderer == NULL)
    return 0;

  atexit(DeinitVideo);

  int flags = (IMG_INIT_PNG);
  if(!(IMG_Init(flags) & flags))
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
  if(backgroundTexture != NULL)
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

int LoadTileset(int set) {
  /* unload previous */
  if(tilesetTexture != NULL)
    SDL_DestroyTexture(tilesetTexture);

  unsigned int size;
  char *buf = LoadTilesetF(set, &size);
  if(buf == NULL)
    return 0;

  /* generate palette */
  SDL_Color cols[PALETTE_COLORS - 1];

  /* set transparency (cyan) */
  cols[0].r = 0;
  cols[0].g = 255;
  cols[0].b = 255;
  cols[0].a = 0;

  /* set shifted colors */
  for(int i = 1; i < PALETTE_COLORS - 1; i++) {
    int palpos = size - PALETTE_SIZE + i * 3;

    cols[i].r = (buf[palpos] << 2);
    cols[i].g = (buf[palpos + 1] << 2);
    cols[i].b = (buf[palpos + 2] << 2);
    cols[i].a = 255; /* opaque */
  }

  SDL_Palette *pal = SDL_AllocPalette(PALETTE_COLORS);
  if(pal == NULL) {
    printf("Failed to allocate palette! Error: %s\n", SDL_GetError());
    return 0;
  }
  SDL_SetPaletteColors(pal, cols, 0, PALETTE_COLORS);

  /* cache tileset in memory */
  SDL_Surface *temp = SDL_CreateRGBSurface(0, TILE_SIZE * TILE_COLS,
                      TILE_SIZE * TILE_COUNT / TILE_COLS, 8, 0, 0, 0, 0);
  if(temp == NULL) {
    printf("Tile surface could not be created! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }

  /* copy tiles */
  for(int i = 0; i < TILE_ROWS; i++) {
    for(int j = 0; j < TILE_COLS; j++) {
      char *p = (char *)buf + ((i * TILE_COLS + j) * TILE_BLOCK);

      SDL_Surface *s = SDL_CreateRGBSurfaceFrom(p, TILE_SIZE, TILE_SIZE, 8, TILE_SIZE, 0,
                       0, 0, 0);
      if(s == NULL) {
        printf("Temporary surface could not be created from tile %d! SDL_Error: %s\n",
               (i * TILE_COLS + j), SDL_GetError());
        return 0;
      }

      SDL_Rect d = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };

      /* copy to big surface and cleanup */
      SDL_BlitSurface(s, NULL, temp, &d);
      SDL_FreeSurface(s);
    }
  }
  /* set palette and transparency */
  SDL_SetSurfacePalette(temp, pal);
  SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 255, 255));

  /* make it a texture */
  tilesetTexture = SDL_CreateTextureFromSurface(renderer, temp);
  if(tilesetTexture == NULL) {
    printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }

  /* cleanup */
  SDL_FreeSurface(temp);
  SDL_FreePalette(pal);
  free(buf);

  return 1;
}

void DrawLevel() {
  /* clear */
  SDL_RenderClear(renderer);

  /* draw background (with parallax effect) */
  int w, h;
  SDL_QueryTexture(backgroundTexture, NULL, NULL, &w, &h);
  int bg_shift = (int)(level.shift_x * BACKGROUND_SHIFT) % w;
  SDL_Rect backgroundClipRect = { bg_shift, 0, w, h };
  SDL_Rect backgroundRect = { 0, 0, (w - bg_shift) *STATIC_ZOOM, h * STATIC_ZOOM };
  SDL_RenderCopy(renderer, backgroundTexture, &backgroundClipRect, &backgroundRect);
  backgroundClipRect.x = 0;
  backgroundRect.x = (w - bg_shift) * STATIC_ZOOM;
  backgroundRect.w = w * STATIC_ZOOM;
  SDL_RenderCopy(renderer, backgroundTexture, &backgroundClipRect, &backgroundRect);

  /* draw level layer */
  int x, y, x1, x2, y1, y2;
  unsigned int map_x, map_y;

  /* get scroll position */
  map_x = level.shift_x / TILE_SIZE;
  x1 = (level.shift_x % TILE_SIZE) * -1;
  x2 = x1 + GAME_WIDTH + (x1 == 0 ? 0 : TILE_SIZE);

  map_y = level.shift_y / TILE_SIZE;
  y1 = (level.shift_y % TILE_SIZE) * -1;
  y2 = y1 + GAME_HEIGHT + (y1 == 0 ? 0 : TILE_SIZE);

  for(y = y1; y < y2; y += TILE_SIZE) {
    map_x = level.shift_x / TILE_SIZE;

    for(x = x1; x < x2; x += TILE_SIZE) {
      unsigned int block = level.map[map_y][map_x];

      /* skip empty blocks */
      if(block != 0) {

        /* magic to get correct tile */
        int row = block / TILE_COLS;
        int col = block - row * TILE_COLS;

        SDL_Rect src, dest;

        src.x = col * TILE_SIZE;
        src.y = row * TILE_SIZE;
        src.w = TILE_SIZE;
        src.h = TILE_SIZE;

        dest.x = x * STATIC_ZOOM;
        dest.y = y * STATIC_ZOOM;
        dest.w = TILE_SIZE * STATIC_ZOOM;
        dest.h = TILE_SIZE * STATIC_ZOOM;

        /* copy block to window */
        SDL_RenderCopy(renderer, tilesetTexture, &src, &dest);
      }

      map_x++;
    }

    map_y++;
  }

  /* draw enemies and caveman layer */

  /* draw foreground layer */

  /* show finished image */
  SDL_RenderPresent(renderer);
}
