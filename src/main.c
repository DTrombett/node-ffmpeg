#include "main.h"
#include <libavcodec/codec.h>
#include <libavcodec/defs.h>
#include <libavutil/avutil.h>
#include <node_api.h>

static void finalize_cb(napi_env env, void *finalize_data,
                        void *finalize_hint) {
  free(finalize_data);
}

static inline napi_value createAVProfile(napi_env env,
                                         const AVProfile *profile) {
  if (!profile)
    return NULL;
  napi_value object = Object(env);

  NODE_API_CALL(napi_define_properties(
      env, object, 2,
      (napi_property_descriptor[]){
          {"profile", NULL, NULL, NULL, NULL, NODE_NUMBER(profile->profile),
           napi_enumerable, NULL},
          {"name", NULL, NULL, NULL, NULL, NODE_STRING(profile->name),
           napi_enumerable, NULL},
      }));
  FREEZE(object);
  return object;
}
static inline napi_value createAVCodec(napi_env env, AVCodec *codec) {
  if (!codec)
    return NULL;
  napi_value object = Object(env);
  napi_property_descriptor properties[] = {
      {"name", NULL, NULL, NULL, NULL, NODE_STRING(codec->name),
       napi_enumerable, NULL},
      {"longName", NULL, NULL, NULL, NULL, NODE_STRING(codec->long_name),
       napi_enumerable, NULL},
      {"type", NULL, NULL, NULL, NULL, NODE_NUMBER(codec->type),
       napi_enumerable, NULL},
      {"id", NULL, NULL, NULL, NULL, NODE_NUMBER(codec->id), napi_enumerable,
       NULL},
      {"capabilities", NULL, NULL, NULL, NULL, NODE_NUMBER(codec->capabilities),
       napi_enumerable, NULL},
      {"maxLowres", NULL, NULL, NULL, NULL, NODE_NUMBER(codec->max_lowres),
       napi_enumerable, NULL},
      {"wrapperName", NULL, NULL, NULL, NULL, NODE_STRING(codec->wrapper_name),
       napi_enumerable, NULL},
      {"profiles", NULL, NULL, NULL, NULL, NULL, napi_enumerable, NULL},
  };
  if (codec->profiles) {
    napi_value profiles = Array(env);

    for (int i = 0; codec->profiles[i].profile != AV_PROFILE_UNKNOWN; i++)
      NODE_API_CALL(napi_set_element(
          env, profiles, i, createAVProfile(env, &codec->profiles[i])));
    FREEZE(profiles);
    properties[7].value = profiles;
  }
  NODE_API_CALL(napi_define_properties(
      env, object, sizeof(properties) / sizeof(properties[0]), properties));
  NODE_API_CALL(napi_wrap(env, object, (void *)codec, finalize_cb, NULL, NULL));
  FREEZE(object);
  return object;
}

static napi_value versionInfo(napi_env env, napi_callback_info cbinfo) {
  return NODE_STRING(av_version_info());
}
static napi_value findEncoderByName(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);
  char *name = toChar(env, arguments[0]);
  const AVCodec *codec = avcodec_find_encoder_by_name(name);

  free(name);
  return createAVCodec(env, (AVCodec *)codec);
}

NAPI_MODULE_INIT(/* napi_env env, napi_value exports */) {
  NODE_SET_PROPERTY(exports, "versionInfo", NODE_FUNCTION(versionInfo));
  NODE_SET_PROPERTY(exports, "findEncoderByName",
                    NODE_FUNCTION(findEncoderByName));
  return exports;
}
