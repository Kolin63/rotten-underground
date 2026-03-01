#include "enemy.h"

#include <math.h>
#include <raymath.h>
#include <stdlib.h>

#include "manager.h"
#include "tile.h"

void enemies_init(struct enemy enemies[MAX_ENEMIES]) {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    enemies[i].active = false;
    enemies[i].anim_timer = 0;
    enemies[i].frame = 0;
    enemies[i].rotation = 0;
  }
}

void enemies_spawn(struct enemy enemies[MAX_ENEMIES],
                   const struct rat_spawn rat_spawns[32]) {
  int enemy_idx = 0;
  for (int i = 0; i < 32 && enemy_idx < MAX_ENEMIES; i++) {
    if (rat_spawns[i].rat_type == 0) continue;
    enemies[enemy_idx].pos.x = (float)(rat_spawns[i].pos.x * TILE_SIZE);
    enemies[enemy_idx].pos.y = (float)(rat_spawns[i].pos.y * TILE_SIZE);
    enemies[enemy_idx].active = true;
    enemies[enemy_idx].anim_timer = 0;
    enemies[enemy_idx].frame = i % 7;
    enemies[enemy_idx].rotation = 0;
    enemy_idx++;
  }
}

void enemies_update(struct enemy enemies[MAX_ENEMIES], Vector2 playerPos,
                    float dt, bool paused) {
  if (paused || manager_get_global()->current_level >= 11) return;

  float speed = 60.0f;

  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) continue;

    // Animation update
    enemies[i].anim_timer += dt;
    if (enemies[i].anim_timer >= 0.1f) {
      enemies[i].anim_timer = 0;
      enemies[i].frame = (enemies[i].frame + 1) % 7;
    }

    // AI: Move towards player
    Vector2 enemyPos = {enemies[i].pos.x, enemies[i].pos.y};
    Vector2 dir = Vector2Subtract(playerPos, enemyPos);
    float dist = Vector2Length(dir);

    // Simple push to prevent overlapping or just direct move
    if (dist > 1.0f) {
      Vector2 norm = Vector2Normalize(dir);
      enemies[i].pos.x += norm.x * speed * dt;
      enemies[i].pos.y += norm.y * speed * dt;

      // Update rotation to face player
      // Offset by 180 degrees because the PNG's face is on the left edge
      enemies[i].rotation = (atan2f(norm.y, norm.x) * RAD2DEG) + 180.0f;
    }
  }
}
