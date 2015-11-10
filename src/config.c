
#include <stdio.h>
#include <stdlib.h>

#include "minIni/dev/minIni.h"
#include "cfgpath/cfgpath.h"
#include "config.h"
#include "game.h"

tconfig config;

void DefaultConfig() {
  config.zoom = 3;
  config.music_volume = 9;
}
