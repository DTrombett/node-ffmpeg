#include "./utils.h"
#include <node_api.h>

napi_value get_int(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo)
  return NUMBER(*(int *)(native + (uint64_t)data));
}
napi_value set_int(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo)
  int value;

  NODE_API_CALL(napi_get_value_int32(env, argv[0], &value));
  *(int *)(native + (uint64_t)data) = value;
  return NULL;
}
napi_value get_int64_t(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo)
  return NUMBER(*(int64_t *)(native + (uint64_t)data));
}
napi_value set_int64_t(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo)
  int64_t value;

  NODE_API_CALL(napi_get_value_int64(env, argv[0], &value));
  *(int *)(native + (uint64_t)data) = value;
  return NULL;
}
napi_value get_float(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo)
  return NUMBER(*(float *)(native + (uint64_t)data));
}
napi_value set_float(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo)
  double value;

  NODE_API_CALL(napi_get_value_double(env, argv[0], &value));
  *(double *)(native + (uint64_t)data) = value;
  return NULL;
}
napi_value get_double(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo)
  return NUMBER(*(float *)(native + (uint64_t)data));
}
napi_value set_double(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo)
  double value;

  NODE_API_CALL(napi_get_value_double(env, argv[0], &value));
  *(double *)(native + (uint64_t)data) = value;
  return NULL;
}
napi_value get_string(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo)
  return STRING(*(char **)(native + (uint64_t)data));
}
napi_value set_string(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo)
  char **pointer = (char **)(native + (uint64_t)data);

  free(*pointer);
  *pointer = toChar(env, argv[0]);
  return NULL;
}