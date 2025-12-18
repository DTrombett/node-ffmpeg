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
  NODE_API_CALL_DEFAULT(napi_add_finalizer(env, object, &entry->key,
                                           mapFinalizeCb, NULL, &ref), );
  entry->value = ref;
}
#endif