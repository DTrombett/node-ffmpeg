#define NAPI_VERSION 10
#include "avutil/frame.h"
#include "avutil/rational.h"
#include "utils.h"
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <node_api.h>

static napi_value versionInfo(napi_env env, napi_callback_info cbinfo) {
  return STRING(av_version_info());
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
static napi_value err2str(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);
  return STRING(av_err2str(parseInt(env, arguments[0], true, 0)));
}
static napi_value frameFree(napi_env env, napi_callback_info cbinfo) {
  NODE_LOAD_ARGUMENTS(1, cbinfo);
  AVFrame *frame = parseExternal(env, arguments[0]);
  av_frame_free(&frame);
  return UNDEFINED;
}

NAPI_MODULE_INIT(/* napi_env env, napi_value exports */) {
  EXPORT_FN(versionInfo, av_version_info);
  EXPORT_FN(optSet, av_opt_set);
  EXPORT_FN(optSetInt, av_opt_set_int);
  EXPORT_FN(optSetDouble, av_opt_set_double);
  EXPORT_FN(optSetQ, av_opt_set_q);
  EXPORT_FN(frameAlloc, av_frame_alloc);
  EXPORT_FN(frameGetBuffer, av_frame_get_buffer);
  EXPORT_FN(frameMakeWritable, av_frame_make_writable);
  EXPORT_FN(err2str, av_err2str);
  EXPORT_FN(frameFree, av_frame_free);
  return exports;
}
