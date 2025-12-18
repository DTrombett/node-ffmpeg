#ifndef MAP_H
#define MAP_H
#include "uthash.h"
#include <node_api.h>
#include <stdio.h>

typedef struct {
  void *ptr;
  size_t size;
} MapKey;

typedef struct {
  MapKey key;
  napi_ref value;
  UT_hash_handle hh;
} MapEntry;

extern MapEntry *refMap;

inline MapEntry *mapAdd(void *ptr, size_t size, napi_ref value) {
  MapEntry *entry = malloc(sizeof(MapEntry));

  if (!entry) {
    fprintf(stderr, "Failed to allocate memory for MapEntry\n");
    return NULL;
  }
  memset(entry, 0, sizeof(MapEntry));
  entry->key.ptr = ptr;
  entry->key.size = size;
  entry->value = value;
  HASH_ADD_PTRSIZE(refMap, key, entry);
  return entry;
}

inline napi_ref mapGet(void *ptr, size_t size) {
  MapEntry search;
  MapEntry *entry = NULL;

  memset(&search, 0, sizeof(MapEntry));
  search.key.ptr = ptr;
  search.key.size = size;
  HASH_FIND_PTRSIZE(refMap, &search.key, entry);
  return entry ? entry->value : NULL;
}

inline napi_ref mapDelete(void *ptr, size_t size) {
  const MapKey key = (MapKey){ptr, size};
  MapEntry *entry = NULL;

  HASH_FIND_PTRSIZE(refMap, &key, entry);
  if (entry) {
    napi_ref ref = entry->value;

    HASH_DEL(refMap, entry);
    free(entry);
    return ref;
  }
  return NULL;
}

inline napi_ref mapDeleteKey(const MapKey *key) {
  MapEntry *entry = NULL;

  HASH_FIND_PTRSIZE(refMap, key, entry);
  if (entry) {
    napi_ref ref = entry->value;

    HASH_DEL(refMap, entry);
    free(entry);
    return ref;
  }
  return NULL;
}

inline void mapClear() {
  MapEntry *current, *tmp;
  HASH_ITER(hh, refMap, current, tmp) {
    HASH_DEL(refMap, current);
    free(current);
  }
}

void mapFinalizeCb(napi_env env, void *finalize_data, void *);

#endif
