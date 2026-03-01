#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "fileio.h"
#include "manager.h"

int main() {
  if (fileio_check_integrity() != 0) {
    printf("Error: Could not find assets directory\n");
    return 1;
  }
  manager_init();
  atexit(manager_cleanup);
  manager_run_game();
  manager_cleanup();

  return 0;
}
