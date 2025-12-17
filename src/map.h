#ifndef MAP_H
#define MAP_H
#include "uthash.h"
#include <node_api.h>

typedef struct MapEntry {
  const void *key;
  napi_ref value;
  UT_hash_handle hh;
} MapEntry;

extern MapEntry *refMap;

inline void mapAdd(const void *key, napi_ref value) {
  MapEntry *entry = malloc(sizeof(MapEntry));
  entry->key = key;
  entry->value = value;
  HASH_ADD_PTR(refMap, key, entry);
}

inline napi_ref mapGet(const void *key) {
  MapEntry *entry = NULL;
  HASH_FIND_PTR(refMap, &key, entry);
  return entry ? entry->value : NULL;
}

inline napi_ref mapDelete(const void *key) {
  MapEntry *entry = NULL;
  HASH_FIND_PTR(refMap, &key, entry);
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
