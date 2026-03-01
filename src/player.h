#ifndef CAMPFIRE_PLAYER_H
#define CAMPFIRE_PLAYER_H

#include "pos.h"

struct player {
  struct pos_float pos;
  bool facing_right;
};

#endif
