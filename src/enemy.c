#include "enemy.h"

#include <math.h>
#include <raymath.h>
#include <stdlib.h>

#include "tile.h"

void enemies_init(struct enemy enemies[MAX_ENEMIES]) {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    enemies[i].active = false;
    enemies[i].anim_timer = 0;
    enemies[i].frame = 0;
    enemies[i].rotation = 0;
  }
}

void enemies_spawn(struct enemy enemies[MAX_ENEMIES]) {
  return;
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) {
      int attempts = 0;
      while (attempts < 100) {
        int tx = rand() % MAP_WIDTH;
        int ty = rand() % MAP_HEIGHT;

        if (tx == 4 && ty == 2) {
          attempts++;
          continue;
        }

        if (isWalkable(tx, ty)) {
          enemies[i].pos.x = (float)(tx * TILE_SIZE);
          enemies[i].pos.y = (float)(ty * TILE_SIZE);
          enemies[i].active = true;
          enemies[i].anim_timer = 0;
          enemies[i].frame = rand() % 7;  // Random start frame
          enemies[i].rotation = 0;
          break;
        }
        attempts++;
      }
    }
  }
}

void enemies_update(struct enemy enemies[MAX_ENEMIES], Vector2 playerPos,
                    float dt, bool paused) {
  if (paused) return;

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
