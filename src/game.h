
#ifndef GAME_H
#define GAME_H

#define GAME_NAME "eracha"
#define GAME_WIDTH 320
#define GAME_HEIGHT 200

enum gamestates
{
  STATE_INTRO = 1,
  STATE_MENU,
  STATE_MENUOPTIONS,
  STATE_GAMEOPTIONS,
  STATE_LEVEL,
  STATE_COUNT
};

enum gamelevels
{
  LEVEL_01 = 1,
  LEVEL_02,
  LEVEL_11,
  LEVEL_12,
  LEVEL_21,
  LEVEL_22,
  LEVEL_31,
  LEVEL_32,
  LEVEL_41,
  LEVEL_COUNT
};

#endif /* GAME_H */
