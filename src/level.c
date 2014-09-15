
#include <stdio.h>
#include <SDL.h>

#include "game.h"
#include "event.h"
#include "audio.h"
#include "video.h"
#include "level.h"
#include "file.h"

static int leave = 0;

tlevel level;

void LevelState() {
  /* Initialize level structures and files */
  if(!InitLevel()) {
    printf("Could not load level.\n");
    leave = 1;
  }

  while(!leave) {
    /* events */
    leave = EventLoop();

    /* status changes (collision/jump/item/...) */

    /* draw */
    DrawLevel();

    /* sounds */

    /* rest */
    SDL_Delay(20);
  }
  /* back to menu state */
  state = STATE_MENU;
}

int InitLevel() {
  level.map = LoadLevelF(level.number, &level.width, &level.height);
  if(level.width == 0 || level.height == 0)
    return 0;

  unsigned int atr_size;
  level.atr = LoadAttributesF(level.number, &atr_size);
  if(atr_size == 0)
    return 0;

  /* reset viewport */
  level.shift_x = 0;
  level.shift_y = 0;
  level.max_x = level.width * TILE_SIZE;
  level.max_y = level.height * TILE_SIZE;

  if(!LoadBackground(level.number))
    return 0;

  if(!LoadTileset(level.number))
    return 0;

  if(!LoadMusic(level.number))
    return 0;

  /* all ok */
  return 1;
}
