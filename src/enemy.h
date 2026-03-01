#ifndef CAMPFIRE_ENEMY_H
#define CAMPFIRE_ENEMY_H

#include <raylib.h>

#include "pos.h"
#include "tile.h"

#define MAX_ENEMIES 10
#define ENEMY_RADIUS 8.0f

struct enemy {
  struct pos_float pos;
  bool active;
  float anim_timer;
  int frame;
  float rotation;
};

void enemies_init(struct enemy enemies[MAX_ENEMIES]);
void enemies_spawn(struct enemy enemies[MAX_ENEMIES], const struct tilemap* tm);
void enemies_update(struct enemy enemies[MAX_ENEMIES], Vector2 playerPos,
                    float dt, bool paused);

#endif
