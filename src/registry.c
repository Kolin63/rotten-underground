#include "registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct registry* registry_init(const char* name, size_t val_size) {
  struct registry* reg = malloc(sizeof(struct registry));
  if (!reg) {
    return NULL;
  }

  reg->name = name;
  reg->length = 0;
  reg->val_size = val_size;
  reg->keys = NULL;
  reg->values = NULL;
  return reg;
}

void registry_cleanup(struct registry* reg) {
  if (!reg) {
    return;
  }

  for (size_t i = 0; i < reg->length; i++) {
    free(reg->keys[i]);
  }
  free(reg->keys);
  free(reg->values);
  free(reg);
}

void registry_add(struct registry* reg, const char* key, const void* val) {
  if (!reg || !key || !val) {
    return;
  }

  // first, we need to find what index to insert at
  // the index means: move all items from that index (inclusive) one to the
  // right, then replace the index with this value
  size_t insert_index = 0;
  while (insert_index < reg->length) {
    int cmp = strcmp(key, reg->keys[insert_index]);
    if (cmp == 0) {
      printf("Registry %s already contains key %s\n", reg->name, key);
      return;
    }
    if (cmp < 0) {
      break;  // insert before current element
    }
    insert_index++;  // key > keys[i], keep scanning
  }

  size_t old_length = reg->length;
  size_t new_length = old_length + 1;

  // grow arrays
  // we realloc into this pointer then set the actual pointer later to handle
  // error
  char** new_keys = realloc(reg->keys, new_length * sizeof(char*));
  if (!new_keys) {
    return;
  }

  void* new_values = realloc(reg->values, new_length * reg->val_size);
  if (!new_values) {
    // keys could realloc but values could not. revert keys and return early
    reg->keys = new_keys;
    return;
  }

  reg->keys = new_keys;
  reg->values = new_values;

  // shift existing elements right by one starting at insert_index
  // we use old length (before +1) because new length includes uninitialized new
  // item
  size_t shift_n = old_length - insert_index;
  if (shift_n > 0) {
    char** keys_src = reg->keys + insert_index;
    char** keys_dest = keys_src + 1;
    memmove(keys_dest, keys_src, shift_n * sizeof(char*));

    void* val_src = (char*)reg->values + insert_index * reg->val_size;
    void* val_dest = (char*)val_src + reg->val_size;
    memmove(val_dest, val_src, shift_n * reg->val_size);
  }

  // now shift back by 1 and add new value
  reg->keys[insert_index] = malloc(strlen(key) + 1);
  if (!reg->keys[insert_index]) {
    if (shift_n > 0) {
      memmove(&reg->keys[insert_index], &reg->keys[insert_index + 1],
              shift_n * sizeof(char*));

      char* base = (char*)reg->values;
      memmove(base + insert_index * reg->val_size,
              base + (insert_index + 1) * reg->val_size,
              shift_n * reg->val_size);
    }
    return;
  }
  strcpy(reg->keys[insert_index], key);

  memcpy((char*)reg->values + insert_index * reg->val_size, val, reg->val_size);

  reg->length = new_length;

  printf("Added to registry %s key: %s\n", reg->name, key);
}

struct registry_optional_index registry_get_index(struct registry* reg,
                                                  const char* key) {
  struct registry_optional_index ret = {.exists = false, .index = 0};
  if (!reg || !key) {
    return ret;
  }

  for (size_t i = 0; i < reg->length; i++) {
    int cmp = strcmp(key, reg->keys[i]);
    if (cmp == 0) {
      ret.exists = true;
      ret.index = i;
      return ret;
    }
    if (cmp < 0) {
      // Since keys are sorted ascending, if key < keys[i], it can't appear
      // later
      return ret;
    }
    // else cmp > 0: keep scanning
  }

  return ret;
}

void* registry_get(struct registry* reg, const char* key) {
  struct registry_optional_index opt_index = registry_get_index(reg, key);
  if (!opt_index.exists) {
    return NULL;
  }
  return (char*)reg->values + opt_index.index * reg->val_size;
}

void* registry_get_by_index(struct registry* reg, size_t i) {
  if (i >= reg->length) {
    return NULL;
  }
  return (char*)reg->values + i * reg->val_size;
}
