#ifndef UTILS_H
#define UTILS_H
#define NODE_API_CALL_DEFAULT(call, def)                                       \
  do {                                                                         \
    napi_status status = (call);                                               \
    if (status != napi_ok) {                                                   \
      const napi_extended_error_info *error_info = NULL;                       \
      napi_get_last_error_info(env, &error_info);                              \
      const char *err_message = error_info->error_message;                     \
      bool is_pending;                                                         \
      napi_is_exception_pending(env, &is_pending);                             \
      const char *message =                                                    \
          (err_message == NULL) ? "empty error message" : err_message;         \
      printf("%s\n^\nError: %s at %s (%s:%d)\n", #call, message, __func__,     \
             __FILE__, __LINE__);                                              \
      if (!is_pending)                                                         \
        napi_throw_error(env, NULL, message);                                  \
      return (def);                                                            \
    }                                                                          \
  } while (0)
#define NODE_API_CALL(call) NODE_API_CALL_DEFAULT(call, NULL)
#define NODE_SET_PROPERTY(object, name, value)                                 \
  NODE_API_CALL(napi_set_named_property(env, (object), (name), (value)))
#define NODE_LOAD_ARGUMENTS(count, cbinfo)                                     \
  napi_value arguments[(count)];                                               \
  {                                                                            \
    size_t argc = (count);                                                     \
    NODE_API_CALL(                                                             \
        napi_get_cb_info(env, (cbinfo), &argc, arguments, NULL, NULL));        \
  }
#define _GET_MACRO2(_1, _2, NAME, ...) NAME
#define _GET_MACRO3(_1, _2, _3, NAME, ...) NAME
#define STRING(str) String(env, (str))
#define NUMBER(value) Number(env, (value))
#define BIGINT64(value) BigInt64(env, (value))
#define BIGUINT64(value) BigUInt64(env, (value))
#define BIGINTWORDS(value) BigIntWords(env, (value))
#define FUNCTION(name) Function(env, #name, (name))
#define FREEZE(object) NODE_API_CALL(napi_object_freeze(env, (object)))
#define Array(...)                                                             \
  _GET_MACRO2(__VA_ARGS__, createArrayWithLength, createArray)(__VA_ARGS__)
#define EXTERNAL(data) External(env, (data), NULL, NULL)
#define DECLARE_PROP_ATTR(name, value, attributes)                             \
  {name, NULL, NULL, NULL, NULL, value, attributes, NULL}
#define DECLARE_PROP_NOATTR(name, value)                                       \
  {name, NULL, NULL, NULL, NULL, value, napi_default_jsproperty, NULL}
#define DECLARE_PROPERTY(...)                                                  \
  _GET_MACRO3(__VA_ARGS__, DECLARE_PROP_ATTR, DECLARE_PROP_NOATTR)(__VA_ARGS__)
#define PROP_CONST(name, value) DECLARE_PROP_ATTR(#name, value, napi_enumerable)
#define PROP_GETSET(name, prop, type)                                          \
  {#name,                                                                      \
   NULL,                                                                       \
   NULL,                                                                       \
   get_##type,                                                                 \
   set_##type,                                                                 \
   NULL,                                                                       \
   napi_enumerable | napi_writable,                                            \
   (void *)offsetof(Wrap, prop)}
#define PROP_GET(name, prop, type)                                             \
  {#name,                                                                      \
   NULL,                                                                       \
   NULL,                                                                       \
   get_##type,                                                                 \
   NULL,                                                                       \
   NULL,                                                                       \
   napi_enumerable,                                                            \
   (void *)offsetof(Wrap, prop)}
#define WRAP(name, type, finalize_cb, ...)                                     \
  static inline napi_value name(napi_env env, type *native) {                  \
    typedef type Wrap;                                                         \
    if (!native)                                                               \
      return NULL;                                                             \
    napi_ref ref = mapGet(native);                                             \
    napi_value object;                                                         \
    if (ref) {                                                                 \
      NODE_API_CALL(napi_get_reference_value(env, ref, &object));              \
      return object;                                                           \
    }                                                                          \
    object = Object(env);                                                      \
    napi_property_descriptor properties[] = {__VA_ARGS__};                     \
    NODE_API_CALL(napi_define_properties(                                      \
        env, object, sizeof(properties) / sizeof(properties[0]), properties)); \
    NODE_API_CALL(napi_wrap(env, object, (void *)native, mapFinalizeCb,        \
                            finalize_cb, &ref));                               \
    mapAdd(native, ref);                                                       \
    FREEZE(object);                                                            \
    return object;                                                             \
  }
#define LOAD_SET(cbinfo)                                                       \
  void *data;                                                                  \
  napi_value thisArg;                                                          \
  napi_value argv[1];                                                          \
  size_t argc = 1;                                                             \
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, &argc, argv, &thisArg, &data));  \
  if (!data || argv[0] == NULL)                                                \
    return NULL;                                                               \
  char *native = unwrap(env, thisArg);                                         \
  if (!native)                                                                 \
    return NULL;
#define LOAD_GET(cbinfo)                                                       \
  void *data;                                                                  \
  napi_value thisArg;                                                          \
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, NULL, NULL, &thisArg, &data));   \
  char *native = unwrap(env, thisArg);                                         \
  if (!native)                                                                 \
    return NULL;

#include "map.h"
#include <node_api.h>
#include <stdio.h>
#include <stdlib.h>

static inline napi_valuetype nodeTypeof(napi_env env, napi_value value) {
  napi_valuetype result;

  NODE_API_CALL_DEFAULT(napi_typeof(env, value, &result), napi_undefined);
  return result;
};
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
static inline napi_value BigInt64(napi_env env, int64_t value) {
  napi_value result;

  NODE_API_CALL(napi_create_bigint_int64(env, value, &result));
  return result;
}
static inline napi_value BigUInt64(napi_env env, uint64_t value) {
  napi_value result;

  NODE_API_CALL(napi_create_bigint_uint64(env, value, &result));
  return result;
}
static inline napi_value BigIntWords(napi_env env, int sign_bit,
                                     size_t word_count, const uint64_t *words) {
  napi_value result;

  NODE_API_CALL(
      napi_create_bigint_words(env, sign_bit, word_count, words, &result));
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
  if (!length)
    return NULL;
  char *name = malloc(length + 1);
  NODE_API_CALL(napi_get_value_string_utf8(env, value, name, length + 1, NULL));
  return name;
}
static inline void *unwrap(napi_env env, napi_value object) {
  void *result;

  NODE_API_CALL(napi_unwrap(env, object, &result));
  return result;
}
static inline napi_value External(napi_env env, void *data,
                                  napi_finalize finalize_cb,
                                  void *finalize_hint) {
  napi_value result;

  NODE_API_CALL(
      napi_create_external(env, data, finalize_cb, finalize_hint, &result));
  return result;
}

napi_value get_int(napi_env env, napi_callback_info cbinfo);
napi_value set_int(napi_env env, napi_callback_info cbinfo);
napi_value get_int64_t(napi_env env, napi_callback_info cbinfo);
napi_value set_int64_t(napi_env env, napi_callback_info cbinfo);
napi_value get_float(napi_env env, napi_callback_info cbinfo);
napi_value set_float(napi_env env, napi_callback_info cbinfo);
napi_value get_double(napi_env env, napi_callback_info cbinfo);
napi_value set_double(napi_env env, napi_callback_info cbinfo);
napi_value get_string(napi_env env, napi_callback_info cbinfo);
napi_value set_string(napi_env env, napi_callback_info cbinfo);
#endif
