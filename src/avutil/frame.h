#ifndef FRAME_H
#define FRAME_H
#include "../utils.h"
#include "channelLayout.h"
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>

#define LOAD_SIZES(size, native)                                               \
  do {                                                                         \
    ptrdiff_t linesizes[AV_NUM_DATA_POINTERS];                                 \
    for (int i = 0; i < AV_NUM_DATA_POINTERS; i++)                             \
      linesizes[i] = native->linesize[i];                                      \
    av_image_fill_plane_sizes(size, native->format, native->height,            \
                              linesizes);                                      \
  } while (0)

static void finalizeFrame(napi_env env, void *finalize_data,
                          void *finalize_hint) {
  av_frame_free((AVFrame **)&finalize_data);
}

static napi_value get_frameData(napi_env env, napi_callback_info cbinfo) {
  napi_value object;
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, NULL, NULL, &object, NULL));
  AVFrame *native = unwrap(env, object);

  if (!native)
    return UNDEFINED;
  NODE_API_CALL(
      napi_create_array_with_length(env, AV_NUM_DATA_POINTERS, &object));
  if (native->nb_samples > 0) {
    for (int i = 0; i < AV_NUM_DATA_POINTERS; i++)
      NODE_API_CALL(napi_set_element(
          env, object, i,
          native->data[i]
              ? Uint8Array(env, native->linesize[0], native->data[i])
              : UNDEFINED));
  } else {
    size_t size[AV_NUM_DATA_POINTERS];
    LOAD_SIZES(size, native);
    for (int i = 0; i < AV_NUM_DATA_POINTERS; i++)
      NODE_API_CALL(napi_set_element(
          env, object, i,
          native->data[i] && size[i] ? Uint8Array(env, size[i], native->data[i])
                                     : UNDEFINED));
  }
  FREEZE(object);
  return object;
}
static napi_value set_frameData(napi_env env, napi_callback_info cbinfo) {
  napi_value argv[1];
  size_t argc = 1;
  napi_value object;
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, &argc, argv, &object, NULL));
  AVFrame *native = unwrap(env, object);

  if (!native)
    return UNDEFINED;
  for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) {
    napi_value value;

    NODE_API_CALL(napi_get_element(env, argv[0], i, &value));
    if (isTypedArray(env, value)) {
      void *dataPtr;
      napi_typedarray_type type;
      size_t length;

      NODE_API_CALL(napi_get_typedarray_info(env, value, &type, NULL, &dataPtr,
                                             NULL, NULL));
      if (type != napi_uint8_array) {
        napi_throw_type_error(env, NULL, "Expected Uint8Array for frame data");
        return UNDEFINED;
      }
      // TODO: use different map and create ref to uint8array
      mapDelete(native->data[i], 0);
      native->data[i] = dataPtr;
    } else
      native->data[i] = NULL;
  }
  return UNDEFINED;
}

static napi_value get_frameLinesize(napi_env env, napi_callback_info cbinfo) {
  napi_value object;
  NODE_API_CALL(napi_get_cb_info(env, cbinfo, NULL, NULL, &object, NULL));
  AVFrame *native = unwrap(env, object);

  if (!native)
    return UNDEFINED;
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
    return UNDEFINED;
  NODE_API_CALL(
      napi_create_array_with_length(env, AV_NUM_DATA_POINTERS, &object));
  if (native->nb_samples > 0)
    for (int i = 0;
         i < native->ch_layout.nb_channels && native->extended_data[i]; i++)
      NODE_API_CALL(napi_set_element(
          env, object, i,
          Uint8Array(env, native->linesize[0], native->extended_data[i])));
  else {
    size_t size[AV_NUM_DATA_POINTERS];
    LOAD_SIZES(size, native);
    for (int i = 0;
         i < AV_NUM_DATA_POINTERS && native->extended_data[i] && size[i]; i++)
      NODE_API_CALL(napi_set_element(
          env, object, i, Uint8Array(env, size[i], native->extended_data[i])));
  }
  FREEZE(object);
  return object;
}

WRAP(createAVFrame, AVFrame, finalizeFrame, PROP_GETSET(data, data, frameData),
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