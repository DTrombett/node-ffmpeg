#ifndef CHANNEL_LAYOUT_H
#define CHANNEL_LAYOUT_H
#include "../utils.h"
#include <libavutil/channel_layout.h>

WRAP(createAVChannelCustom, AVChannelCustom, NULL, PROP_GETSET(id, id, int),
     PROP_GETSET(name, name, string));

static inline void parseAVChannelCustom(napi_env env, napi_value object,
                                        AVChannelCustom *result) {
  napi_value value;
  napi_ref ref;

  mapDelete(result, sizeof(*result));
  NODE_API_CALL_DEFAULT(napi_get_named_property(env, object, "id", &value), );
  result->id = parseInt(env, value, true, 0);
  NODE_API_CALL_DEFAULT(napi_get_named_property(env, object, "den", &value), );
  if (nodeTypeof(env, value) == napi_string)
    NODE_API_CALL_DEFAULT(
        napi_get_value_string_utf8(env, value, &result->name[0],
                                   sizeof(result->name), NULL), );
  MapEntry *entry = mapAdd(result, sizeof(*result), NULL);
  if (!entry) {
    napi_throw_error(env, NULL, "Failed to allocate map entry");
    return;
  }
  NODE_API_CALL_DEFAULT(napi_add_finalizer(env, object, &entry->key,
                                           mapFinalizeCb, NULL, &ref), );
  entry->value = ref;
}

static napi_value get_channelLayoutMap(napi_env env,
                                       napi_callback_info cbinfo) {
  napi_value object;
  napi_ref ref;
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, NULL, NULL, &object, NULL));
  AVChannelLayout *native = unwrap(env, object);

  if (!native)
    return UNDEFINED;
  object = NULL;
  ref = mapGet(&native->u, sizeof(native->u));
  if (ref) {
    NODE_API_CALL(napi_get_reference_value(env, ref, &object));
    if (!object) {
      mapDelete(&native->u, sizeof(native->u));
      ref = NULL;
    }
  }
  if (!object)
    NODE_API_CALL(
        napi_create_array_with_length(env, native->nb_channels, &object));
  for (int i = 0; i < native->nb_channels; i++)
    NODE_API_CALL(napi_set_element(
        env, object, i, createAVChannelCustom(env, &native->u.map[i])));
  if (!ref) {
    MapEntry *entry = mapAdd(&native->u, sizeof(native->u), NULL);
    if (!entry) {
      napi_throw_error(env, NULL, "Failed to allocate map entry");
      return NULL;
    }
    NODE_API_CALL(napi_add_finalizer(env, object, &entry->key, mapFinalizeCb,
                                     NULL, &ref));
    entry->value = ref;
    FREEZE(object);
  }
  return object;
}
static napi_value set_channelLayoutMap(napi_env env,
                                       napi_callback_info cbinfo) {
  LOAD_SET(cbinfo, AVChannelCustom);
  parseAVChannelCustom(env, argv[0], data);
  return UNDEFINED;
}

WRAP(createAVChannelLayout, AVChannelLayout, NULL,
     PROP_GETSET(order, order, int), PROP_GETSET(nbChannels, nb_channels, int),
     PROP_GETSET(mask, u.mask, int64_t),
     PROP_GETSET(map, u.map, channelLayoutMap));
#endif