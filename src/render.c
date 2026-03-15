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
void render_npcs();
void render_end_credits();

void render_game() {
  BeginDrawing();

  ClearBackground(BLACK);

  BeginMode2D(manager_get_global()->camera);

  render_tiles();
  render_bullets();
  render_enemies();
  render_player();
  render_money();
  render_npcs();
  render_end_credits();

  EndMode2D();

  render_dialog();
  render_ui();

  EndDrawing();
}

void render_tiles() {
  tilemap_draw();
  struct manager* mgr = manager_get_global();
  if (mgr->current_level == 10) {
    DrawTextureV(mgr->goolabfloor_tex, (Vector2){17 * TILE_SIZE, 10 * TILE_SIZE}, WHITE);
    DrawTextureV(mgr->goo_tex, (Vector2){20 * TILE_SIZE, 9 * TILE_SIZE}, WHITE);
  }
}

void render_bullets() {
  struct manager* mgr = manager_get_global();
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (mgr->bullets[i].active) {
      Vector2 p = {mgr->bullets[i].pos.x - mgr->bullet_tex.width / 2.0f,
                   mgr->bullets[i].pos.y - mgr->bullet_tex.height / 2.0f};
      DrawTextureV(mgr->bullet_tex, p, WHITE);
    }
  }
}

void render_player() {
  const struct manager* mgr = manager_get_global();
  float width = (float)mgr->player_tex.width;
  float height = (float)mgr->player_tex.height;

  Rectangle source = {0.0f, 0.0f, width, height};
  if (!mgr->player->facing_right) source.width = -width;

  Rectangle dest = {(float)mgr->player->pos.x, (float)mgr->player->pos.y,
                    width * 2.0f, height * 2.0f};
  DrawTexturePro(mgr->player_tex, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void render_enemies() {
  struct manager* mgr = manager_get_global();
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (mgr->enemies[i].active) {
      Texture2D tex;
      if (mgr->enemies[i].type == 2 || mgr->enemies[i].type == 3) {
        tex = mgr->rat2_anim[mgr->enemies[i].frame % 2];
      } else {
        tex = mgr->rat_anim[mgr->enemies[i].frame % 7];
      }
      Rectangle source = {0.0f, 0.0f, (float)tex.width, (float)tex.height};

      float scale = 1.0f;
      if (mgr->enemies[i].type == 2) scale = 1.6f;
      if (mgr->enemies[i].type == 3) scale = 2.2f;

      float dim = 32.0f * scale;
      float offset = 16.0f * scale;

      // Dest rect centered on pos
      Rectangle dest = {(float)mgr->enemies[i].pos.x + 16,
                        (float)mgr->enemies[i].pos.y + 16, dim, dim};
      Vector2 origin = {offset, offset};
      DrawTexturePro(tex, source, dest, origin, mgr->enemies[i].rotation,
                     WHITE);
    }
  }

  if (mgr->current_level == 10) {
    Texture2D tex;
    if (mgr->boss_stage >= 4) {
      tex = mgr->boss_head_knocked_tex;
    } else {
      int idx = mgr->boss_stage - 1;
      if (idx < 0) idx = 0;
      if (idx > 2) idx = 2;
      tex = mgr->boss_tex[idx];
    }
    Rectangle source = {0.0f, 0.0f, (float)tex.width, (float)tex.height};
    Rectangle dest = {18 * TILE_SIZE, 9 * TILE_SIZE, 128, 128};
    Vector2 origin = {16, 16};
    DrawTexturePro(tex, source, dest, origin, 0, WHITE);

    char healthbar[32];
    snprintf(healthbar, sizeof(healthbar), "%i", mgr->boss_hp);
    DrawTextEx(mgr->font, healthbar, (Vector2){400, 0}, 64, 2, BLACK);
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

void render_npcs() {
  struct manager* mgr = manager_get_global();
  bool flip = !mgr->player->facing_right;

  // NPC 1 (Crazy Man) on Level 3
  if (mgr->current_level == 3) {
    Texture2D tex = mgr->npc_tex[0];
    float w = (float)tex.width;
    float h = (float)tex.height;
    Rectangle src = {0, 0, flip ? -w : w, h};
    Rectangle dst = {20 * TILE_SIZE, 8 * TILE_SIZE, w * 2.0f, h * 2.0f};
    DrawTexturePro(tex, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
  }
  // NPC 2 (Helpless Person) on Level 4
  if (mgr->current_level == 4) {
    Texture2D tex = mgr->npc_tex[1];
    float w = (float)tex.width;
    float h = (float)tex.height;
    Rectangle src = {0, 0, flip ? -w : w, h};
    Rectangle dst = {22 * TILE_SIZE, 8 * TILE_SIZE, w * 2.0f, h * 2.0f};
    DrawTexturePro(tex, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
  }
  // NPC 3 (Crazy Gal) on Level 6
  if (mgr->current_level == 6) {
    Texture2D tex = mgr->npc_tex[2];
    float w = (float)tex.width;
    float h = (float)tex.height;
    Rectangle src = {0, 0, flip ? -w : w, h};
    Rectangle dst = {20 * TILE_SIZE, 8 * TILE_SIZE, w * 2.0f, h * 2.0f};
    DrawTexturePro(tex, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
  }
  // NPC 4 (Crazy Guy) on Level 9
  if (mgr->current_level == 9) {
    Texture2D tex = mgr->npc_tex[3];
    float w = (float)tex.width;
    float h = (float)tex.height;
    Rectangle src = {0, 0, flip ? -w : w, h};
    Rectangle dst = {20 * TILE_SIZE, 8 * TILE_SIZE, w * 2.0f, h * 2.0f};
    DrawTexturePro(tex, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
  }
}

void render_end_credits() {
  struct manager* mgr = manager_get_global();
  if (mgr->current_level != 11) return;

  render_tiles();

  const char* text =
      "A game made by:\nDevansh Malhotra,\nAyan Bindal,\nand Colin Melican";
  Vector2 size = MeasureTextEx(mgr->font, text, 64, 2);
  Vector2 pos = {(GetScreenWidth() - size.x) / 2.0f,
                 (GetScreenHeight() - size.y) / 2.0f};
  DrawTextEx(mgr->font, text, pos, 64, 2, BLACK);
}

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
    // Money with money icon
    DrawTextureEx(mgr->moneyicon_tex, (Vector2){10, 12}, 0.0f, 1.5f, WHITE);
    char moneyStr[32];
    sprintf(moneyStr, "%d", mgr->money);
    DrawTextEx(mgr->font, moneyStr, (Vector2){38, 10}, 24, 2, YELLOW);

    // Weapon
    const char* weaponName = (mgr->active_weapon == 0) ? "Crowbar" : "Gun";
    DrawTextEx(mgr->font, TextFormat("Weapon: %s", weaponName),
               (Vector2){10, 40}, 24, 2, WHITE);

    // Hearts (lives)
    render_hearts();
  }
}

void render_hearts() {
  struct manager* mgr = manager_get_global();
  for (int i = 0; i < 3; i++) {
    Vector2 pos = {850 + i * 30, 10};
    int half_hearts_for_this = mgr->lives - i * 2;
    if (half_hearts_for_this >= 2) {
      DrawTextureV(mgr->heart_tex, pos, WHITE);
    } else if (half_hearts_for_this == 1) {
      DrawTextureV(mgr->halfheart_tex, pos, WHITE);
    } else {
      DrawTextureV(mgr->heart_tex, pos, (Color){100, 100, 100, 100});
    }
  }
}
