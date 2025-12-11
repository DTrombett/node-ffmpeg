#ifndef UTILS_H
#define UTILS_H
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
#define DECLARE_PROP_ATTR(name, value, attributes)                             \
  {name, NULL, NULL, NULL, NULL, value, attributes, NULL}
#define DECLARE_PROP_NOATTR(name, value)                                       \
  {name, NULL, NULL, NULL, NULL, value, napi_default_jsproperty, NULL}
#define DECLARE_PROPERTY(...)                                                  \
  _GET_MACRO3(__VA_ARGS__, DECLARE_PROP_ATTR, DECLARE_PROP_NOATTR)(__VA_ARGS__)
#define PROP_CONST(name, value) DECLARE_PROP_ATTR(#name, value, napi_enumerable)
#define WRAP(name, type, finalize_cb, ...)                                     \
  static inline napi_value name(napi_env env, type *native) {                  \
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
  char *name = malloc(length + 1);
  NODE_API_CALL(napi_get_value_string_utf8(env, value, name, length + 1, NULL));
  return name;
}
static inline void *unwrap(napi_env env, napi_value object) {
  void *result;

  NODE_API_CALL(napi_unwrap(env, object, &result));
  return result;
}
#endif
