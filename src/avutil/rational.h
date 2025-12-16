#ifndef RATIONAL_H
#define RATIONAL_H
#include "../utils.h"
#include <libavutil/rational.h>
#include <node_api.h>

WRAP(createAVRational, AVRational, NULL, PROP_GETSET(num, num, int),
     PROP_GETSET(den, den, int))

static inline void parseAVRational(napi_env env, napi_value object,
                                   AVRational *result) {
  napi_value value;

  NODE_API_CALL_DEFAULT(napi_get_named_property(env, object, "num", &value), );
  result->num = parseInt(env, value, true, result->num);
  NODE_API_CALL_DEFAULT(napi_get_named_property(env, object, "den", &value), );
  result->den = parseInt(env, value, true, result->den);
}
#endif