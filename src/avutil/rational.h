#ifndef RATIONAL_H
#define RATIONAL_H
#include "../utils.h"
#include <libavutil/rational.h>
#include <node_api.h>

WRAP(createAVRational, AVRational, NULL, PROP_GETSET(num, num, int),
     PROP_GETSET(den, den, int))

static napi_value get_AVRational(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo)
  return createAVRational(env, (AVRational *)(native + (uint64_t)data));
}
static napi_value set_AVRational(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo)
  AVRational *rational = (AVRational *)(native + (uint64_t)data);
  *rational = (AVRational){};
  napi_value result;

  NODE_API_CALL(napi_get_named_property(env, argv[0], "num", &result));
  NODE_API_CALL(napi_get_value_int32(env, result, &rational->num));
  NODE_API_CALL(napi_get_named_property(env, argv[0], "den", &result));
  NODE_API_CALL(napi_get_value_int32(env, result, &rational->den));
  return NULL;
}
#endif