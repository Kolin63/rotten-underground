#ifndef CAMPFIRE_FILEIO_H
#define CAMPFIRE_FILEIO_H

#include <stdio.h>
#include <stdlib.h>

struct fileio {
  char* buf;
  size_t buf_size;
};

// Moves all contents of file into a buffer
void fileio_read_all(struct fileio* buf, FILE* file);

// checks that the assets directory is working well
// returns 0 if ok
int fileio_check_integrity();

// gets project data root path
// on linux something like: "/home/colin/.local/share/rotten-underground"
// returns 0 if ok
int fileio_get_data_root(char* buf);

// gets absolute path from relative path
// for example: "assets/tileset.tsj" -->
// "/home/colin/.local/share/rotten-underground/assets/tileset.tsj"
void fileio_get_file_path(char* buf, const char* rel_path);

#endif
