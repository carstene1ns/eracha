
#ifndef GAME_H
#define GAME_H

#define GAME_NAME "eracha"
#define GAME_WIDTH 320
#define GAME_HEIGHT 200

enum gamestates {
  STATE_INTRO = 1,
  STATE_MENU,
  STATE_MENUOPTIONS,
  STATE_GAMEOPTIONS,
  STATE_MAP,
  STATE_LEVEL,
  STATE_COUNT
};

enum gamelevels {
  LEVEL_01 = 10,
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

enum gamearchives {
  ARCHIVE_BKGS = 20,
  ARCHIVE_MAPS,
  ARCHIVE_ATTR,
  ARCHIVE_ENEM,
  ARCHIVE_MUS1,
  ARCHIVE_MUS2,
  ARCHIVE_MUS3,
  ARCHIVE_SND1,
  ARCHIVE_SND2,
  ARCHIVE_SND3,
  ARCHIVE_COUNT
};

enum gamebosses {
  BOSS_01 = 40,
  BOSS_02,
  BOSS_03,
  BOSS_04,
  BOSS_COUNT
};

enum extramusic {
  MUSIC_MENU = 50,
  MUSIC_MAP,
  MUSIC_END,
  MUSIC_COUNT
};

/* globals */
extern int state;
extern int level;

#endif /* GAME_H */
