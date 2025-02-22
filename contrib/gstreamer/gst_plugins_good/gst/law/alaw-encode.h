/* GStreamer PCM to A-Law conversion
 * Copyright (C) 2000 by Abramo Bagnara <abramo@alsa-project.org>
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


#ifndef __GST_ALAW_ENCODE_H__
#define __GST_ALAW_ENCODE_H__

#include <gst/gst.h>
#include <gst/gst_global.h>
#include <gst/riff/riff-read.h>
#include <gst/riff/riff-ids.h>
#include <gst/riff/riff-media.h>
#include <gst/base/gstadapter.h>

G_BEGIN_DECLS

#define GST_TYPE_ALAW_ENC \
  (gst_alaw_enc_get_type())
#define GST_ALAW_ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ALAW_ENC,GstALawEnc))
#define GST_ALAW_ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_ALAW_ENC,GstALawEncClass))
#define GST_IS_ALAW_ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ALAW_ENC))
#define GST_IS_ALAW_ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_ALAW_ENC))

typedef struct _GstALawEnc GstALawEnc;
typedef struct _GstALawEncClass GstALawEncClass;

struct _GstALawEnc {
  GstElement element;

  GstPad *sinkpad,*srcpad;

  gint channels;
  gint rate;
};

struct _GstALawEncClass {
  GstElementClass parent_class;
};

GType gst_alaw_enc_get_type(void);

G_END_DECLS

#endif /* __GST_ALAW_ENCODE_H__ */
