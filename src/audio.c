
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_mixer.h>

#include "audio.h"
#include "file.h"
#include "game.h"

static Mix_Music *music = NULL;

int InitAudio() {
  /* first try modplug */
  int flags = MIX_INIT_MODPLUG;
  if((Mix_Init(flags) & flags) != flags) {
    /* second try mikmod */
    flags = MIX_INIT_MOD;
    if((Mix_Init(flags) & flags) != flags)
      return 0;
  }

  if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
    return 0;

  Mix_AllocateChannels(16);

  atexit(DeinitAudio);

  /* all ok */
  return 1;
}

void DeinitAudio() {
  Mix_AllocateChannels(0);
  Mix_CloseAudio();
  Mix_Quit();
}

int LoadMusic(int track) {
  unsigned int size;
  char *buf = LoadMusicF(track, &size);
  music = Mix_LoadMUS_RW(SDL_RWFromConstMem(buf, size), 1);
  free(buf);
  if(!music) {
    printf("Mix Error: %s\n", Mix_GetError());
    return 0;
  }

  /* all ok */
  return 1;
}

void PlayMusic() {
  Mix_PlayMusic(music, -1);
  Mix_VolumeMusic(config.music_volume);
}

void MusicVolumeUp() {
  int oldvol = Mix_VolumeMusic(-1);
  if(oldvol < MIX_MAX_VOLUME - 8)
    Mix_VolumeMusic(oldvol + 8);
}

void MusicVolumeDown() {
  int oldvol = Mix_VolumeMusic(-1);
  if(oldvol > 8)
    Mix_VolumeMusic(oldvol - 8);
}

void EffectVolumeUp() {
  int oldvol = Mix_Volume(-1, -1);
  if(oldvol < MIX_MAX_VOLUME - 8)
    Mix_Volume(-1, oldvol + 8);
}

void EffectVolumeDown() {
  int oldvol = Mix_Volume(-1, -1);
  if(oldvol > 8)
    Mix_Volume(-1, oldvol - 8);
}
