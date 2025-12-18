#define NAPI_VERSION 10
#include "avcodec/CodecContext.h"
#include "avcodec/codec.h"
#include "avcodec/frame.h"
#include "avcodec/packet.h"
#include "avutil/rational.h"
#include "utils.h"
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <node_api.h>

#define EXPORT_FN(fn) NODE_SET_PROPERTY(exports, #fn, FUNCTION(fn));

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
  const AVCodec *codec = unwrap(env, arguments[0]);

  return createAVCodecContext(env, avcodec_alloc_context3(codec));
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
static napi_value frameGetBuffer(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(2, cbinfo);
  return NUMBER(av_frame_get_buffer(parseExternal(env, arguments[0]),
                                    parseInt(env, arguments[1], true, 0)));
}
static napi_value frameMakeWritable(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);
  return NUMBER(av_frame_make_writable(parseExternal(env, arguments[0])));
}
static napi_value sendFrame(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(2, cbinfo);
  return NUMBER(avcodec_send_frame(parseExternal(env, arguments[0]),
                                   parseExternal(env, arguments[1])));
}
static napi_value receivePacket(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(2, cbinfo);
  return NUMBER(avcodec_receive_packet(parseExternal(env, arguments[0]),
                                       parseExternal(env, arguments[1])));
}
static napi_value packetUnref(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);
  av_packet_unref(parseExternal(env, arguments[0]));
  return UNDEFINED;
}
static napi_value err2str(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);
  return STRING(av_err2str(parseInt(env, arguments[0], true, 0)));
}
static napi_value freeContext(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);
  AVCodecContext *ctx = parseExternal(env, arguments[0]);
  avcodec_free_context(&ctx);
  return UNDEFINED;
}
static napi_value frameFree(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);
  AVFrame *frame = parseExternal(env, arguments[0]);
  av_frame_free(&frame);
  return UNDEFINED;
}
static napi_value packetFree(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);
  AVPacket *pkt = parseExternal(env, arguments[0]);
  av_packet_free(&pkt);
  return UNDEFINED;
}

NAPI_MODULE_INIT(/* napi_env env, napi_value exports */) {
  EXPORT_FN(versionInfo);
  EXPORT_FN(findEncoderByName);
  EXPORT_FN(allocContext3);
  EXPORT_FN(packetAlloc);
  EXPORT_FN(optSet);
  EXPORT_FN(optSetInt);
  EXPORT_FN(optSetDouble);
  EXPORT_FN(optSetQ);
  EXPORT_FN(open2);
  EXPORT_FN(frameAlloc);
  EXPORT_FN(frameGetBuffer);
  EXPORT_FN(frameMakeWritable);
  EXPORT_FN(sendFrame);
  EXPORT_FN(receivePacket);
  EXPORT_FN(packetUnref);
  EXPORT_FN(err2str);
  EXPORT_FN(freeContext);
  EXPORT_FN(frameFree);
  EXPORT_FN(packetFree);
  return exports;
}
