#define NAPI_VERSION 10
#include "avcodec/CodecContext.h"
#include "avcodec/codec.h"
#include "utils.h"
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <node_api.h>

static void finalize_cb(napi_env env, void *finalize_data,
                        void *finalize_hint) {
  free(finalize_data);
}
static napi_value versionInfo(napi_env env, napi_callback_info cbinfo) {
  return STRING(av_version_info());
}
static napi_value findEncoderByName(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);
  char *name = toChar(env, arguments[0]);
  const AVCodec *codec = avcodec_find_encoder_by_name(name);

  free(name);
  return createAVCodec(env, (AVCodec *)codec);
}
static napi_value allocContext3(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);

  return createAVCodecContext(
      env, avcodec_alloc_context3(unwrap(env, arguments[0])));
}

NAPI_MODULE_INIT(/* napi_env env, napi_value exports */) {
  NODE_SET_PROPERTY(exports, "versionInfo", FUNCTION(versionInfo));
  NODE_SET_PROPERTY(exports, "findEncoderByName", FUNCTION(findEncoderByName));
  NODE_SET_PROPERTY(exports, "allocContext3", FUNCTION(allocContext3));
  return exports;
}
