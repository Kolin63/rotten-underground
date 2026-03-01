#include "manager.h"

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "controller.h"
#include "enemy.h"
#include "player.h"
#include "render.h"
#include "tile.h"
#include "tilemaps.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900

static struct manager* global_manager;

static bool cleanup_done = false;

void manager_init() {
  srand(time(NULL));
  global_manager = malloc(sizeof(struct manager));
  global_manager->game_should_run = true;
  global_manager->player = malloc(sizeof(struct player));
  global_manager->player->pos.x = 4 * TILE_SIZE;
  global_manager->player->pos.y = 4 * TILE_SIZE;

  global_manager->camera.offset.x = SCREEN_WIDTH / 2.0;
  global_manager->camera.offset.y = SCREEN_HEIGHT / 2.0;
  global_manager->camera.target.x = 0;
  global_manager->camera.target.y = 0;
  global_manager->camera.rotation = 0.0f;
  global_manager->camera.zoom = 1.0f;

  global_manager->current_level = 0;
  global_manager->intro_step = 0;
  global_manager->intro_timer = 0;
  global_manager->title_alpha = 0.0f;
  global_manager->money = 0;
  global_manager->lives = 3;
  global_manager->has_gun = false;
  global_manager->active_weapon = 0;
  global_manager->boss_active = false;
  global_manager->boss_hp = 100;

  for (int i = 0; i < MAX_MONEY; i++) {
    global_manager->money_items[i].active = false;
  }
  global_manager->boss_stage = 1;

  tilemap_init();
  tilemap_load_level(0);

  global_manager->dialog = malloc(sizeof(struct dialog_box));
  dialog_init(global_manager->dialog);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rotten Underground");
  SetTargetFPS(60);

  global_manager->font = LoadFont("assets/Jersey15-Regular.ttf");
  global_manager->dialog->font = global_manager->font;

  global_manager->player_tex = LoadTexture("assets/player.png");
  for (int i = 0; i < 7; i++) {
    char path[64];
    sprintf(path, "assets/ratStage1/1rat_%d.png", i + 1);
    global_manager->rat_anim[i] = LoadTexture(path);
  }
  global_manager->gravel_tex = LoadTexture("assets/gravel.png");
  global_manager->track_left_tex = LoadTexture("assets/trackLeft.png");
  global_manager->track_middle_tex = LoadTexture("assets/trackMiddle.png");
  global_manager->track_right_tex = LoadTexture("assets/trackRight.png");
  global_manager->platform_tex = LoadTexture("assets/platform.png");
  global_manager->rubble_tex = LoadTexture("assets/rubble.png");
  global_manager->wall_tex = LoadTexture("assets/wall.png");
  global_manager->coin_tex = LoadTexture("assets/coin.png");
  global_manager->heart_tex = LoadTexture("assets/heart.png");
  global_manager->halfheart_tex = LoadTexture("assets/halfHeart.png");
  global_manager->boss_tex[0] = LoadTexture("assets/boss_1.png");
  global_manager->boss_tex[1] = LoadTexture("assets/boss_2.png");
  global_manager->boss_tex[2] = LoadTexture("assets/boss_3.png");
  global_manager->boss_head_knocked_tex =
      LoadTexture("assets/boss_head_knocked.png");
  InitAudioDevice();
  global_manager->death_snd = LoadSound("assets/death.mp3");

  static const char* intro[] = {"Welcome to the Graywater Subway System.",
                                "Specifically, the 'Rotten Underground'.",
                                "*you hear a faint scurrying sound*"};
  dialog_show(global_manager->dialog, "System", intro, 3);

  global_manager->game_should_run = true;

  for (int i = 0; i < MAX_BULLETS; i++) {
    bullet(&global_manager->bullets[i]);
  }

  enemies_init(global_manager->enemies);
  enemies_spawn(global_manager->enemies, tilemap_get_current_level()->rat_spawns);
}

void manager_cleanup() {
  if (cleanup_done) return;
  cleanup_done = true;
  printf("Cleaning up...\n");
  UnloadTexture(global_manager->player_tex);
  for (int i = 0; i < 7; i++) {
    UnloadTexture(global_manager->rat_anim[i]);
  }
  UnloadTexture(global_manager->gravel_tex);
  UnloadTexture(global_manager->track_left_tex);
  UnloadTexture(global_manager->track_middle_tex);
  UnloadTexture(global_manager->track_right_tex);
  UnloadTexture(global_manager->platform_tex);
  UnloadTexture(global_manager->rubble_tex);
  UnloadTexture(global_manager->wall_tex);
  UnloadTexture(global_manager->coin_tex);
  UnloadTexture(global_manager->heart_tex);
  UnloadTexture(global_manager->halfheart_tex);
  for (int i = 0; i < 3; i++) {
    UnloadTexture(global_manager->boss_tex[i]);
  }
  UnloadTexture(global_manager->boss_head_knocked_tex);
  UnloadSound(global_manager->death_snd);
  UnloadFont(global_manager->font);
  CloseAudioDevice();
  CloseWindow();
  free(global_manager->dialog);
  free(global_manager->player);
  free(global_manager);
}

void manager_run_game() {
  while (global_manager->game_should_run) {
    float dt = GetFrameTime();
    controller_tick();
    dialog_update(global_manager->dialog, dt);

    // Level Scripting Logic
    if (global_manager->current_level == 0) {
      if (!global_manager->dialog->active) {
        if (global_manager->intro_step == 0) {
          // Trigger bite
          global_manager->intro_step = 1;
          global_manager->intro_timer = 2.0f;
          static const char* bite[] = {"*gets bitten by a rat*",
                                       "Agh! These pesky rats....",
                                       "*starts chasing after them*"};
          dialog_show(global_manager->dialog, "Johnny", bite, 3);
        } else if (global_manager->intro_step == 1) {
          // Chase logic
          if (global_manager->intro_timer > 0) {
            global_manager->intro_timer -= dt;
            global_manager->player->pos.x += 200 * dt;
          } else {
            global_manager->intro_step = 2;
            // Tunnel collapse
            struct tilemap* tm = tilemap_get_current();
            int p_tile_x = (int)(global_manager->player->pos.x / TILE_SIZE);
            for (size_t y = 0; y < tm->height; y++) {
              for (int x = 0; x < p_tile_x - 3 && x < (int)tm->width; x++) {
                tm->tiles[y * tm->width + x] = TILE_RUBBLE;
              }
            }
            static const char* collapse[] = {"*tunnel collapses behind you*",
                                             "Holy- What just happened?!"};
            dialog_show(global_manager->dialog, "Johnny", collapse, 2);
          }
        } else if (global_manager->intro_step == 2) {
          global_manager->intro_step = 3;
          static const char* sealed[] = {"It’s way too high, I’m sealed in.",
                                         "I guess there’s only one way left."};
          dialog_show(global_manager->dialog, "Johnny", sealed, 2);
        } else if (global_manager->intro_step == 3) {
          // Show Title
          global_manager->intro_step = 4;
          global_manager->intro_timer = 3.0f;
          global_manager->title_alpha = 0.0f;
        } else if (global_manager->intro_step == 4) {
          if (global_manager->intro_timer > 0) {
            global_manager->intro_timer -= dt;
            global_manager->title_alpha += dt / 1.5f;
            if (global_manager->title_alpha > 1.0f)
              global_manager->title_alpha = 1.0f;
          } else {
            global_manager->intro_step = 5;
            global_manager->intro_timer = 1.0f;
          }
        }
      }
    } else if (global_manager->current_level == 2) {
      if (global_manager->intro_step == 0) {
        global_manager->intro_step = 1;
        static const char* crowbar[] = {
            "Press left mouse btn to use your crowbar"};
        dialog_show(global_manager->dialog, "Tutorial", crowbar, 1);
      }
    } else if (global_manager->current_level == 4) {
      if (!global_manager->has_gun) {
        // Simple pickup logic: if player near center
        if (Vector2Distance((Vector2){global_manager->player->pos.x,
                                      global_manager->player->pos.y},
                            (Vector2){500, 500}) < 50) {
          global_manager->has_gun = true;
          static const char* gun[] = {"Ooh, a gun! This might be useful...",
                                      "Right Click to swap weapons."};
          dialog_show(global_manager->dialog, "Johnny", gun, 2);
        }
      }
    } else if (global_manager->current_level == 10) {
      if (!global_manager->boss_active) {
        global_manager->boss_active = true;
        global_manager->boss_hp = 300;
        global_manager->boss_stage = 1;
        static const char* bossIntro[] = {"Who goes there?!",
                                          "You shall not pass Graywater!"};
        dialog_show(global_manager->dialog, "The Rat King", bossIntro, 2);
      } else {
        // Boss death logic
        if (global_manager->boss_hp <= 0) {
          static const char* bossDead[] = {"Aargh... my kingdom...",
                                           "*fades away*"};
          dialog_show(global_manager->dialog, "The Rat King", bossDead, 2);
          global_manager->boss_active = false;
          // Game win logic?
        } else if (global_manager->boss_hp < 100 &&
                   global_manager->boss_stage == 2) {
          global_manager->boss_stage = 3;
          static const char* stage3[] = {"ENOUGH! FEEL THE WRATH!"};
          dialog_show(global_manager->dialog, "The Rat King", stage3, 1);
        } else if (global_manager->boss_hp < 200 &&
                   global_manager->boss_stage == 1) {
          global_manager->boss_stage = 2;
          static const char* stage2[] = {
              "You're tougher than you look, worker!"};
          dialog_show(global_manager->dialog, "The Rat King", stage2, 1);
        }
      }
    }

    // Level Transition Logic
    if (global_manager->player->pos.x > (MAP_WIDTH - 1) * TILE_SIZE - 20) {
      if (global_manager->current_level < LEVELS_AMOUNT - 1) {
        global_manager->current_level++;
        tilemap_load_level(global_manager->current_level);
        global_manager->player->pos.x = TILE_SIZE;
        // spawn enemies and money for new level
        enemies_spawn(global_manager->enemies);

        // Basic money spawn for testing
        for (int i = 0; i < 5; i++) {
          global_manager->money_items[i].pos = (Vector2){100 + i * 160, 100};
          global_manager->money_items[i].active = true;
        }
      }
    }

    Vector2 playerPos = {global_manager->player->pos.x,
                         global_manager->player->pos.y};
    enemies_update(global_manager->enemies, playerPos, dt,
                   global_manager->dialog->active);

    for (int i = 0; i < MAX_BULLETS; i++) {
      if (!global_manager->bullets[i].active) continue;
      bullet_update(&global_manager->bullets[i], dt);

      // Check collision with enemies
      for (int j = 0; j < MAX_ENEMIES; j++) {
        if (!global_manager->enemies[j].active) continue;

        Vector2 enemyCenter = (Vector2){global_manager->enemies[j].pos.x + 8,
                                        global_manager->enemies[j].pos.y + 8};
        if (CheckCollisionCircles(global_manager->bullets[i].pos, BULLET_RADIUS,
                                  enemyCenter, ENEMY_RADIUS)) {
          PlaySound(global_manager->death_snd);
          global_manager->bullets[i].active = false;
          global_manager->enemies[j].active = false;
          break;
        }
      }

      int tx = (int)(global_manager->bullets[i].pos.x / TILE_SIZE);
      int ty = (int)(global_manager->bullets[i].pos.y / TILE_SIZE);
      if (!isWalkable(tx, ty)) {
        global_manager->bullets[i].active = false;
      }
    }

    render_game();
  }
}

struct manager* manager_get_global() { return global_manager; }
