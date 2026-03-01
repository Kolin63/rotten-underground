#include "controller.h"

#include <math.h>
#include <raylib.h>
#include <raymath.h>

#include "dialog.h"
#include "manager.h"
#include "player.h"
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

  // Weapon swap (Right Click)
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && mgr->has_gun) {
    mgr->active_weapon = (mgr->active_weapon == 0) ? 1 : 0;
  }

  // Left Click: money pickup -> crowbar attack -> gun fire
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    Vector2 playerCenter = (Vector2){p->pos.x + 10, p->pos.y + 10};

    // Money Pickup Check
    for (int i = 0; i < MAX_MONEY; i++) {
      if (!mgr->money_items[i].active) continue;
      if (Vector2Distance(playerCenter, mgr->money_items[i].pos) < 32.0f) {
        mgr->money += 10;
        mgr->money_items[i].active = false;
        // "don't mind if I do..." voiceline
        return;
      }
    }

    if (mgr->active_weapon == 0) {  // Crowbar
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!mgr->enemies[i].active) continue;
        Vector2 enemyPos = {mgr->enemies[i].pos.x + 8,
                            mgr->enemies[i].pos.y + 8};
        if (Vector2Distance(playerCenter, enemyPos) < 64.0f) {
          mgr->enemies[i].active = false;
          PlaySound(mgr->death_snd);
        }
      }
    } else if (mgr->active_weapon == 1) {  // Gun
      float dx = mousePos.x - playerCenter.x;
      float dy = mousePos.y - playerCenter.y;
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
}
