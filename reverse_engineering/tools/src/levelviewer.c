
/******************************************************************************
 * levelviewer.c ERACHA level viewer
 * written by carstene1ns, 2014, 2023
 *****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>

/* the font */
#include "pixelfont.h"
#include "helper.h"

/* window parameters */
#define WINDOW_TITLE "ERACHA level viewer"
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define BARSIZE 24

/* file parameters */
#define ENDOFSMEHEADER 41
#define ENDOFATRHEADER 4
#define ENDOFTILHEADER 23

/* typedefs make our life with structs easier */
typedef struct
{
  bool running, needsupdate, checkerboard, dragging;
  int zoom, overlay, width, height;
  SDL_Point pos, mouseoffset, dragstart, dragoffset;
} viewerstate_t;

typedef struct
{
  int width, height;
  char name[13];
  uint16_t **background;
  uint32_t **attributes;
} levelinfo_t;

typedef struct
{
  int size, count;
} tilesetinfo_t;

/* globals */
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *tiletex = NULL;
SDL_Texture *overlaytex = NULL;
SDL_Texture *checkerboardtex = NULL;
SDL_Cursor* hand_cursor = NULL;
SDL_Cursor* cursor = NULL;
TTF_Font *font = NULL;

SDL_Color white = { 0xFF, 0xFF, 0xFF, 0 };

viewerstate_t viewerstate = { true, true, true, false,
  1, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
  {0, 0}, {0, 0}, {0, 0}, {0, 0} };
levelinfo_t levelinfo = { 0, 0, "", NULL, NULL };
tilesetinfo_t tilesetinfo = { 0, 0 };

/* helper functions */
void cleanup()
{
  if(font)
    TTF_CloseFont(font);

  if (TTF_WasInit())
    TTF_Quit();

  if(cursor)
    SDL_FreeCursor(cursor);

  if(tiletex)
    SDL_DestroyTexture(tiletex);
  if(checkerboardtex)
    SDL_DestroyTexture(checkerboardtex);
  if(overlaytex)
    SDL_DestroyTexture(overlaytex);

  if(renderer)
    SDL_DestroyRenderer(renderer);

  if(window)
    SDL_DestroyWindow(window);

  SDL_Quit();
}

bool init()
{
  /* init SDL library */
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  /* always do cleanup */
  atexit(cleanup);

  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
  if(window == NULL)
  {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(renderer == NULL)
  {
    printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  /* create some colors for our overlay */
  SDL_Surface *temp = SDL_CreateRGBSurface(0, 8, 3 * 8, 32, 0x000000ff,
    0x0000ff00, 0x00ff0000, 0xff000000);
  if(temp == NULL)
  {
    printf("Surface could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  for(int i = 0; i < 3; i++)
  {
    SDL_Rect d;
    d.x = 0;
    d.y = i * 8;
    d.w = 8;
    d.h = 8;

    /* add different colors to surface */
    SDL_FillRect(temp, &d, SDL_MapRGBA(temp->format,
    (i == 0) ? 255 : 0, (i == 1) ? 255 : 0, (i == 2) ? 255 : 0, 100));
  }

  overlaytex = SDL_CreateTextureFromSurface(renderer, temp);
  if(overlaytex == NULL)
  {
    printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }
  SDL_FreeSurface(temp);

  /* create transparency effect */
  #define BLOCKSIZE 8
  temp = SDL_CreateRGBSurface(0, 2 * BLOCKSIZE, 2 * BLOCKSIZE, 32, 0x000000ff,
    0x0000ff00, 0x00ff0000, 0xff000000);
  if(temp == NULL)
  {
    printf("Surface could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  for(int i = 0; i < 2; i++)
  {
    for(int j = 0; j < 2; j++)
    {
      SDL_Rect d = { i * BLOCKSIZE, j * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE };

      /* make checkerboard */
      SDL_FillRect(temp, &d, i == j ? SDL_MapRGBA(temp->format, 128, 128, 128, 100) :
        SDL_MapRGBA(temp->format, 255, 255, 255, 100));
    }
  }
  #undef BLOCKSIZE

  checkerboardtex = SDL_CreateTextureFromSurface(renderer, temp);
  if(checkerboardtex == NULL)
  {
    printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }
  SDL_FreeSurface(temp);

  /* dragging cursor */
  hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  if(hand_cursor == NULL)
  {
    printf("Cursor could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }
  /* normal cursor */
  cursor = SDL_GetCursor();

  /* init SDL2_TTF library */
  if(TTF_Init() == -1) {
    printf("Error while TTF_Init: %s\n", TTF_GetError());
    return false;
  }

  /* load font */
#if 0
  font = TTF_OpenFont("font/04B_03__.TTF", 8);
#else
  font = TTF_OpenFontRW(SDL_RWFromMem(pixelfont, pixelfont_len), 1, 8);
#endif
  if(!font) {
    printf("Error while TTF_OpenFont: %s\n", TTF_GetError());
    return false;
  }

  /* all good */
  return true;
}

bool loadlevel(char *levelfilename, char *attributesfilename)
{
  unsigned char *buf;
  unsigned long size;

  /* load level file */
  buf = loadfile2(levelfilename, &size);
  if(buf == NULL)
    return false;

  /* parse file header */
  levelinfo.width = buf[ENDOFSMEHEADER - 8] | (buf[ENDOFSMEHEADER - 7] << 8);
  levelinfo.height = buf[ENDOFSMEHEADER - 4] | (buf[ENDOFSMEHEADER - 3] << 8);

  /* cache level in memory */
  levelinfo.background = (uint16_t **)malloc(sizeof(uint16_t *) * levelinfo.height + sizeof(uint16_t) * levelinfo.width * levelinfo.height);
  if(levelinfo.background == NULL)
  {
    perror("malloc");
    return false;
  }
  uint16_t *bgptr = (uint16_t *)(levelinfo.background + levelinfo.height);
  for(int y = 0; y < levelinfo.height; y++) {
    levelinfo.background[y] = (bgptr + levelinfo.width * y);

    for(int x = 0; x < levelinfo.width; x++) {
      levelinfo.background[y][x] = (buf[ENDOFSMEHEADER + y * levelinfo.width * 2 + x * 2]) |
        (buf[ENDOFSMEHEADER + y * levelinfo.width * 2 + x * 2 + 1] << 8);
    }
  }

  /* cleanup */
  free(buf);
  buf = NULL;
  size = 0L;

  /* load attributes file */
  buf = loadfile2(attributesfilename, &size);
  if(buf == NULL)
    return false;

  /* cache attributes in memory */
  levelinfo.attributes = (uint32_t **)malloc(sizeof(uint32_t *) * levelinfo.height + sizeof(uint32_t) * levelinfo.width * levelinfo.height);
  if(levelinfo.attributes == NULL)
  {
    perror("malloc");
    return false;
  }
  uint32_t *atrptr = (uint32_t *)(levelinfo.attributes + levelinfo.height);
  for(int y = 0; y < levelinfo.height; y++) {
    levelinfo.attributes[y] = (atrptr + levelinfo.width * y);

    for(int x = 0; x < levelinfo.width; x++) {
      levelinfo.attributes[y][x] = (buf[ENDOFATRHEADER + y * levelinfo.width * 4 + x * 4]) |
        (buf[ENDOFATRHEADER + y * levelinfo.width * 4 + x * 4 + 1] << 8) |
        (buf[ENDOFATRHEADER + y * levelinfo.width * 4 + x * 4 + 2] << 16) |
        (buf[ENDOFATRHEADER + y * levelinfo.width * 4 + x * 4 + 3] << 24);
    }
  }

  /* cleanup */
  free(buf);

  return true;
}

bool loadtileset(char *filename)
{
  unsigned char *buf = loadfile1(filename);
  if(buf == NULL)
    return false;

  /* parse file header */
  tilesetinfo.size = buf[ENDOFTILHEADER - 3];
  tilesetinfo.count = buf[ENDOFTILHEADER - 2] | (buf[ENDOFTILHEADER - 1] << 8);

  /* generate palette */
  SDL_Palette* pal = SDL_AllocPalette(256);
  if(pal == NULL)
  {
    perror("SDL_AllocPalette");
    return false;
  }

  SDL_Color col[255];

  /* set transparency (cyan) */
  col[0].r = 0;
  col[0].g = 255;
  col[0].b = 255;
  col[0].a = 0;

  /* set rest of palette */
  for(int i = 1; i < 256; i++)
  {
    int palpos = ENDOFTILHEADER + tilesetinfo.size * tilesetinfo.size * tilesetinfo.count + i * 3;

    col[i].r = (buf[palpos] << 2);
    col[i].g = (buf[palpos + 1] << 2);
    col[i].b = (buf[palpos + 2] << 2);
    col[i].a = 255;
  }
  SDL_SetPaletteColors(pal, col, 0, 256);

  /* cache tileset in memory */
  SDL_Surface *temp = SDL_CreateRGBSurface(0, tilesetinfo.size * 40, tilesetinfo.size * tilesetinfo.count / 40, 8, 0, 0, 0, 0);
  if(temp == NULL)
  {
    printf("Surface could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  /* copy tiles */
  for(int i = 0; i < 75; i++) /* tiles in column */
  {
    for(int j = 0; j < 40; j++) /* tiles in row */
    {
      unsigned char *p = (unsigned char *)buf + ENDOFTILHEADER + ((i * 40 + j)
        * (tilesetinfo.size * tilesetinfo.size));
      SDL_Surface *s = SDL_CreateRGBSurfaceFrom(p, tilesetinfo.size,
        tilesetinfo.size, 8, tilesetinfo.size, 0, 0, 0, 0);
      if(s == NULL)
      {
        printf("Surface could not be created from tile %d! SDL_Error: %s\n",
          (i * 40 + j), SDL_GetError());
        return false;
      }

      SDL_Rect d;
      d.x = j * tilesetinfo.size;
      d.y = i * tilesetinfo.size;
      d.w = tilesetinfo.size;
      d.h = tilesetinfo.size;

      /* copy to big surface and cleanup */
      SDL_BlitSurface(s, NULL, temp, &d);
      SDL_FreeSurface(s);
    }
  }
  /* set palette and transparency */
  SDL_SetSurfacePalette(temp, pal);
  SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 255, 255));

  tiletex = SDL_CreateTextureFromSurface(renderer, temp);
  if(tiletex == NULL)
  {
    printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  /* cleanup */
  SDL_FreeSurface(temp);
  SDL_FreePalette(pal);
  free(buf);

  return true;
}

void cliplevelpos() {
  if(viewerstate.pos.y < -levelinfo.height)
    viewerstate.pos.y = -levelinfo.height;

  if(viewerstate.pos.y * 8 > viewerstate.height / viewerstate.zoom)
    viewerstate.pos.y = (viewerstate.height / viewerstate.zoom) / 8;

  if(viewerstate.pos.x < -levelinfo.width)
    viewerstate.pos.x = -levelinfo.width;

  if(viewerstate.pos.x * 8 > viewerstate.width / viewerstate.zoom)
    viewerstate.pos.x = (viewerstate.width / viewerstate.zoom) / 8;
}

void scroll_x(bool isleft, bool with_shift)
{
  int amount = (with_shift ? 8 : 4);
  viewerstate.pos.x += (isleft ? -amount : amount) / viewerstate.zoom;
  viewerstate.needsupdate = 1;
}

void scroll_y(bool isup, bool with_shift)
{
  int amount = (with_shift ? 8 : 4);
  viewerstate.pos.y += (isup ? -amount : amount) / viewerstate.zoom;
  viewerstate.needsupdate = 1;
}

void zoom_in()
{
  if(viewerstate.zoom < 8)
  {
    viewerstate.zoom++;
    viewerstate.needsupdate = 1;
  }
}

void zoom_out()
{
  if(viewerstate.zoom > 1)
  {
    viewerstate.zoom--;
    viewerstate.needsupdate = 1;
  }
}

void next_overlay()
{
  if(viewerstate.overlay < 30)
  {
    viewerstate.overlay++;
    viewerstate.needsupdate = 1;
  }
}

void prev_overlay()
{
  if(viewerstate.overlay > -1)
  {
    viewerstate.overlay--;
    viewerstate.needsupdate = 1;
  }
}

int screen2tilepos(int v) {
  return (v / viewerstate.zoom) / 8;
}

void checkevents()
{
  SDL_Event e;

  while(SDL_PollEvent(&e) != 0)
  {
    if(e.type == SDL_QUIT)
    {
      viewerstate.running = 0;
    }
    else if(e.type == SDL_KEYDOWN)
    {
      bool shift_pressed = e.key.keysym.mod & KMOD_SHIFT;
      switch(e.key.keysym.sym)
      {
        case SDLK_UP:
          scroll_y(1, shift_pressed);
          break;
        case SDLK_DOWN:
          scroll_y(0, shift_pressed);
          break;
        case SDLK_LEFT:
          scroll_x(1, shift_pressed);
          break;
        case SDLK_RIGHT:
          scroll_x(0, shift_pressed);
          break;
        case SDLK_PLUS:
          zoom_in();
          break;
        case SDLK_MINUS:
          zoom_out();
          break;
        case SDLK_PAGEUP:
          next_overlay();
          break;
        case SDLK_PAGEDOWN:
          prev_overlay();
          break;
        case SDLK_F5:
          /* toggle transparency effect */
          viewerstate.checkerboard ^= 1;
          viewerstate.needsupdate = 1;
          break;
        case SDLK_F6:
          /* reset window */
          SDL_SetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
          viewerstate.width = WINDOW_WIDTH;
          viewerstate.height = WINDOW_HEIGHT;
          viewerstate.needsupdate = 1;
          break;
        case SDLK_q:
        case SDLK_ESCAPE:
          /* close */
          viewerstate.running = 0;
          break;

        default:
          break;
      }
    }
    else if(e.type == SDL_MOUSEWHEEL)
    {
      if(e.wheel.y < 0)
      {
        zoom_out();
      }
      else if(e.wheel.y > 0)
      {
        zoom_in();
      }
    }
    else if(e.type == SDL_MOUSEBUTTONDOWN)
    {
      if (!viewerstate.dragging && e.button.button == SDL_BUTTON_LEFT)
      {
        SDL_SetCursor(hand_cursor);
        viewerstate.dragging = true;
        viewerstate.dragoffset = viewerstate.mouseoffset;
        viewerstate.dragstart = viewerstate.pos;
      }
    }
    else if(e.type == SDL_MOUSEBUTTONUP)
    {
      if (viewerstate.dragging && e.button.button == SDL_BUTTON_LEFT)
      {
        SDL_SetCursor(cursor);
        viewerstate.dragging = false;
      }
    }
    else if(e.type == SDL_MOUSEMOTION)
    {
      viewerstate.mouseoffset.x = screen2tilepos(e.motion.x);
      viewerstate.mouseoffset.y = screen2tilepos(e.motion.y);

      if (viewerstate.dragging)
      {
        viewerstate.pos.x = viewerstate.dragstart.x - (viewerstate.dragoffset.x - viewerstate.mouseoffset.x);
        viewerstate.pos.y = viewerstate.dragstart.y - (viewerstate.dragoffset.y - viewerstate.mouseoffset.y);
        viewerstate.needsupdate = 1;
      }
    }
    else if(e.type == SDL_WINDOWEVENT)
    {
      switch (e.window.event) {
        case SDL_WINDOWEVENT_RESIZED:
          viewerstate.width = e.window.data1;
          viewerstate.height = e.window.data2;
          /* FALLTHRU */
        case SDL_WINDOWEVENT_MOVED:
        case SDL_WINDOWEVENT_EXPOSED:
          viewerstate.needsupdate = 1;
          break;

        default:
          break;
      }
    }
  }
}

void plotlevel()
{
  /* nothing to do? */
  if(!viewerstate.needsupdate)
    return;

  viewerstate.needsupdate = false;

  int xpos, ypos, line, column;
  SDL_Rect src, dest;

  /* start with empty black image */
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  /* draw checkerboard */
  if(viewerstate.checkerboard)
  {
    int w = 0, h = 0;
    SDL_QueryTexture(checkerboardtex, NULL, NULL, &w, &h);
    for(int x = 0; x < viewerstate.width; x+=w)
    {
      for(int y = BARSIZE; y < viewerstate.height; y+=w)
      {
        SDL_Rect d = { x, y, w, h };
        SDL_RenderCopy(renderer, checkerboardtex, NULL, &d);
      }
    }
  }

  for(int x = 0; x < levelinfo.width; x++)
  {
    for(int y = 0; y < levelinfo.height; y++)
    {
      xpos = (x * 8 + viewerstate.pos.x * 8) * viewerstate.zoom;
      ypos = BARSIZE + (y * 8 + viewerstate.pos.y * 8) * viewerstate.zoom;

      /* make sure block is in window bounds */
      if(xpos < 0 || xpos > viewerstate.width ||
        ypos < BARSIZE || ypos > viewerstate.height)
        continue;

      /* first: draw static objects */
      uint16_t bgblock = levelinfo.background[y][x];

      /* skip empty blocks */
      if(bgblock != 0) {

        /* magic to get correct tile */
        line = bgblock / 40;
        column = bgblock - line * 40;

        src.x = column * tilesetinfo.size;
        src.y = line * tilesetinfo.size;
        src.w = tilesetinfo.size;
        src.h = tilesetinfo.size;

        dest.x = xpos;
        dest.y = ypos;
        dest.w = viewerstate.zoom * tilesetinfo.size;
        dest.h = viewerstate.zoom * tilesetinfo.size;

        /* copy block to window */
        SDL_RenderCopy(renderer, tiletex, &src, &dest);
      }

      /* second: draw overlay */
      uint32_t olblock = levelinfo.attributes[y][x];

      /* -1 is special, denotes all blocks without special handling */
      if(viewerstate.overlay == -1 && olblock == 0)
      {
        SDL_Rect src, dest;

        src.x = 0;
        src.y = 8;
        src.w = tilesetinfo.size;
        src.h = tilesetinfo.size;

        dest.x = xpos;
        dest.y = ypos;
        dest.w = viewerstate.zoom * tilesetinfo.size;
        dest.h = viewerstate.zoom * tilesetinfo.size;

        /* copy block to window */
        SDL_RenderCopy(renderer, overlaytex, &src, &dest);
      }
      else
      {
        float flag = powf(2, viewerstate.overlay);

        /* skip other blocks */
        if(olblock & (int)flag)
        {
          SDL_Rect src, dest;

          src.x = 0;
          src.y = (viewerstate.overlay > 8 ? 16: 0);
          src.w = tilesetinfo.size;
          src.h = tilesetinfo.size;

          dest.x = xpos;
          dest.y = ypos;
          dest.w = viewerstate.zoom * tilesetinfo.size;
          dest.h = viewerstate.zoom * tilesetinfo.size;

          /* copy block to window */
          SDL_RenderCopy(renderer, overlaytex, &src, &dest);
        }
      }
    }
  }

  /* update title bar */
  char title[100];
  sprintf(title, "level: %s, %dx%d blocks | window: %dx%d | pos: %dx%d | zoom: %dx | overlay: %d(%d)",
    levelinfo.name, levelinfo.width, levelinfo.height,
    viewerstate.width, viewerstate.height,
    viewerstate.pos.x, viewerstate.pos.y,
    viewerstate.zoom, viewerstate.overlay,
    (int)pow(2, viewerstate.overlay));

  SDL_Surface *text = TTF_RenderText_Solid(font, title, white);
  SDL_Texture *texttex = SDL_CreateTextureFromSurface(renderer, text);

  /* set text position and zoom */
  SDL_Rect textrect;
  textrect.x = 6;
  textrect.y = 4;
  textrect.w = text->w * 2;
  textrect.h = text->h * 2;

  /* draw and cleanup */
  SDL_RenderCopy(renderer, texttex, NULL, &textrect);
  SDL_FreeSurface(text);
  SDL_DestroyTexture(texttex);

  /* draw gray bottom line */
  SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
  SDL_RenderDrawLine(renderer, 0, BARSIZE - 1, viewerstate.width, BARSIZE -1);

  /* finally, update display */
  SDL_RenderPresent(renderer);
}

/* main function */
int main(int argc, char *argv[])
{
  /* variables */
  char *levelfile = argv[1];
  char *tilesetfile, *attributesfile;

  if(argc == 1 || argc > 2)
  {
    printf("%s\n--------------------------------------------------------------",
      WINDOW_TITLE);
    printf("\nusage: levelviewer era0XX.sme\n\n");
    printf("You need a corresponding .atr and .til file in the same folder\n");
    exit(1);
  }

  /* set additional files to load */
  attributesfile = (char *)malloc(sizeof(char) * strlen(levelfile) + 1);
  if(attributesfile == NULL)
  {
    perror("malloc");
    exit(1);
  }

  tilesetfile = (char *)malloc(sizeof(char) * strlen(levelfile) + 1);
  if(tilesetfile == NULL)
  {
    perror("malloc");
    exit(1);
  }
  strcpy(attributesfile, levelfile);
  strcpy(tilesetfile, levelfile);

  changefileext(attributesfile, ".atr");
  changefileext(tilesetfile, ".til");

  /* get filename without extension */
  extractfilename(levelinfo.name, levelfile);

  /* setup window */
  if(!init())
    exit(1);

  /* load data */
  if(!loadlevel(levelfile, attributesfile))
    exit(1);

  /* load gfx */
  if(!loadtileset(tilesetfile))
    exit(1);

  /* main loop */
  while(viewerstate.running)
  {
    /* check for key presses and other events */
    checkevents();

    /* clip position after scrolling */
    cliplevelpos();

    /* update display */
    plotlevel();

    /* take a breath */
    SDL_Delay(1);
  }

  /* deinit */
  free(attributesfile);
  free(tilesetfile);

  exit(0);
}
