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

static struct manager* global_manager;

static bool cleanup_done = false;

void manager_init() {
  srand(time(NULL));
  global_manager = malloc(sizeof(struct manager));
  global_manager->game_should_run = true;
  global_manager->player = malloc(sizeof(struct player));
  global_manager->player->pos.x = 4 * TILE_SIZE;
  global_manager->player->pos.y = 4 * TILE_SIZE;

  global_manager->tilemap = malloc(sizeof(struct tilemap));
  tilemap_init(global_manager->tilemap);

  global_manager->dialog = malloc(sizeof(struct dialog_box));
  dialog_init(global_manager->dialog);

  InitWindow(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE,
             "Rotten Underground");
  SetTargetFPS(60);

  global_manager->font = LoadFont("assets/Jersey15-Regular.ttf");
  global_manager->dialog->font = global_manager->font;

  global_manager->player_tex = LoadTexture("assets/player.png");
  global_manager->rat_tex = LoadTexture("assets/rat.png");
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
  enemies_spawn(global_manager->enemies, global_manager->tilemap);
}

void manager_cleanup() {
  if (cleanup_done) return;
  cleanup_done = true;
  printf("Cleaning up...\n");
  UnloadTexture(global_manager->player_tex);
  UnloadTexture(global_manager->rat_tex);
  UnloadSound(global_manager->death_snd);
  UnloadFont(global_manager->font);
  CloseAudioDevice();
  CloseWindow();
  free(global_manager->dialog);
  free(global_manager->tilemap);
  free(global_manager->player);
  free(global_manager);
}

void manager_run_game() {
  while (global_manager->game_should_run) {
    float dt = GetFrameTime();
    controller_tick();
    dialog_update(global_manager->dialog, dt);

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
      if (!isWalkable(global_manager->tilemap, tx, ty)) {
        global_manager->bullets[i].active = false;
      }
    }

    render_game();
  }
}

struct manager* manager_get_global() { return global_manager; }
