#ifndef CAMPFIRE_REGISTRY_H
#define CAMPFIRE_REGISTRY_H

#include <stdlib.h>

struct registry {
  const char* name;
  size_t length;
  size_t val_size;
  char** keys;
  void* values;
};

struct registry_optional_index {
  bool exists;
  size_t index;
};

struct registry* registry_init(const char* name, size_t val_size);
void registry_cleanup(struct registry* reg);

void registry_add(struct registry* reg, const char* key, const void* val);

// exists will be false if element is not there
struct registry_optional_index registry_get_index(struct registry* reg,
                                                  const char* key);

// returns NULL if element is not there
void* registry_get(struct registry* reg, const char* key);
void* registry_get_by_index(struct registry* reg, size_t i);

#endif
