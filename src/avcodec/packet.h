#ifndef PACKET_H
#define PACKET_H
#include "../utils.h"
#include <libavcodec/defs.h>
#include <libavcodec/packet.h>
#include <node_api.h>

static void packetFree(napi_env env, void *finalize_data, void *finalize_hint) {
  av_packet_free((AVPacket **)&finalize_data);
}
static napi_value get_data(napi_env env, napi_callback_info cbinfo) {
  napi_value object;
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, NULL, NULL, &object, NULL));
  AVPacket *native = unwrap(env, object);

  if (!native)
    return NULL;
  NODE_API_CALL(napi_create_external_arraybuffer(
      env, native->data, native->size, NULL, NULL, &object));
  return object;
}

WRAP(createAVPacket, AVPacket, packetFree, PROP_CONST(buf, NULL),
     PROP_GETSET(pts, pts, int64_t), PROP_GETSET(dts, dts, int64_t),
     PROP_GET(data, data, data), PROP_GETSET(size, size, int),
     PROP_GETSET(streamIndex, stream_index, int),
     PROP_GETSET(flags, flags, int), PROP_CONST(sideData, NULL),
     PROP_GETSET(duration, duration, int64_t), PROP_GETSET(pos, pos, int64_t),
     PROP_GETSET(timeBase, time_base, AVRational));
#endif