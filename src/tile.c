#include "tile.h"

#include <cjson/cJSON.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "fileio.h"
#include "registry.h"

void tileset_init() {
  tileset = registry_init("tileset", sizeof(struct tile));
  char path[512];
  fileio_get_file_path(path, "/assets/tileset.tsj");
  FILE* file = fopen(path, "r");
  if (!file) {
    printf("Error: could not open tileset %s\n", path);
    exit(EXIT_FAILURE);
  }

  struct fileio fileio = {.buf = NULL, .buf_size = 0};
  fileio_read_all(&fileio, file);

  const cJSON* cjson = cJSON_Parse(fileio.buf);
  const cJSON* tiles_list = cJSON_GetObjectItemCaseSensitive(cjson, "tiles");
  if (!cJSON_IsArray(tiles_list)) {
    printf("Error: could not get array 'tiles' from %s", path);
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < cJSON_GetArraySize(tiles_list); i++) {
    // cJSON* x =
    // cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(tiles_list, i), "x");
    // cJSON* y;
    // cJSON* width;
    // cJSON* height;
    const cJSON* elem = cJSON_GetArrayItem(tiles_list, i);
    const cJSON* filename = cJSON_GetObjectItemCaseSensitive(elem, "image");

    struct tile tile = {0};
  }
}

void tilemap_init(struct tilemap* tm) {
  // todo
}

void tilemap_draw(const struct tilemap* tm) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
    }
  }
}

bool isWalkable(const struct tilemap* tm, int tile_x, int tile_y) {
  if (tile_x < 0 || tile_x >= MAP_WIDTH || tile_y < 0 || tile_y >= MAP_HEIGHT) {
    return false;
  }
  return true;
}
