#include "utils.h"

napi_value get_int(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo, int)
  return NUMBER(*data);
}
napi_value set_int(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo, int)
  *data = parseInt(env, argv[0], true, 0);
  return UNDEFINED;
}
napi_value get_int64_t(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo, int64_t)
  return NUMBER(*data);
}
napi_value set_int64_t(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo, int64_t)
  *data = parseInt64(env, argv[0], true, 0);
  return UNDEFINED;
}
napi_value get_float(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo, float)
  return NUMBER(*data);
}
napi_value set_float(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo, float)
  *data = parseDouble(env, argv[0], true, 0);
  return UNDEFINED;
}
napi_value get_double(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo, double)
  return NUMBER(*data);
}
napi_value set_double(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo, double)
  *data = parseDouble(env, argv[0], true, 0);
  return UNDEFINED;
}
napi_value get_string(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo, char *)
  return STRING(*data);
}
napi_value set_string(napi_env env, napi_callback_info cbinfo) {
  LOAD_SET(cbinfo, char *)
  free(*data);
  *data = parseString(env, argv[0]);
  return UNDEFINED;
}
napi_value get_External(napi_env env, napi_callback_info cbinfo) {
  LOAD_GET(cbinfo, void *)
  return External(env, *data, NULL, NULL);
}