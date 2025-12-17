#include "map.h"

MapEntry *refMap = NULL;

void mapFinalizeCb(napi_env env, void *finalize_data, void *finalize_hint) {
  napi_ref ref = mapDelete(finalize_data);
  if (ref)
    napi_delete_reference(env, ref);
  if (finalize_hint)
    ((void (*)(struct napi_env__ *, void *, void *))finalize_hint)(
        env, finalize_data, NULL);
}
