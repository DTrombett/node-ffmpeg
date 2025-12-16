#define NAPI_VERSION 10
#include "avcodec/CodecContext.h"
#include "avcodec/codec.h"
#include "avcodec/frame.h"
#include "avcodec/packet.h"
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
  char *name = parseString(env, arguments[0]);
  const AVCodec *codec = avcodec_find_encoder_by_name(name);

  free(name);
  return createAVCodec(env, (AVCodec *)codec);
}
static napi_value allocContext3(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);

  return createAVCodecContext(
      env, avcodec_alloc_context3(unwrap(env, arguments[0])));
}
static napi_value packetAlloc(napi_env env, napi_callback_info cbinfo) {
  return createAVPacket(env, av_packet_alloc());
}
static napi_value optSet(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(4, cbinfo);
  char *val = parseString(env, arguments[2]);

  OPT_SET(, val)
  free(val);
  return NUMBER(ret);
}
static napi_value optSetInt(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(4, cbinfo);
  OPT_SET(_int, parseInt64(env, arguments[2], false, 0))
  return NUMBER(ret);
}
static napi_value optSetDouble(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(4, cbinfo);
  OPT_SET(_double, parseDouble(env, arguments[2], false, 0))
  return NUMBER(ret);
}
static napi_value optSetQ(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(4, cbinfo);
  AVRational val;

  parseAVRational(env, arguments[2], &val);
  OPT_SET(_q, val)
  return NUMBER(ret);
}
static napi_value open2(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(3, cbinfo);
  return NUMBER(avcodec_open2(parseExternal(env, arguments[0]),
                              parseExternal(env, arguments[1]),
                              parseExternal(env, arguments[2])));
}
static napi_value frameAlloc(napi_env env, napi_callback_info cbinfo) {
  return createAVFrame(env, av_frame_alloc());
}

NAPI_MODULE_INIT(/* napi_env env, napi_value exports */) {
  NODE_SET_PROPERTY(exports, "versionInfo", FUNCTION(versionInfo));
  NODE_SET_PROPERTY(exports, "findEncoderByName", FUNCTION(findEncoderByName));
  NODE_SET_PROPERTY(exports, "allocContext3", FUNCTION(allocContext3));
  NODE_SET_PROPERTY(exports, "packetAlloc", FUNCTION(packetAlloc));
  NODE_SET_PROPERTY(exports, "optSet", FUNCTION(optSet));
  NODE_SET_PROPERTY(exports, "optSetInt", FUNCTION(optSetInt));
  NODE_SET_PROPERTY(exports, "optSetDouble", FUNCTION(optSetDouble));
  NODE_SET_PROPERTY(exports, "optSetQ", FUNCTION(optSetQ));
  NODE_SET_PROPERTY(exports, "open2", FUNCTION(open2));
  NODE_SET_PROPERTY(exports, "frameAlloc", FUNCTION(frameAlloc));
  return exports;
}
