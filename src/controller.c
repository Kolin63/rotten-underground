#include "controller.h"

#include <math.h>
#include <raylib.h>

#include "dialog.h"
#include "manager.h"
#include "tile.h"

#define PLAYER_SPEED 5.0f

void controller_tick() {
  struct manager* mgr = manager_get_global();

  if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()) {
    mgr->game_should_run = false;
    return;
  }

  if (mgr->dialog->active) {
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
      dialog_advance(mgr->dialog);
    }
    return;
  }

  struct player* p = mgr->player;

  const bool up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
  const bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
  const bool down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
  const bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

  const float velocity_x = (right - left) * PLAYER_SPEED;
  const float velocity_y = (down - up) * PLAYER_SPEED;

  p->pos.x += velocity_x;
  p->pos.y += velocity_y;

  // if (velocity_x != 0) {
  //   float next_x = p->pos.x + velocity_x;

  //   if (next_x < 0) next_x = 0;
  //   if (next_x > (MAP_WIDTH * TILE_SIZE) - 20)
  //     next_x = (MAP_WIDTH * TILE_SIZE) - 20;

  //   float check_x = (velocity_x > 0) ? (next_x + 20) : next_x;
  //   int t_x = (int)(check_x / TILE_SIZE);
  //   int t_y1 = (int)(p->pos.y / TILE_SIZE);
  //   int t_y2 = (int)((p->pos.y + 19) / TILE_SIZE);

  //   if (isWalkable(t_x, t_y1) && isWalkable(t_x, t_y2)) {
  //     p->pos.x = next_x;
  //   }
  // }

  // if (velocity_y != 0) {
  //   float next_y = p->pos.y + velocity_y;

  //   if (next_y < 0) next_y = 0;
  //   if (next_y > (MAP_HEIGHT * TILE_SIZE) - 20)
  //     next_y = (MAP_HEIGHT * TILE_SIZE) - 20;

  //   float check_y = (velocity_y > 0) ? (next_y + 20) : next_y;
  //   int t_y = (int)(check_y / TILE_SIZE);
  //   int t_x1 = (int)(p->pos.x / TILE_SIZE);
  //   int t_x2 = (int)((p->pos.x + 19) / TILE_SIZE);

  //   if (isWalkable(t_x1, t_y) && isWalkable(t_x2, t_y)) {
  //     p->pos.y = next_y;
  //   }
  // }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 worldMouse = GetScreenToWorld2D(GetMousePosition(), mgr->camera);
    Vector2 playerCenter = (Vector2){p->pos.x + 10, p->pos.y + 10};

    float dx = worldMouse.x - playerCenter.x;
    float dy = worldMouse.y - playerCenter.y;
    float len = (float)sqrt(dx * dx + dy * dy);

    if (len > 0) {
      Vector2 dir = (Vector2){dx / len, dy / len};
      for (int i = 0; i < MAX_BULLETS; i++) {
        if (!mgr->bullets[i].active) {
          bullet_fire(&mgr->bullets[i], playerCenter, dir);
          break;
        }
      }
    }
  }
}
