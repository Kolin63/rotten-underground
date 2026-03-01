#ifndef CAMPFIRE_TILE_H
#define CAMPFIRE_TILE_H

#include <raylib.h>
#include <stdbool.h>

#include "registry.h"

#define TILE_SIZE 32
#define MAP_WIDTH 25
#define MAP_HEIGHT 19

struct tile_collision_object {
  int x;
  int y;
  int width;
  int height;
};

struct tile {
  const char* filename;
  struct tile_collision_object collisions[32];
  Texture2D sprite;
};

struct tilemap {
  // 2d array of tiles - dynamic size
  int** tiles;
  size_t width;
  size_t height;
};

static struct registry* tileset;

void tileset_init();
void tileset_cleanup();

void tilemap_init(struct tilemap* tm);
void tilemap_draw(const struct tilemap* tm);
bool isWalkable(const struct tilemap* tm, int tile_x, int tile_y);

#endif
