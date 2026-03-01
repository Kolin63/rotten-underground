#ifndef CAMPFIRE_TILE_H
#define CAMPFIRE_TILE_H

#include <raylib.h>
#include <stdbool.h>

#define TILE_SIZE 32
#define MAP_WIDTH 20
#define MAP_HEIGHT 15

typedef enum {
  TILE_GRAVEL,
  TILE_TRACK_LEFT,
  TILE_TRACK_RIGHT,
  TILE_PLATFORM,
  TILE_COUNT
} TileType;

struct tilemap {
  TileType tiles[MAP_HEIGHT][MAP_WIDTH];
};

void tilemap_init(struct tilemap* tm);
void tilemap_draw(const struct tilemap* tm);
bool isWalkable(const struct tilemap* tm, int tile_x, int tile_y);

#endif
