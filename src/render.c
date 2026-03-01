#include "render.h"

#include <raylib.h>

#include "manager.h"
#include "tile.h"

void render_tiles();
void render_player();
void render_bullets();
void render_enemies();
void render_dialog();

void render_game() {
  BeginDrawing();

  ClearBackground(BLACK);

  render_tiles();
  render_bullets();
  render_enemies();
  render_player();
  render_dialog();

  EndDrawing();
}

void render_tiles() { tilemap_draw(manager_get_global()->tilemap); }

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

void render_dialog() { dialog_draw(manager_get_global()->dialog); }
