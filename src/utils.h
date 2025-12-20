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
      if (!is_pending) {                                                       \
        const char *message =                                                  \
            err_message ? err_message : "empty error message";                 \
        int len = snprintf(NULL, 0, "%s\n    at %s (%s:%d)", message,          \
                           __func__, __FILE__, __LINE__) +                     \
                  1;                                                           \
        char *buf = malloc(len);                                               \
        if (buf) {                                                             \
          snprintf(buf, len, "%s\n    at %s (%s:%d)", message, __func__,       \
                   __FILE__, __LINE__);                                        \
          message = buf;                                                       \
        }                                                                      \
        napi_throw_error(env, NULL, message);                                  \
        free(buf);                                                             \
      }                                                                        \
      return def;                                                              \
    }                                                                          \
  } while (0)
#define NODE_API_CALL(call) NODE_API_CALL_DEFAULT(call, UNDEFINED)
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
#define SEAL(object) NODE_API_CALL(napi_object_seal(env, (object)))
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
   &native->prop}
#define PROP_GET(name, prop, type)                                             \
  {#name, NULL, NULL, get_##type, NULL, NULL, napi_enumerable, &native->prop}
#define WRAP(name, type, finalize_cb, ...)                                     \
  static inline napi_value name(napi_env env, type *native) {                  \
    typedef type Wrap;                                                         \
    if (!native)                                                               \
      return UNDEFINED;                                                        \
    napi_ref ref = mapGet((void *)native, sizeof(type));                       \
    napi_value object;                                                         \
    if (ref) {                                                                 \
      NODE_API_CALL(napi_get_reference_value(env, ref, &object));              \
      if (object)                                                              \
        return object;                                                         \
      mapDelete((void *)native, sizeof(type));                                 \
    }                                                                          \
    object = Object(env);                                                      \
    napi_property_descriptor properties[] = {__VA_ARGS__};                     \
    NODE_API_CALL(napi_define_properties(                                      \
        env, object, sizeof(properties) / sizeof(properties[0]), properties)); \
    MapEntry *entry = mapAdd((void *)native, sizeof(type), NULL);              \
    if (!entry) {                                                              \
      napi_throw_error(env, NULL, "Failed to allocate map entry");             \
      return NULL;                                                             \
    }                                                                          \
    NODE_API_CALL(napi_wrap(env, object, &entry->key, mapFinalizeCb,           \
                            finalize_cb, &ref));                               \
    entry->value = ref;                                                        \
    SEAL(object);                                                              \
    return object;                                                             \
  }
#define LOAD_SET(cbinfo, type)                                                 \
  type *data;                                                                  \
  napi_value argv[1];                                                          \
  size_t argc = 1;                                                             \
  NODE_API_CALL(                                                               \
      napi_get_cb_info(env, cbinfo, &argc, argv, NULL, (void **)&data));       \
  if (!data || !argv[0])                                                       \
    return UNDEFINED;
#define LOAD_GET(cbinfo, type)                                                 \
  type *data;                                                                  \
  NODE_API_CALL(                                                               \
      napi_get_cb_info(env, cbinfo, NULL, NULL, NULL, (void **)&data));
#define OPT_SET(func, ...)                                                     \
  char *name = parseString(env, arguments[1]);                                 \
  int ret =                                                                    \
      av_opt_set##func(parseExternal(env, arguments[0]), name, __VA_ARGS__,    \
                       parseInt(env, arguments[3], true, 0));                  \
  free(name);
#define UNDEFINED undefined(env)
#define EXPORT_FN(fn, orig) NODE_SET_PROPERTY(exports, #orig, FUNCTION(fn));
#define LOOP_ARRAY(array)                                                      \
  uint32_t length;                                                             \
  NODE_API_CALL(napi_get_array_length(env, array, &length));                   \
  for (uint32_t i = 0; i < length; i++)

#include "map.h"

static inline napi_value undefined(napi_env env) {
  napi_value result;

  NODE_API_CALL(napi_get_undefined(env, &result));
  return result;
}
static inline napi_valuetype nodeTypeof(napi_env env, napi_value value) {
  napi_valuetype result;

  NODE_API_CALL_DEFAULT(napi_typeof(env, value, &result), napi_undefined);
  return result;
};
static inline bool isArray(napi_env env, napi_value value) {
  bool result;

  NODE_API_CALL_DEFAULT(napi_is_array(env, value, &result), 0);
  return result;
};
static inline bool isTypedArray(napi_env env, napi_value value) {
  if (nodeTypeof(env, value) != napi_object)
    return false;
  bool result;

  NODE_API_CALL_DEFAULT(napi_is_typedarray(env, value, &result), 0);
  return result;
};
static inline napi_value String(napi_env env, const char *str) {
  if (!str)
    return UNDEFINED;
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
static inline char *parseString(napi_env env, napi_value value) {
  size_t length;

  if (nodeTypeof(env, value) != napi_string)
    return NULL;
  NODE_API_CALL_DEFAULT(
      napi_get_value_string_utf8(env, value, NULL, 0, &length), NULL);
  if (!length)
    return NULL;
  char *name = malloc(length + 1);
  if (!name) {
    napi_throw_error(env, NULL, "Failed to allocate memory for string");
    return NULL;
  }
  NODE_API_CALL_DEFAULT(
      napi_get_value_string_utf8(env, value, name, length + 1, NULL), NULL);
  return name;
}
static inline int parseInt(napi_env env, napi_value value, bool ignoreNaN,
                           int defaultValue) {
  int result;

  if (ignoreNaN && nodeTypeof(env, value) != napi_number)
    return defaultValue;
  NODE_API_CALL_DEFAULT(napi_get_value_int32(env, value, &result),
                        defaultValue);
  return result;
}
static inline int64_t parseInt64(napi_env env, napi_value value, bool ignoreNaN,
                                 int64_t defaultValue) {
  int64_t result;

  if (ignoreNaN && nodeTypeof(env, value) != napi_number)
    return defaultValue;
  NODE_API_CALL_DEFAULT(napi_get_value_int64(env, value, &result),
                        defaultValue);
  return result;
}
static inline double parseDouble(napi_env env, napi_value value, bool ignoreNaN,
                                 double defaultValue) {
  double result;

  if (ignoreNaN && nodeTypeof(env, value) != napi_number)
    return defaultValue;
  NODE_API_CALL_DEFAULT(napi_get_value_double(env, value, &result),
                        defaultValue);
  return result;
}
static inline void *unwrap(napi_env env, napi_value object) {
  void *result;
  napi_status status = napi_unwrap(env, object, &result);

  if (status != napi_ok) {
    napi_get_and_clear_last_exception(env, result);
    return NULL;
  }
  return ((MapKey *)(result))->ptr;
}
static inline napi_value External(napi_env env, void *data,
                                  napi_finalize finalize_cb,
                                  void *finalize_hint) {
  napi_ref ref = mapGet(data, sizeof(data));
  napi_value value;

  if (ref) {
    NODE_API_CALL(napi_get_reference_value(env, ref, &value));
    if (value)
      return value;
    mapDelete(data, sizeof(data));
  }
  NODE_API_CALL(
      napi_create_external(env, data, finalize_cb, finalize_hint, &value));
  MapEntry *entry = mapAdd(data, sizeof(data), NULL);
  if (!entry) {
    napi_throw_error(env, NULL, "Failed to allocate map entry");
    return NULL;
  }
  NODE_API_CALL(
      napi_add_finalizer(env, value, &entry->key, mapFinalizeCb, NULL, &ref));
  entry->value = ref;
  return value;
}
static inline void *parseExternal(napi_env env, napi_value value) {
  if (!value)
    return NULL;
  void *result = NULL;
  napi_valuetype type = nodeTypeof(env, value);

  if (type == napi_external)
    NODE_API_CALL(napi_get_value_external(env, value, &result));
  else if (type == napi_object) {
    napi_status status = napi_unwrap(env, value, &result);

    if (status != napi_ok) {
      napi_get_and_clear_last_exception(env, result);
      return NULL;
    }
    result = ((MapKey *)(result))->ptr;
  }
  return result;
}
static inline napi_value Uint8Array(napi_env env, size_t size,
                                    void *external_data) {
  napi_ref ref = mapGet(external_data, size);
  napi_value value;

  if (ref) {
    NODE_API_CALL(napi_get_reference_value(env, ref, &value));
    if (value)
      return value;
    mapDelete(external_data, size);
  }
  NODE_API_CALL(napi_create_external_arraybuffer(env, external_data, size, NULL,
                                                 NULL, &value));
  NODE_API_CALL(
      napi_create_typedarray(env, napi_uint8_array, size, value, 0, &value));
  MapEntry *entry = mapAdd(external_data, size, NULL);
  if (!entry) {
    napi_throw_error(env, NULL, "Failed to allocate map entry");
    return NULL;
  }
  NODE_API_CALL(
      napi_add_finalizer(env, value, &entry->key, mapFinalizeCb, NULL, &ref));
  entry->value = ref;
  return value;
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
napi_value get_AVRational(napi_env env, napi_callback_info cbinfo);
napi_value set_AVRational(napi_env env, napi_callback_info cbinfo);
napi_value get_External(napi_env env, napi_callback_info cbinfo);
#endif
