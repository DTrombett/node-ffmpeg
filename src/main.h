#define NAPI_VERSION 10
#define NODE_API_CALL(call)                                                    \
  do {                                                                         \
    napi_status status = (call);                                               \
    if (status != napi_ok) {                                                   \
      const napi_extended_error_info *error_info = NULL;                       \
      napi_get_last_error_info(env, &error_info);                              \
      const char *err_message = error_info->error_message;                     \
      bool is_pending;                                                         \
      napi_is_exception_pending(env, &is_pending);                             \
      if (!is_pending) {                                                       \
        const char *message =                                                  \
            (err_message == NULL) ? "empty error message" : err_message;       \
        napi_throw_error(env, NULL, message);                                  \
      }                                                                        \
      return NULL;                                                             \
    }                                                                          \
  } while (0)
#define NODE_SET_PROPERTY(object, name, value)                                 \
  NODE_API_CALL(napi_set_named_property(env, (object), (name), (value)))
#define NODE_LOAD_ARGUMENTS(count, cbinfo)                                     \
  napi_value arguments[(count)];                                               \
  {                                                                            \
    size_t argc = (count);                                                     \
    NODE_API_CALL(                                                             \
        napi_get_cb_info(env, (cbinfo), &argc, arguments, NULL, NULL));        \
  }
#define NODE_STRING(str) String(env, (str))
#define NODE_NUMBER(value) Number(env, (value))
#define NODE_FUNCTION(name) Function(env, #name, (name))
#define FREEZE(object) NODE_API_CALL(napi_object_freeze(env, (object)))
#define _GET_MACRO(_1, _2, NAME, ...) NAME
#define Array(...)                                                             \
  _GET_MACRO(__VA_ARGS__, createArrayWithLength, createArray)(__VA_ARGS__)
#include <node_api.h>
#include <stdlib.h>

static inline napi_value String(napi_env env, const char *str) {
  if (!str)
    return NULL;
  napi_value result;

  NODE_API_CALL(napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &result));
  return result;
}
static inline napi_value Number(napi_env env, double value) {
  napi_value result;

  NODE_API_CALL(napi_create_double(env, value, &result));
  return result;
}
static inline napi_value Object(napi_env env) {
  napi_value result;

  NODE_API_CALL(napi_create_object(env, &result));
  return result;
}
static inline napi_value createArrayWithLength(napi_env env, size_t length) {
  napi_value result;

  NODE_API_CALL(napi_create_array_with_length(env, length, &result));
  return result;
}
static inline napi_value createArray(napi_env env) {
  napi_value result;

  NODE_API_CALL(napi_create_array(env, &result));
  return result;
}
static inline napi_value Function(napi_env env, const char *name,
                                  napi_callback cb) {
  napi_value result;

  NODE_API_CALL(
      napi_create_function(env, name, NAPI_AUTO_LENGTH, cb, NULL, &result));
  return result;
}
static inline char *toChar(napi_env env, napi_value value) {
  size_t length;

  NODE_API_CALL(napi_get_value_string_utf8(env, value, NULL, 0, &length));
  char *name = malloc(length + 1);
  NODE_API_CALL(napi_get_value_string_utf8(env, value, name, length + 1, NULL));
  return name;
}