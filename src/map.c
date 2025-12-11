#include "map.h"
#include <stdio.h>

PtrMapEntry *refMap = NULL;

void mapFinalizeCb(napi_env env, void *finalize_data, void *finalize_hint) {
  printf("mapFinalizeCb called\n");
  mapDelete(finalize_data);
  if (finalize_hint)
    ((void (*)(struct napi_env__ *, void *, void *))finalize_hint)(
        env, finalize_data, NULL);
}