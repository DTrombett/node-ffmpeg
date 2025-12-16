#ifndef FRAME_H
#define FRAME_H
#include "../utils.h"
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <node_api.h>

#define LOAD_SIZES(size, native)                                               \
  {                                                                            \
    ptrdiff_t linesizes[AV_NUM_DATA_POINTERS];                                 \
    for (int i = 0; i < AV_NUM_DATA_POINTERS; i++)                             \
      linesizes[i] = native->linesize[i];                                      \
    av_image_fill_plane_sizes(size, native->format, native->height,            \
                              linesizes);                                      \
  }
#define LOAD_VIDEO_FRAME_DATA(field)                                           \
  {                                                                            \
    size_t size[AV_NUM_DATA_POINTERS];                                         \
    LOAD_SIZES(size, native);                                                  \
    for (int i = 0; i < AV_NUM_DATA_POINTERS; i++)                             \
      if (native->field[i] && size[i])                                         \
        NODE_API_CALL(napi_set_element(                                        \
            env, object, i, ArrayBuffer(env, size[i], native->field[i])));     \
  }

static void frameFree(napi_env env, void *finalize_data, void *finalize_hint) {
  av_frame_free((AVFrame **)&finalize_data);
}

WRAP(createAVChannelLayout, AVChannelLayout, NULL,
     PROP_GETSET(order, order, int), PROP_GETSET(nbChannels, nb_channels, int),
     PROP_GETSET(mask, u.mask, int64_t), PROP_CONST(map, NULL));

static napi_value get_frameData(napi_env env, napi_callback_info cbinfo) {
  napi_value object;
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, NULL, NULL, &object, NULL));
  AVFrame *native = unwrap(env, object);

  if (!native)
    return NULL;
  NODE_API_CALL(
      napi_create_array_with_length(env, AV_NUM_DATA_POINTERS, &object));
  if (native->nb_samples > 0) {
    for (int i = 0; i < AV_NUM_DATA_POINTERS; i++)
      if (native->data[i])
        NODE_API_CALL(napi_set_element(
            env, object, i,
            ArrayBuffer(env, native->linesize[0], native->data[i])));
  } else
    LOAD_VIDEO_FRAME_DATA(data);
  FREEZE(object);
  return object;
}

static napi_value get_frameLinesize(napi_env env, napi_callback_info cbinfo) {
  napi_value object;
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, NULL, NULL, &object, NULL));
  AVFrame *native = unwrap(env, object);

  if (!native)
    return NULL;
  NODE_API_CALL(
      napi_create_array_with_length(env, AV_NUM_DATA_POINTERS, &object));
  for (int i = 0; i < AV_NUM_DATA_POINTERS; i++)
    NODE_API_CALL(
        napi_set_element(env, object, i, NUMBER(native->linesize[i])));
  FREEZE(object);
  return object;
}

static napi_value get_frameExtendedData(napi_env env,
                                        napi_callback_info cbinfo) {
  napi_value object;
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, NULL, NULL, &object, NULL));
  AVFrame *native = unwrap(env, object);

  if (!native)
    return NULL;
  NODE_API_CALL(
      napi_create_array_with_length(env, AV_NUM_DATA_POINTERS, &object));
  if (native->nb_samples > 0)
    for (int i = 0;
         i < native->ch_layout.nb_channels && native->extended_data[i]; i++)
      NODE_API_CALL(napi_set_element(
          env, object, i,
          ArrayBuffer(env, native->linesize[0], native->extended_data[i])));
  else
    LOAD_VIDEO_FRAME_DATA(extended_data);
  FREEZE(object);
  return object;
}

WRAP(createAVFrame, AVFrame, frameFree, PROP_GET(data, data, frameData),
     PROP_GET(linesize, linesize, frameLinesize),
     PROP_GET(extendedData, extended_data, frameExtendedData),
     PROP_GETSET(width, width, int), PROP_GETSET(height, height, int),
     PROP_GETSET(nbSamples, nb_samples, int), PROP_GETSET(format, format, int),
     PROP_GETSET(pictType, pict_type, int),
     PROP_GETSET(sampleAspectRatio, sample_aspect_ratio, AVRational),
     PROP_GETSET(pts, pts, int64_t), PROP_GETSET(pktDts, pkt_dts, int64_t),
     PROP_GETSET(timeBase, time_base, AVRational),
     PROP_GETSET(quality, quality, int),
     PROP_GETSET(repeatPict, repeat_pict, int),
     PROP_GETSET(sampleRate, sample_rate, int), PROP_CONST(hwFramesCtx, NULL),
     PROP_CONST(buf, NULL), PROP_CONST(extendedBuf, NULL),
     PROP_CONST(sideData, NULL), PROP_GETSET(flags, flags, int),
     PROP_GETSET(colorRange, color_range, int),
     PROP_GETSET(colorPrimaries, color_primaries, int),
     PROP_GETSET(colorTrc, color_trc, int),
     PROP_GETSET(colorspace, colorspace, int),
     PROP_GETSET(chromaLocation, chroma_location, int),
     PROP_GET(bestEffortTimestamp, best_effort_timestamp, int64_t),
     PROP_GET(metadata, metadata, External),
     PROP_GET(decodeErrorFlags, decode_error_flags, int),
     PROP_CONST(opaqueRef, NULL), PROP_GETSET(cropTop, crop_top, int64_t),
     PROP_GETSET(cropBottom, crop_bottom, int64_t),
     PROP_GETSET(cropLeft, crop_left, int64_t),
     PROP_GETSET(cropRight, crop_right, int64_t),
     PROP_CONST(chLayout, createAVChannelLayout(env, &native->ch_layout)),
     PROP_GETSET(duration, duration, int64_t));
#endif