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
static napi_value get_AVRational(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo)
  return createAVRational(env, (AVRational *)(native + (uint64_t)data));
}
static napi_value set_AVRational(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo)
  parseAVRational(env, argv[0], (AVRational *)(native + (uint64_t)data));
  return NULL;
}
#endif