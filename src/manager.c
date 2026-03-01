#include "manager.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "controller.h"
#include "player.h"
#include "render.h"
#include "tile.h"

static struct manager* global_manager;

void manager_init() {
  global_manager = malloc(sizeof(struct manager));
  global_manager->game_should_run = true;
  global_manager->player = malloc(sizeof(struct player));
  global_manager->player->pos.x = 3;
  global_manager->player->pos.y = 3;

  global_manager->tilemap = malloc(sizeof(struct tilemap));
  tilemap_init(global_manager->tilemap);

  global_manager->dialog = malloc(sizeof(struct dialog_box));
  dialog_init(global_manager->dialog);

  InitWindow(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE,
             "Rotten Underground");
  SetTargetFPS(60);

  // Load custom font
  global_manager->font = LoadFont("assets/Jersey15-Regular.ttf");
  global_manager->dialog->font = global_manager->font;

  static const char* intro[] = {"Welcome to the Graywater Subway System.",
                                "Specifically, the 'Rotten Underground'.",
                                "*you hear a faint scurrying sound*"};
  dialog_show(global_manager->dialog, "System", intro, 3);

  global_manager->game_should_run = true;
}

void manager_cleanup() {
  printf("Cleaning up...\n");
  CloseWindow();
  UnloadFont(global_manager->font);
  free(global_manager->dialog);
  free(global_manager->player);
  free(global_manager);
}

void manager_run_game() {
  while (global_manager->game_should_run) {
    controller_tick();
    dialog_update(global_manager->dialog, GetFrameTime());
    render_game();
  }
}

struct manager* manager_get_global() { return global_manager; }
