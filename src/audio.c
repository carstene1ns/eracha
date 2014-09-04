
#include <SDL_mixer.h>
#include "audio.h"

int InitAudio() {
  /* first try modplug */
  int flags = MIX_INIT_MODPLUG;
  if((Mix_Init(flags) & flags) != flags) {
    /* second try mikmod */
    flags = MIX_INIT_MOD;
    if((Mix_Init(flags) & flags) != flags)
      return 0;
  }

  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
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
