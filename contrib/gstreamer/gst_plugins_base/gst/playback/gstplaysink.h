/* GStreamer
 * Copyright (C) <2007> Wim Taymans <wim.taymans@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
#ifndef __GST_PLAY_SINK_H__
#define __GST_PLAY_SINK_H__

#include <gst/gst.h>

#include "gstplay-enum.h"

G_BEGIN_DECLS

#define GST_TYPE_PLAY_SINK \
  (gst_play_sink_get_type())
#define GST_PLAY_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_PLAY_SINK, GstPlaySink))
#define GST_PLAY_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_PLAY_SINK, GstPlaySinkClass))
#define GST_IS_PLAY_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_PLAY_SINK))
#define GST_IS_PLAY_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_PLAY_SINK))

/**
 * GstPlaySinkType:
 * @GST_PLAY_SINK_TYPE_AUDIO: A non-raw audio pad
 * @GST_PLAY_SINK_TYPE_AUDIO_RAW: a raw audio pad
 * @GST_PLAY_SINK_TYPE_VIDEO: a non-raw video pad
 * @GST_PLAY_SINK_TYPE_VIDEO_RAW: a raw video pad
 * @GST_PLAY_SINK_TYPE_TEXT: a raw text pad
 * @GST_PLAY_SINK_TYPE_LAST: the last type
 *
 * Types of pads that can be requested from the sinks.
 */
typedef enum {
  GST_PLAY_SINK_TYPE_AUDIO     = 0,
  GST_PLAY_SINK_TYPE_AUDIO_RAW = 1,
  GST_PLAY_SINK_TYPE_VIDEO     = 2,
  GST_PLAY_SINK_TYPE_VIDEO_RAW = 3,
  GST_PLAY_SINK_TYPE_TEXT      = 4,
  GST_PLAY_SINK_TYPE_LAST      = 5
} GstPlaySinkType;

typedef struct _GstPlaySink GstPlaySink;
typedef struct _GstPlaySinkClass GstPlaySinkClass;
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GType gst_play_sink_get_type (void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstPad *         gst_play_sink_request_pad    (GstPlaySink *playsink, GstPlaySinkType type);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void             gst_play_sink_release_pad    (GstPlaySink *playsink, GstPad *pad);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void             gst_play_sink_set_video_sink (GstPlaySink * playsink, GstElement * sink);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void             gst_play_sink_set_audio_sink (GstPlaySink * playsink, GstElement * sink);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void             gst_play_sink_set_vis_plugin (GstPlaySink * playsink, GstElement * vis);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void             gst_play_sink_set_volume     (GstPlaySink *playsink, gdouble volume);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gdouble          gst_play_sink_get_volume     (GstPlaySink *playsink);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void             gst_play_sink_set_mute       (GstPlaySink *playsink, gboolean mute);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean         gst_play_sink_get_mute       (GstPlaySink *playsink);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean         gst_play_sink_set_flags      (GstPlaySink * playsink, GstPlayFlags flags);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstPlayFlags     gst_play_sink_get_flags      (GstPlaySink * playsink);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstBuffer *      gst_play_sink_get_last_frame (GstPlaySink * playsink);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean         gst_play_sink_reconfigure    (GstPlaySink * playsink);

G_END_DECLS

#endif /* __GST_PLAY_SINK_H__ */
