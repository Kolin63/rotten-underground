#ifndef CAMPFIRE_MANAGER_H
#define CAMPFIRE_MANAGER_H

#include "bullet.h"
#include "dialog.h"
#include "enemy.h"
#include "player.h"

struct manager {
  bool game_should_run;
  struct player* player;
  struct dialog_box* dialog;
  Font font;
  Camera2D camera;
  Texture2D player_tex;
  Texture2D rat_anim[7];
  Texture2D gravel_tex;
  Texture2D track_left_tex;
  Texture2D track_middle_tex;
  Texture2D track_right_tex;
  Texture2D platform_tex;
  Sound death_snd;
  struct bullet bullets[MAX_BULLETS];
  struct enemy enemies[MAX_ENEMIES];
};

void manager_init();
void manager_update();
void manager_draw();
void manager_cleanup();

void manager_run_game();

struct manager* manager_get_global();

#endif
