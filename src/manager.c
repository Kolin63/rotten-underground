#include "manager.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "controller.h"
#include "enemy.h"
#include "player.h"
#include "render.h"
#include "tile.h"

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
