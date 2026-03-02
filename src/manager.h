#ifndef CAMPFIRE_MANAGER_H
#define CAMPFIRE_MANAGER_H

#include <raylib.h>

#include "bullet.h"
#include "dialog.h"
#include "enemy.h"
#include "player.h"

#define MAX_MONEY 20
struct money_item {
  Vector2 pos;
  bool active;
};

struct manager {
  bool game_should_run;
  struct player* player;
  struct dialog_box* dialog;
  Font font;
  Camera2D camera;
  Texture2D player_tex;
  Texture2D rat_anim[7];
  Texture2D rat2_anim[2];
  Texture2D gravel_tex;
  Texture2D track_left_tex;
  Texture2D track_middle_tex;
  Texture2D track_right_tex;
  Texture2D platform_tex;
  Texture2D rubble_tex;
  Texture2D wall_tex;
  Texture2D labfloor_tex;
  Texture2D coin_tex;
  Texture2D moneyicon_tex;
  Texture2D heart_tex;
  Texture2D halfheart_tex;
  Texture2D boss_tex[3];
  Texture2D boss_head_knocked_tex;
  Texture2D npc_tex[4];

  int current_level;
  int intro_step;
  float intro_timer;
  float title_alpha;

  int money;
  int lives;  // 6 half-hearts (3 full hearts)
  bool has_gun;
  int active_weapon;  // 0: Crowbar, 1: Gun
  float crowbar_cooldown;
  float gun_cooldown;
  float player_invincibility_timer;
  float contact_timer;

  int boss_stage;  // 1, 2, 3
  int boss_hp;
  bool boss_active;

  Sound death_snd;
  Music bgm;
  struct bullet bullets[MAX_BULLETS];
  struct enemy enemies[MAX_ENEMIES];
  struct money_item money_items[MAX_MONEY];
};

void manager_init();
void manager_update();
void manager_draw();
void manager_cleanup();

void manager_run_game();

struct manager* manager_get_global();

#endif
