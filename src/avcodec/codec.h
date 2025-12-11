#ifndef CODEC_H
#define CODEC_H
#include "../utils.h"
#include <libavcodec/codec.h>
#include <libavcodec/defs.h>
#include <node_api.h>

WRAP(createAVProfile, const AVProfile *, NULL,
     PROP_CONST(profile, NUMBER(native->profile)),
     PROP_CONST(name, STRING(native->name)))

static inline napi_value createProfiles(napi_env env,
                                        const AVProfile *profiles) {
  if (!profiles)
    return NULL;
  napi_value array = Array(env);

  for (int i = 0; profiles[i].profile != AV_PROFILE_UNKNOWN; i++)
    NODE_API_CALL(
        napi_set_element(env, array, i, createAVProfile(env, &profiles[i])));
  FREEZE(array);
  return array;
}

WRAP(createAVCodec, AVCodec *, NULL, PROP_CONST(name, STRING(native->name)),
     PROP_CONST(longName, STRING(native->long_name)),
     PROP_CONST(type, NUMBER(native->type)), PROP_CONST(id, NUMBER(native->id)),
     PROP_CONST(capabilities, NUMBER(native->capabilities)),
     PROP_CONST(maxLowres, NUMBER(native->max_lowres)),
     PROP_CONST(wrapperName, STRING(native->wrapper_name)),
     PROP_CONST(profiles, createProfiles(env, native->profiles)))
#endif