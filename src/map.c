#include "map.h"

MapEntry *refMap = NULL;

void mapFinalizeCb(napi_env env, void *finalize_data, void *finalize_hint) {
  napi_ref ref = mapDeleteKey((MapKey *)finalize_data);
  if (ref)
    napi_delete_reference(env, ref);
  if (finalize_hint)
    ((node_api_basic_finalize)finalize_hint)(env, finalize_data, NULL);
}
