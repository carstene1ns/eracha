
/******************************************************************************
 * levelviewer.c ERACHA level viewer
 * written by carstene1ns, 2014
 *****************************************************************************/

#include <stdio.h>
#include <ctype.h>

#include <SDL.h>
#include <SDL_ttf.h>

/* the font */
#include "pixelfont.h"

/* window parameters */
#define WINDOW_TITLE "ERACHA level viewer"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BARSIZE 24

/* file parameters */
#define ENDOFSMEHEADER 41
#define ENDOFATRHEADER 4
#define ENDOFTILHEADER 23

/* typedefs make our life with structs easier */
typedef struct
{
  int running, posx, posy, zoom, overlay, width, height, needsupdate;
} tviewerstate;

typedef struct
{
  int width, height;
  char name[13];
  unsigned char *background;
  unsigned char *attributes;
} tlevelinfo;

typedef struct
{
  int size, count;
} ttilesetinfo;

/* globals */
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *tiletex = NULL;
SDL_Texture *overlaytex = NULL;
TTF_Font *font = NULL;

SDL_Color white = { 0xFF, 0xFF, 0xFF, 0 };

tviewerstate viewerstate = { 1, 0, 0, 1, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 1 };
tlevelinfo levelinfo = { 0, 0, "", NULL, NULL };
ttilesetinfo tilesetinfo = { 0, 0 };

/* helper functions */
void changefileext(char *path, char *ext)
{
  /* find file name */
  char *pFile = strrchr(path, '/');
  pFile = pFile == NULL ? path : pFile + 1;

  /* change extension */
  char *pExt = strrchr(pFile, '.');
  if (pExt != NULL)
    strcpy(pExt, ext);
  else
    strcat(pFile, ext);
}

void extractfilename(char *name, char *path)
{
  /* find file name */
  char *pFile = strrchr(path, '/');
  pFile = pFile == NULL ? path : pFile + 1;

  /* remove extension */
  char *pExt = strrchr(pFile, '.');
  if (pExt != NULL)
    strncpy(name, pFile, pExt - pFile);
  else
    strcpy(name, pFile);
}

void cleanup()
{
  if (TTF_WasInit())
    TTF_Quit();

  SDL_Quit();
}

int init()
{
  /* init SDL library */
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }

  /* always do cleanup */
  atexit(cleanup);

  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if(window == NULL)
  {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }

  renderer = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(renderer == NULL)
  {
    printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }

  /* create some colors for our overlay */
  SDL_Surface *temp = SDL_CreateRGBSurface(0, 8, 3 * 8, 32, 0x000000ff,
    0x0000ff00, 0x00ff0000, 0xff000000);
  if(temp == NULL)
  {
    printf("Surface could not be created! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }

  for(int i = 0; i < 3; i++)
  {
    SDL_Rect d;
    d.x = 0;
    d.y = i * 8;
    d.w = 8;
    d.h = 8;

    /* copy to big surface */
    SDL_FillRect(temp, &d, SDL_MapRGBA(temp->format,
    (i == 0) ? 255 : 0, (i == 1) ? 255 : 0, (i == 2) ? 255 : 0, 100));
  }

  overlaytex = SDL_CreateTextureFromSurface(renderer, temp);
  if(overlaytex == NULL)
  {
    printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }
  SDL_FreeSurface(temp);

  /* init SDL2_TTF library */
  if(TTF_Init() == -1) {
    printf("Error while TTF_Init: %s\n", TTF_GetError());
    return 0;
  }

  /* load font */
#if 0
  font = TTF_OpenFont("font/04B_03__.TTF", 8);
#else
  font = TTF_OpenFontRW(SDL_RWFromMem(pixelfont, pixelfont_len), 1, 8);
#endif
  if(!font) {
    printf("Error while TTF_OpenFont: %s\n", TTF_GetError());
    return 0;
  }

  /* all good */
  return 1;
}

int loadlevel(char *levelfilename, char *attributesfilename)
{
  /* load level file */
  FILE *fp = fopen(levelfilename, "rb");
  if(fp == NULL)
  {
    perror("fopen");
    return 0;
  }

  fseek(fp, 0, SEEK_END);
  unsigned long size = ftell(fp);
  rewind(fp);

  unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char) * size);
  if(buf == NULL)
  {
    perror("malloc");
    return 0;
  }

  size_t res = fread(buf, 1, size, fp);
  if(res != size)
  {
    perror("fread");
    return 0;
  }

  /* parse file header */
  levelinfo.width = buf[ENDOFSMEHEADER - 8] | (buf[ENDOFSMEHEADER - 7] << 8);
  levelinfo.height = buf[ENDOFSMEHEADER - 4] | (buf[ENDOFSMEHEADER - 3] << 8);

  /* cache level in memory */
  levelinfo.background = (unsigned char *)malloc(sizeof(unsigned char) * (size - ENDOFSMEHEADER));
  if(levelinfo.background == NULL)
  {
    perror("malloc");
    return 0;
  }
  memcpy(levelinfo.background, buf + ENDOFSMEHEADER, sizeof(unsigned char) * (size - ENDOFSMEHEADER));

  /* cleanup */
  free(buf);
  fclose(fp);

  /* load attributes file */
  fp = fopen(attributesfilename, "rb");
  if(fp == NULL)
  {
    perror("fopen");
    return 0;
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);

  buf = (unsigned char *)malloc(sizeof(unsigned char) * size);
  if(buf == NULL)
  {
    perror("malloc");
    return 0;
  }

  res = fread(buf, 1, size, fp);
  if(res != size)
  {
    perror("fread");
    return 0;
  }

  /* cache attributes in memory */
  levelinfo.attributes = (unsigned char *)malloc(sizeof(unsigned char) * (size - ENDOFATRHEADER));
  if(levelinfo.attributes == NULL)
  {
    perror("malloc");
    return 0;
  }
  memcpy(levelinfo.attributes, buf + ENDOFATRHEADER, sizeof(unsigned char) * (size - ENDOFATRHEADER));

  /* cleanup */
  free(buf);
  fclose(fp);

  return 1;
}

int loadtileset(char *filename)
{
  FILE *fp = fopen(filename, "rb");
  if(fp == NULL)
  {
    perror("fopen");
    return 0;
  }

  fseek(fp, 0, SEEK_END);
  unsigned long size = ftell(fp);
  rewind(fp);

  unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char) * size);
  if(buf == NULL)
  {
    perror("malloc");
    return 0;
  }

  size_t res = fread(buf, 1, size, fp);
  if(res != size)
  {
    perror("fread");
    return 0;
  }

  /* parse file header */
  tilesetinfo.size = buf[ENDOFTILHEADER - 3];
  tilesetinfo.count = buf[ENDOFTILHEADER - 2] | (buf[ENDOFTILHEADER - 1] << 8);

  /* generate palette */
  SDL_Palette* pal = SDL_AllocPalette(256);
  if(pal == NULL)
  {
    perror("SDL_AllocPalette");
    return 0;
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
    return 0;
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
        return 0;
      }

      SDL_Rect d;
      d.x = j * tilesetinfo.size;
      d.y = i * tilesetinfo.size;
      d.w = tilesetinfo.size;
      d.h = tilesetinfo.size;

      /* copy to big surface */
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
    return 0;
  }

  /* cleanup */
  SDL_FreeSurface(temp);
  SDL_FreePalette(pal);
  free(buf);
  fclose(fp);

  return 1;
}

void zoom_in()
{
  if (viewerstate.zoom < 8)
  {
    viewerstate.zoom++;
    viewerstate.needsupdate = 1;
  }
}

void zoom_out()
{
  if (viewerstate.zoom > 1)
  {
    viewerstate.zoom--;
    viewerstate.needsupdate = 1;
  }
}

void next_overlay()
{
  if (viewerstate.overlay < 31)
  {
    viewerstate.overlay++;
    viewerstate.needsupdate = 1;
  }
}

void prev_overlay()
{
  if (viewerstate.overlay > -1)
  {
    viewerstate.overlay--;
    viewerstate.needsupdate = 1;
  }
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
      switch(e.key.keysym.sym)
      {
        case SDLK_UP:
          if(viewerstate.posy > -levelinfo.height)
          {
            viewerstate.posy -= 8 / viewerstate.zoom;
            viewerstate.needsupdate = 1;
          }
          break;
        case SDLK_DOWN:
          if(viewerstate.posy * 8 < viewerstate.height / viewerstate.zoom)
          {
            viewerstate.posy += 8 / viewerstate.zoom;
            viewerstate.needsupdate = 1;
          }
          break;
        case SDLK_LEFT:
          if(viewerstate.posx > -levelinfo.width)
          {
            viewerstate.posx -= 8 / viewerstate.zoom;
            viewerstate.needsupdate = 1;
          }
          break;
        case SDLK_RIGHT:
          if(viewerstate.posx * 8 < viewerstate.width / viewerstate.zoom)
          {
            viewerstate.posx += 8 / viewerstate.zoom;
            viewerstate.needsupdate = 1;
          }
          break;
        case SDLK_PLUS:
          /* zoom in */
          zoom_in();
          break;
        case SDLK_MINUS:
          /* zoom out */
          zoom_out();
          break;
        case SDLK_PAGEUP:
          next_overlay();
          break;
        case SDLK_PAGEDOWN:
          prev_overlay();
          break;
        case SDLK_F6:
          /* reset window */
          SDL_SetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
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
        /* zoom out */
        zoom_out();
      }
      else if(e.wheel.y > 0)
      {
        /* zoom in */
        zoom_in();
      }
    }
    else if(e.type == SDL_MOUSEMOTION)
    {
      if(e.motion.state & SDL_BUTTON_LMASK)
      {
        viewerstate.posx += (e.motion.xrel / viewerstate.zoom) / 8;
        viewerstate.posy += (e.motion.yrel / viewerstate.zoom) / 8;
        viewerstate.needsupdate = 1;
      }
    }
    else if(e.type == SDL_WINDOWEVENT)
    {
      switch (e.window.event) {
        case SDL_WINDOWEVENT_RESIZED:
          viewerstate.width = e.window.data1;
          viewerstate.height = e.window.data2;
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
  if(viewerstate.needsupdate)
    viewerstate.needsupdate = 0;
  else
    return;

  unsigned long block;
  int xpos, ypos, line, column;
  SDL_Rect src, dest;

  /* start with empty black image */
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for(int x = 0; x < levelinfo.width; x++)
  {
    for(int y = 0; y < levelinfo.height; y++)
    {
      xpos = (x * 8 + viewerstate.posx * 8) * viewerstate.zoom;
      ypos = BARSIZE + (y * 8 + viewerstate.posy * 8) * viewerstate.zoom;

      /* make sure block is in window bounds */
      if(xpos < 0 || xpos > viewerstate.width ||
        ypos < BARSIZE || ypos > viewerstate.height)
        continue;

      /* first: draw static objects */
      block = (levelinfo.background[y * levelinfo.width * 2 + x * 2]) |
              (levelinfo.background[y * levelinfo.width * 2 + x * 2 + 1] << 8);

      /* skip empty blocks */
      if(block != 0) {

        /* magic to get correct tile */
        line = block / 40;
        column = block - line * 40;

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
      block = (levelinfo.attributes[y * levelinfo.width * 4 + x * 4]) |
              (levelinfo.attributes[y * levelinfo.width * 4 + x * 4 + 1] << 8) |
              (levelinfo.attributes[y * levelinfo.width * 4 + x * 4 + 2] << 16) |
              (levelinfo.attributes[y * levelinfo.width * 4 + x * 4 + 3] << 24);

      /* -1 is special */
      if(viewerstate.overlay == -1)
      {
        if(block == 0)
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
      }
      else
      {
        double flag = pow(2, viewerstate.overlay);

        /* skip other blocks */
        if(block & (int)flag)
        {
          SDL_Rect src, dest;

          src.x = 0;
          src.y = 0;
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
  sprintf(title, "level: %s, %dx%d blocks | window: %dx%d | pos: %dx%d | zoom: %dx | overlay: %d",
    levelinfo.name, levelinfo.width, levelinfo.height,
    viewerstate.width, viewerstate.height,
    viewerstate.posx, viewerstate.posy,
    viewerstate.zoom, viewerstate.overlay);

  SDL_Surface *text = TTF_RenderText_Solid(font, title, white);
  SDL_Texture *texttex = SDL_CreateTextureFromSurface(renderer, text);
  SDL_FreeSurface(text);

  /* set text position and zoom */
  SDL_Rect textrect;
  textrect.x = 6;
  textrect.y = 4;
  textrect.w = text->w * 2;
  textrect.h = text->h * 2;
  SDL_RenderCopy(renderer, texttex, NULL, &textrect);

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
    return 0;
  }

  tilesetfile = (char *)malloc(sizeof(char) * strlen(levelfile) + 1);
  if(tilesetfile == NULL)
  {
    perror("malloc");
    return 0;
  }
  strcpy(attributesfile, levelfile);
  strcpy(tilesetfile, levelfile);

  changefileext(attributesfile, ".atr");
  changefileext(tilesetfile, ".til");

  /* get filename without extension */
  extractfilename(levelinfo.name, levelfile);

  /* setup window */
  if(!init())
  {
    exit(0);
  }

  /* load data */
  if(!loadlevel(levelfile, attributesfile))
  {
    exit(0);
  }

  /* load gfx */
  if(!loadtileset(tilesetfile))
  {
    exit(0);
  }

  /* main loop */
  while(viewerstate.running)
  {
    /* check for key presses and other events */
    checkevents();

    /* update display */
    plotlevel();

    /* take a breath */
    SDL_Delay(1);
  }

  /* deinit */
  free(attributesfile);
  free(tilesetfile);
  TTF_CloseFont(font);
  SDL_DestroyTexture(tiletex);
  SDL_DestroyWindow(window);

  exit(0);
}
