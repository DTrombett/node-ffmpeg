#ifndef MAP_H
#define MAP_H
#include "uthash.h"
#include <node_api.h>

typedef struct PtrMapEntry {
  const void *key;
  napi_value value;
  UT_hash_handle hh;
} PtrMapEntry;

extern PtrMapEntry *valuesMap;

inline void mapAdd(const void *key, napi_value value) {
  PtrMapEntry *entry = malloc(sizeof(PtrMapEntry));
  entry->key = key;
  entry->value = value;
  HASH_ADD_PTR(valuesMap, key, entry);
}

inline napi_value mapGet(const void *key) {
  PtrMapEntry *entry = NULL;
  HASH_FIND_PTR(valuesMap, &key, entry);
  return entry ? entry->value : NULL;
}

inline void mapDelete(const void *key) {
  PtrMapEntry *entry = NULL;
  HASH_FIND_PTR(valuesMap, &key, entry);
  if (entry) {
    HASH_DEL(valuesMap, entry);
    free(entry);
  }
}

inline void mapClear() {
  PtrMapEntry *current, *tmp;
  HASH_ITER(hh, valuesMap, current, tmp) {
    HASH_DEL(valuesMap, current);
    free(current);
  }
}

void mapFinalizeCb(napi_env env, void *finalize_data, void *);

#endif
