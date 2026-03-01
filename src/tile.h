#ifndef CAMPFIRE_TILE_H
#define CAMPFIRE_TILE_H

#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>

#include "tilemaps.h"

#define TILE_SIZE 32

#define MAP_WIDTH tilemap_get_current()->width
#define MAP_HEIGHT tilemap_get_current()->height

typedef enum {
  TILE_GRAVEL = 3,
  TILE_TRACK_LEFT = 5,
  TILE_TRACK_MIDDLE = 7,
  TILE_TRACK_RIGHT = 8,
  TILE_PLATFORM = 9,
  TILE_WALL = 10,
  TILE_RUBBLE = 11,
  TILE_COUNT
} TileType;

struct tilemap {
  size_t width;
  size_t height;
  TileType* tiles;
};

void tilemap_init();
void tilemap_load_level(int level);
void tilemap_cleanup();
void tilemap_draw();
struct tilemap* tilemap_get_current();
const struct level* tilemap_get_current_level();
bool isWalkable(size_t tile_x, size_t tile_y);

#endif
