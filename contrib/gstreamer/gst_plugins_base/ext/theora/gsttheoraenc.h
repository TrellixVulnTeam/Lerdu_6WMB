/* GStreamer
 * Copyright (C) 2004 Wim Taymans <wim@fluendo.com>
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

#ifndef __GST_THEORAENC_H__
#define __GST_THEORAENC_H__

#include <gst/gst.h>
#include <theora/theora.h>

G_BEGIN_DECLS

#define GST_TYPE_THEORA_ENC \
  (gst_theora_enc_get_type())
#define GST_THEORA_ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_THEORA_ENC,GstTheoraEnc))
#define GST_THEORA_ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_THEORA_ENC,GstTheoraEncClass))
#define GST_IS_THEORA_ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_THEORA_ENC))
#define GST_IS_THEORA_ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_THEORA_ENC))

typedef struct _GstTheoraEnc GstTheoraEnc;
typedef struct _GstTheoraEncClass GstTheoraEncClass;

/**
 * GstTheoraEncBorderMode:
 * @BORDER_NONE: no border
 * @BORDER_BLACK: black border
 * @BORDER_MIRROR: Mirror image in border
 *
 * Border color to add when sizes not multiple of 16. 
 */ 
typedef enum
{
  BORDER_NONE,
  BORDER_BLACK,
  BORDER_MIRROR
}
GstTheoraEncBorderMode;

/**
 * GstTheoraEnc:
 *
 * Opaque data structure.
 */
struct _GstTheoraEnc
{
  GstElement element;

  GstPad *sinkpad;
  GstPad *srcpad;

  ogg_stream_state to;

  theora_state state;
  theora_info info;
  theora_comment comment;
  gboolean initialised;

  gboolean center;
  GstTheoraEncBorderMode border;

  gint video_bitrate;           /* bitrate target for Theora video */
  gint video_quality;           /* Theora quality selector 0 = low, 63 = high */
  gboolean quick;
  gboolean keyframe_auto;
  gint keyframe_freq;
  gint keyframe_force;
  gint keyframe_threshold;
  gint keyframe_mindistance;
  gint noise_sensitivity;
  gint sharpness;

  gint info_width, info_height;
  gint width, height;
  gint offset_x, offset_y;
  gint fps_n, fps_d;
  GstClockTime next_ts;

  GstClockTime expected_ts;
  gboolean next_discont;

  guint packetno;
  guint64 bytes_out;
  guint64 granulepos_offset;
  guint64 timestamp_offset;
  gint granule_shift;  
};

struct _GstTheoraEncClass
{
  GstElementClass parent_class;
};

G_END_DECLS

#endif /* __GST_THEORAENC_H__ */

