#include "tile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "manager.h"
#include "tilemaps.h"

static struct tilemap* current_tilemap;
static const struct level* current_level;

void tilemap_init() {
  current_tilemap = malloc(sizeof(struct tilemap));
  current_tilemap->width = 0;
  current_tilemap->height = 0;
  current_tilemap->tiles = NULL;
}

void tilemap_load_level(int level) {
  const struct level* level_data = &levels[level];
  current_level = level_data;
  current_tilemap->width = level_data->width;
  current_tilemap->height = level_data->height;

  char filename[128];
  snprintf(filename, sizeof(filename), "assets/levels/level%d.tmj", level);
  char* file_text = LoadFileText(filename);

  if (file_text == NULL) {
    printf("ERROR: Could not load level file: %s\n", filename);
    return;
  }

  // Simple JSON parser for "data" array
  char* data_start = strstr(file_text, "\"data\":[");
  if (data_start) {
    data_start += 8;  // move past "data":[
    current_tilemap->tiles =
        realloc(current_tilemap->tiles,
                level_data->width * level_data->height * sizeof(TileType));

    char* ptr = data_start;
    for (size_t i = 0; i < level_data->width * level_data->height; i++) {
      while (*ptr && !(*ptr >= '0' && *ptr <= '9')) ptr++;
      if (*ptr == '\0') break;
      int gid = atoi(ptr);
      current_tilemap->tiles[i] = gid;
      while (*ptr && (*ptr >= '0' && *ptr <= '9')) ptr++;
    }
  }

  UnloadFileText(file_text);

  manager_get_global()->camera.target.x = level_data->width / 2.0 * TILE_SIZE;
  manager_get_global()->camera.target.y = level_data->height / 2.0 * TILE_SIZE;

  manager_get_global()->player->pos.x = level_data->player_spawn.x * TILE_SIZE;
  manager_get_global()->player->pos.y = level_data->player_spawn.y * TILE_SIZE;
}

void tilemap_cleanup() {
  free(current_tilemap->tiles);
  free(current_tilemap);
}

void tilemap_draw() {
  struct manager* mgr = manager_get_global();
  for (size_t y = 0; y < current_tilemap->height; y++) {
    for (size_t x = 0; x < current_tilemap->width; x++) {
      Texture2D tex;
      switch (current_tilemap->tiles[y * current_tilemap->width + x]) {
      case TILE_GRAVEL:
        tex = mgr->gravel_tex;
        break;
      case TILE_TRACK_LEFT:
        tex = mgr->track_left_tex;
        break;
      case TILE_TRACK_MIDDLE:
        tex = mgr->track_middle_tex;
        break;
      case TILE_TRACK_RIGHT:
        tex = mgr->track_right_tex;
        break;
      case TILE_PLATFORM:
        tex = mgr->platform_tex;
        break;
      case TILE_RUBBLE:
        tex = mgr->rubble_tex;
        break;
      case TILE_WALL:
        tex = mgr->wall_tex;
        break;
      case TILE_LABFLOOR:
        tex = mgr->labfloor_tex;
        break;
      case TILE_GOOFLOOR:
        tex = mgr->goofloor_tex;
        break;
      default:
        tex = mgr->gravel_tex;
        break;
      }

      // Draw scaled texture to fill the 32x32 tile area
      Rectangle source = {0.0f, 0.0f, (float)tex.width, (float)tex.height};
      Rectangle dest = {(float)x * TILE_SIZE, (float)y * TILE_SIZE,
                        (float)TILE_SIZE, (float)TILE_SIZE};
      Vector2 origin = {0.0f, 0.0f};
      DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
    }
  }
}

struct tilemap* tilemap_get_current() { return current_tilemap; }

const struct level* tilemap_get_current_level() { return current_level; }

bool isWalkable(size_t tile_x, size_t tile_y) {
  TileType tile_type =
      current_tilemap->tiles[tile_y * current_tilemap->width + tile_x];
  if (tile_x >= current_tilemap->width || tile_y >= current_tilemap->height ||
      tile_type == TILE_WALL || tile_type == TILE_RUBBLE) {
    return false;
  }
  // All tiles are now walkable
  return true;
}
