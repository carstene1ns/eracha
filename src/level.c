
#include <SDL.h>

#include "game.h"
#include "event.h"
#include "audio.h"
#include "video.h"
#include "level.h"

static int leave = 0;

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
  if(!LoadBackground(level))
    return 0;

  if(!LoadMusic(level))
    return 0;

  /* all ok */
  return 1;
}
