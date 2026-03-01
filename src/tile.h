#ifndef CAMPFIRE_TILE_H
#define CAMPFIRE_TILE_H

#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>

#define TILE_SIZE 32

#define MAP_WIDTH tilemap_get_current()->width
#define MAP_HEIGHT tilemap_get_current()->width

typedef enum {
  TILE_GRAVEL = 2,
  TILE_TRACK_LEFT = 4,
  TILE_TRACK_MIDDLE = 6,
  TILE_TRACK_RIGHT = 7,
  TILE_PLATFORM = 8,
  TILE_WALL = 9,
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
bool isWalkable(size_t tile_x, size_t tile_y);

#endif
