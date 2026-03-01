#include <raylib.h>
#include <stdio.h>

#include "dialog.h"
#include "manager.h"
#include "player.h"
#include "pos.h"
#include "tile.h"

void render_tiles();
void render_player();
void render_bullets();
void render_enemies();
void render_money();
void render_dialog();
void render_ui();
void render_hearts();

void render_game() {
  BeginDrawing();

  ClearBackground(BLACK);

  BeginMode2D(manager_get_global()->camera);

  render_tiles();
  render_bullets();
  render_enemies();
  render_player();
  render_money();

  EndMode2D();

  render_dialog();
  render_ui();

  EndDrawing();
}

void render_tiles() { tilemap_draw(); }

void render_bullets() {
  struct manager* mgr = manager_get_global();
  for (int i = 0; i < MAX_BULLETS; i++) {
    bullet_draw(&mgr->bullets[i]);
  }
}

void render_player() {
  const struct manager* mgr = manager_get_global();
  DrawTexture(mgr->player_tex, (int)mgr->player->pos.x, (int)mgr->player->pos.y,
              WHITE);
}

void render_enemies() {
  struct manager* mgr = manager_get_global();
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (mgr->enemies[i].active) {
      Texture2D tex = mgr->rat_anim[mgr->enemies[i].frame];
      Rectangle source = {0.0f, 0.0f, (float)tex.width, (float)tex.height};
      // Dest rect centered on pos
      Rectangle dest = {(float)mgr->enemies[i].pos.x + 16,
                        (float)mgr->enemies[i].pos.y + 16, 32, 32};
      Vector2 origin = {16, 16};
      DrawTexturePro(tex, source, dest, origin, mgr->enemies[i].rotation,
                     WHITE);
    }
  }
}

void render_money() {
  struct manager* mgr = manager_get_global();
  for (int i = 0; i < MAX_MONEY; i++) {
    if (mgr->money_items[i].active) {
      DrawTextureV(mgr->coin_tex, mgr->money_items[i].pos, WHITE);
    }
  }
}

void render_dialog() { dialog_draw(manager_get_global()->dialog); }

void render_ui() {
  struct manager* mgr = manager_get_global();

  // Title for Level 0
  if (mgr->current_level == 0 && mgr->intro_step >= 4) {
    Color col = WHITE;
    col.a = (unsigned char)(mgr->title_alpha * 255);
    const char* title = "Rotten Underground";
    Vector2 size = MeasureTextEx(mgr->font, title, 64, 2);
    Vector2 pos = {(GetScreenWidth() - size.x) / 2.0f,
                   (GetScreenHeight() - size.y) / 2.0f};
    DrawTextEx(mgr->font, title, pos, 64, 2, col);
  }

  // HUD (Coins, Health, Weapon)
  if (mgr->current_level > 0) {
    // Money
    char moneyStr[32];
    sprintf(moneyStr, "Money: $%d", mgr->money);
    DrawTextEx(mgr->font, moneyStr, (Vector2){10, 10}, 24, 2, YELLOW);

    // Weapon
    const char* weaponName = (mgr->active_weapon == 0) ? "Crowbar" : "Gun";
    DrawTextEx(mgr->font, TextFormat("Weapon: %s", weaponName),
               (Vector2){10, 40}, 24, 2, WHITE);

    // Hearts (lives)
    render_hearts();
  }

  // Tutorial prompts
  if (mgr->current_level == 1) {
    DrawTextEx(mgr->font, "WASD to Move", (Vector2){400, 580}, 32, 2, BLACK);
  } else if (mgr->current_level == 2) {
    DrawTextEx(mgr->font, "LMB: Crowbar Attack", (Vector2){350, 580}, 32, 2,
               BLACK);
  } else if (mgr->current_level == 4 && mgr->has_gun) {
    DrawTextEx(mgr->font, "RMB: Swap Weapons", (Vector2){360, 580}, 32, 2,
               BLACK);
  }
}

void render_hearts() {
  struct manager* mgr = manager_get_global();
  for (int i = 0; i < 3; i++) {
    Vector2 pos = {820 + i * 30, 40};
    if (i < mgr->lives) {
      DrawTextureV(mgr->heart_tex, pos, WHITE);
    } else {
      DrawTextureV(mgr->heart_tex, pos, (Color){100, 100, 100, 100});
    }
  }
}
