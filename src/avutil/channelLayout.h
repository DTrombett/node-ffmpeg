#ifndef CHANNEL_LAYOUT_H
#define CHANNEL_LAYOUT_H
#include "../utils.h"
#include <libavutil/channel_layout.h>

WRAP(createAVChannelCustom, AVChannelCustom, NULL, PROP_GETSET(id, id, int),
     PROP_GETSET(name, name, string));

static napi_value get_channelLayoutMap(napi_env env,
                                       napi_callback_info cbinfo) {
  napi_value object;
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, NULL, NULL, &object, NULL));
  AVChannelLayout *native = unwrap(env, object);

  if (!native)
    return UNDEFINED;
  NODE_API_CALL(
      napi_create_array_with_length(env, native->nb_channels, &object));
  for (int i = 0; i < native->nb_channels; i++)
    NODE_API_CALL(napi_set_element(
        env, object, i, createAVChannelCustom(env, &native->u.map[i])));
  FREEZE(object);
  return object;
}
static napi_value set_channelLayoutMap(napi_env env,
                                       napi_callback_info cbinfo) {
  LOAD_SET(cbinfo, AVChannelCustom *);
  LOOP_ARRAY(argv[0]) {
    napi_value object, value;

    NODE_API_CALL(napi_get_element(env, argv[0], i, &object));
    NODE_API_CALL(napi_get_named_property(env, object, "id", &value));
    (*data)[i].id = parseInt(env, value, true, 0);
    NODE_API_CALL(napi_get_named_property(env, object, "name", &value));
    if (nodeTypeof(env, value) == napi_string)
      NODE_API_CALL(napi_get_value_string_utf8(env, value, (*data)[i].name,
                                               sizeof((*data)[i].name), NULL));
  }
  return UNDEFINED;
}

WRAP(createAVChannelLayout, AVChannelLayout, NULL,
     PROP_GETSET(order, order, int), PROP_GETSET(nbChannels, nb_channels, int),
     PROP_GETSET(mask, u.mask, int64_t),
     PROP_GETSET(map, u.map, channelLayoutMap));
#endif