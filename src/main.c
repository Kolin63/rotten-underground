#include <raylib.h>
#include <stdlib.h>

#include "manager.h"
#include "tile.h"

int main(int argc, char** argv) {
  manager_init();
  atexit(manager_cleanup);
  // this is to help testing.
  // USAGE: ./game [level number]
  int start_level = 0;
  if (argc >= 2) {
    start_level = atoi(argv[1]);
  }
  manager_get_global()->current_level = start_level;
  if (start_level != 0) {
    tilemap_load_level(start_level);
  }
  manager_run_game();
  manager_cleanup();

  return 0;
}
