#include "controller.h"

#include <math.h>
#include <raylib.h>
#include <raymath.h>

#include "dialog.h"
#include "manager.h"
#include "player.h"
#include "tile.h"

#define PLAYER_SPEED 3.5f

static void handle_movement(struct player* p) {
  const bool up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
  const bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
  const bool down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
  const bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

  const float velocity_x = (right - left) * PLAYER_SPEED;
  const float velocity_y = (down - up) * PLAYER_SPEED;

  if (velocity_x != 0) {
    float next_x = p->pos.x + velocity_x;
    next_x = Clamp(next_x, 0, (MAP_WIDTH * TILE_SIZE) - 64);

    float check_x = (velocity_x > 0) ? (next_x + 60) : (next_x + 4);
    int t_x = (int)(check_x / TILE_SIZE);
    int t_y1 = (int)((p->pos.y + 4) / TILE_SIZE);
    int t_ym = (int)((p->pos.y + 31) / TILE_SIZE);
    int t_y2 = (int)((p->pos.y + 59) / TILE_SIZE);

    if (isWalkable(t_x, t_y1) && isWalkable(t_x, t_ym) &&
        isWalkable(t_x, t_y2)) {
      p->pos.x = next_x;
    }

    if (velocity_x > 0)
      p->facing_right = true;
    else if (velocity_x < 0)
      p->facing_right = false;
  }

  if (velocity_y != 0) {
    float next_y = p->pos.y + velocity_y;
    next_y = Clamp(next_y, 0, (MAP_HEIGHT * TILE_SIZE) - 64);

    float check_y = (velocity_y > 0) ? (next_y + 60) : (next_y + 4);
    int t_y = (int)(check_y / TILE_SIZE);
    int t_x1 = (int)((p->pos.x + 4) / TILE_SIZE);
    int t_xm = (int)((p->pos.x + 31) / TILE_SIZE);
    int t_x2 = (int)((p->pos.x + 59) / TILE_SIZE);

    if (isWalkable(t_x1, t_y) && isWalkable(t_xm, t_y) &&
        isWalkable(t_x2, t_y)) {
      p->pos.y = next_y;
    }
  }
}

static void handle_attacks(struct manager* mgr, struct player* p) {
  Vector2 mousePos = GetMousePosition();
  Vector2 playerCenter = (Vector2){p->pos.x + 32, p->pos.y + 32};

  // Money Pickup Check
  for (int i = 0; i < MAX_MONEY; i++) {
    if (!mgr->money_items[i].active) continue;
    if (Vector2Distance(playerCenter, mgr->money_items[i].pos) < 32.0f) {
      mgr->money += 10;
      mgr->money_items[i].active = false;
      return;
    }
  }

  if (mgr->active_weapon == 0) {  // Crowbar
    if (mgr->current_level == 4 && !mgr->has_gun) {
      // Simple pickup logic: if player near gun
      if (Vector2Distance(playerCenter, (Vector2){500, 500}) < 50) {
        mgr->has_gun = true;
        static const char* gun[] = {"Ooh, a gun! This might be useful...",
                                    "Right mouse button to swap weapons."};
        dialog_show(mgr->dialog, "Johnny", gun, 2);
      }
    }

    if (mgr->crowbar_cooldown <= 0) {
      mgr->crowbar_cooldown = 1.0f;
      
      if (mgr->current_level == 10) {
        if (Vector2Distance(playerCenter,
                            (struct Vector2){18 * TILE_SIZE, 9 * TILE_SIZE}) < 100) {
          mgr->boss_hp -= 50;
        }
      }
      
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!mgr->enemies[i].active) continue;
        Vector2 enemyPos = (Vector2){mgr->enemies[i].pos.x + 8,
                                     mgr->enemies[i].pos.y + 8};
        if (Vector2Distance(playerCenter, enemyPos) < 64.0f) {
          mgr->enemies[i].health -= 50;
          if (mgr->enemies[i].health <= 0) {
            mgr->enemies[i].active = false;
          }
        }
      }
    }
  } else if (mgr->active_weapon == 1) {  // Gun
    if (mgr->gun_cooldown <= 0) {
      float dx = mousePos.x - playerCenter.x;
      float dy = mousePos.y - playerCenter.y;
      float len = (float)sqrt(dx * dx + dy * dy);

      if (len > 0) {
        if (mgr->ammo > 0) {
          mgr->ammo--;
          mgr->gun_cooldown = 0.15f;
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
}

void controller_tick() {
  struct manager* mgr = manager_get_global();

  // Cooldown counters
  float dt = GetFrameTime();
  if (mgr->crowbar_cooldown > 0) mgr->crowbar_cooldown -= dt;
  if (mgr->gun_cooldown > 0) mgr->gun_cooldown -= dt;
  if (mgr->player_invincibility_timer > 0)
    mgr->player_invincibility_timer -= dt;

  if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()) {
    mgr->game_should_run = false;
    return;
  }

  // Block manual input during Level 0 (cinematic) or if dialog is active
  if (mgr->current_level == 0 || mgr->dialog->active) {
    if (mgr->dialog->active && IsKeyPressed(KEY_SPACE)) {
      dialog_advance(mgr->dialog);
    }
    return;
  }

  // Shop Toggle
  if (mgr->current_level > 0 && !mgr->dialog->active && mgr->current_level != 11 && mgr->current_level != 12) {
    if (IsKeyPressed(KEY_TAB)) {
      mgr->shop_active = !mgr->shop_active;
    }
  }

  if (mgr->shop_active) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      Vector2 mp = GetMousePosition();
      if (mgr->has_gun) {
        // Ammo Button: {320, 230, 320, 60}
        if (CheckCollisionPointRec(mp, (Rectangle){320, 230, 320, 60})) {
          if (mgr->money >= 10) {
            mgr->money -= 10;
            mgr->ammo += 10;
          }
        }
        // Health Button: {320, 310, 320, 60}
        if (CheckCollisionPointRec(mp, (Rectangle){320, 310, 320, 60})) {
          if (mgr->lives < 6 && mgr->money >= 20) {
            mgr->money -= 20;
            mgr->lives += 2;
            if (mgr->lives > 6) mgr->lives = 6;
          }
        }
      } else {
        // Health Button: {320, 230, 320, 60}
        if (CheckCollisionPointRec(mp, (Rectangle){320, 230, 320, 60})) {
          if (mgr->lives < 6 && mgr->money >= 20) {
            mgr->money -= 20;
            mgr->lives += 2;
            if (mgr->lives > 6) mgr->lives = 6;
          }
        }
      }
      // Close Button: {400, 410, 160, 50}
      if (CheckCollisionPointRec(mp, (Rectangle){400, 410, 160, 50})) {
        mgr->shop_active = false;
      }
    }
    return; // Block movement/attacks while shop is open
  }

  struct player* p = mgr->player;

  // Handle Movement
  handle_movement(p);

  // Weapon swap (Right Click)
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && mgr->has_gun) {
    mgr->active_weapon = (mgr->active_weapon == 0) ? 1 : 0;
  }

  // Left Click: money pickup -> crowbar attack -> gun fire
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    handle_attacks(mgr, p);
  }
}
