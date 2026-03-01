#include "bullet.h"

#include <raylib.h>

#include "tile.h"

void bullet(struct bullet* b) { b->active = false; }

void bullet_fire(struct bullet* b, Vector2 pos, Vector2 direction) {
  b->pos = pos;
  b->velocity =
      (Vector2){direction.x * BULLET_SPEED, direction.y * BULLET_SPEED};
  b->active = true;
}

void bullet_update(struct bullet* b, float dt) {
  if (!b->active) return;

  b->pos.x += b->velocity.x * dt;
  b->pos.y += b->velocity.y * dt;

  if (b->pos.x < 0 || b->pos.x > tilemap_get_current()->width * TILE_SIZE ||
      b->pos.y < 0 || b->pos.y > tilemap_get_current()->height * TILE_SIZE) {
    b->active = false;
    return;
  }
}

void bullet_draw(const struct bullet* b) {
  if (!b->active) return;
  DrawCircleV(b->pos, BULLET_RADIUS, RED);
}
