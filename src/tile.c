#include "tile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "manager.h"
#include "tilemaps.h"

static struct tilemap* current_tilemap;

void tilemap_init() {
  current_tilemap = malloc(sizeof(struct tilemap));
  current_tilemap->width = 0;
  current_tilemap->height = 0;
  current_tilemap->tiles = NULL;
}

void tilemap_load_level(int level) {
  const struct level* level_data = &levels[level];
  current_tilemap->width = level_data->width;
  current_tilemap->height = level_data->height;
  current_tilemap->tiles =
      realloc(current_tilemap->tiles,
              level_data->width * level_data->height * sizeof(TileType));
  const char* tilestr = level_data->tilemap;
  for (size_t i = 0; i < strlen(tilestr); i++) {
    char c = tilestr[i];
    int tile_id =
        (c - '0') - 1;  // convert Tiled GID to local tile ID (firstgid=1)
    current_tilemap->tiles[i] = tile_id;
  }

  manager_get_global()->camera.target.x = level_data->width / 2.0 * TILE_SIZE;
  manager_get_global()->camera.target.y = level_data->height / 2.0 * TILE_SIZE;

  printf("CAMERA SETTING POS ON LOAD LEVEL: %f, %f\n", manager_get_global()->camera.target.x, manager_get_global()->camera.target.y);
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

bool isWalkable(size_t tile_x, size_t tile_y) {
  if (tile_x >= current_tilemap->width || tile_y >= current_tilemap->height) {
    return false;
  }
  // All tiles are now walkable
  return true;
}
