#ifndef RATIONAL_H
#define RATIONAL_H
#include "../utils.h"
#include <libavutil/rational.h>

WRAP(createAVRational, AVRational, NULL, PROP_GETSET(num, num, int),
     PROP_GETSET(den, den, int))

static inline void parseAVRational(napi_env env, napi_value object,
                                   AVRational *result) {
  napi_value value;
  napi_ref ref;

  mapDelete(result, sizeof(*result));
  NODE_API_CALL_DEFAULT(napi_get_named_property(env, object, "num", &value), );
  result->num = parseInt(env, value, true, result->num);
  NODE_API_CALL_DEFAULT(napi_get_named_property(env, object, "den", &value), );
  result->den = parseInt(env, value, true, result->den);
  MapEntry *entry = mapAdd(result, sizeof(*result), NULL);
  if (!entry) {
    napi_throw_error(env, NULL, "Failed to allocate map entry");
    return;
  }
  NODE_API_CALL_DEFAULT(napi_add_finalizer(env, object, &entry->key,
                                           mapFinalizeCb, NULL, &ref), );
  entry->value = ref;
}

napi_value get_AVRational(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo, AVRational)
  napi_ref ref = mapGet(data, sizeof(*data));
  napi_value value;

  if (ref) {
    NODE_API_CALL(napi_get_reference_value(env, ref, &value));
    if (value)
      return value;
    mapDelete(data, sizeof(*data));
    ref = NULL;
  }
  value = createAVRational(env, data);
  MapEntry *entry = mapAdd(data, sizeof(*data), NULL);
  if (!entry) {
    napi_throw_error(env, NULL, "Failed to allocate map entry");
    return NULL;
  }
  NODE_API_CALL(
      napi_add_finalizer(env, value, &entry->key, mapFinalizeCb, NULL, &ref));
  entry->value = ref;
  return value;
}
napi_value set_AVRational(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo, AVRational)
  parseAVRational(env, argv[0], data);
  return UNDEFINED;
}
#endif