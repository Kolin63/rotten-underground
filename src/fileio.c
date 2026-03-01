#include "fileio.h"

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fileio_check_integrity() {
  char buf[512];
  if (fileio_get_data_root(buf) != 0) {
    return 1;
  }

  char test_file_path[512];
  fileio_get_file_path(test_file_path, "/assets/tileset.tsj");
  FILE* test_file = fopen(test_file_path, "r");
  if (!test_file) {
    printf("Error: could not open file to test asset directory at %s\n",
           test_file_path);
    return 1;
  }
  fclose(test_file);
  return 0;
}

int fileio_get_data_root(char* buf) {
  // first check if in development directory
  const char* const test_file_path = "assets/tileset.tsj";
  FILE* test_file = fopen(test_file_path, "r");
  if (test_file) {
    strcpy(buf, "./");
    return 0;
  }
  fclose(test_file);

#ifdef __linux__
  const char* home = getenv("HOME");
  if (!home) {
    printf("Error: could not get value of $HOME\n");
    return 1;
  }
  strcpy(buf, home);
  strcat(buf, "/.local/share/rotten-underground");
  return 0;
#elifdef _WIN32
  const char* lad = getenv("LOCALAPPDATA");
  if (!lad) {
    printf("Error: could not get value of %%LOCALAPPDATA%%\n");
    return 1;
  }
  strcpy(buf, lad);
  strcat(buf, "/rotten-underground");
  return 0;
#elifdef __APPLE__
  const char* home = getenv("HOME");
  if (!home) {
    printf("Error: could not get value of $HOME\n");
    return 1;
  }
  strcpy(buf, home);
  strcat(buf, "/Library/Application Support");
  return 0;
#else
  static_assert(0, "Could not get data root for this OS");
#endif
  return 1;
}

void fileio_get_file_path(char* buf, const char* rel_path) {
  fileio_get_data_root(buf);
  if (rel_path[0] != '/') {
    strcat(buf, "/");
  }
  strcat(buf, rel_path);
}
