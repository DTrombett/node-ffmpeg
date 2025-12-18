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

#define EXPORT_FN(fn, orig) NODE_SET_PROPERTY(exports, #orig, FUNCTION(fn));

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
  EXPORT_FN(versionInfo, av_version_info);
  EXPORT_FN(findEncoderByName, avcodec_find_encoder_by_name);
  EXPORT_FN(allocContext3, avcodec_alloc_context3);
  EXPORT_FN(packetAlloc, av_packet_alloc);
  EXPORT_FN(optSet, av_opt_set);
  EXPORT_FN(optSetInt, av_opt_set_int);
  EXPORT_FN(optSetDouble, av_opt_set_double);
  EXPORT_FN(optSetQ, av_opt_set_q);
  EXPORT_FN(open2, avcodec_open2);
  EXPORT_FN(frameAlloc, av_frame_alloc);
  EXPORT_FN(frameGetBuffer, av_frame_get_buffer);
  EXPORT_FN(frameMakeWritable, av_frame_make_writable);
  EXPORT_FN(sendFrame, avcodec_send_frame);
  EXPORT_FN(receivePacket, avcodec_receive_packet);
  EXPORT_FN(packetUnref, av_packet_unref);
  EXPORT_FN(err2str, av_err2str);
  EXPORT_FN(freeContext, avcodec_free_context);
  EXPORT_FN(frameFree, av_frame_free);
  EXPORT_FN(packetFree, av_packet_free);
  return exports;
}
