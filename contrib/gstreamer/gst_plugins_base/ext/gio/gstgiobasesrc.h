/* GStreamer
 *
 * Copyright (C) 2007 Rene Stadler <mail@renestadler.de>
 * Copyright (C) 2007 Sebastian Dröge <slomo@circular-chaos.org>
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

#ifndef __GST_GIO_BASE_SRC_H__
#define __GST_GIO_BASE_SRC_H__

#include "gstgio.h"

#include <gst/base/gstbasesrc.h>

G_BEGIN_DECLS

#define GST_TYPE_GIO_BASE_SRC \
  (gst_gio_base_src_get_type())
#define GST_GIO_BASE_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GIO_BASE_SRC,GstGioBaseSrc))
#define GST_GIO_BASE_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_GIO_BASE_SRC,GstGioBaseSrcClass))
#define GST_IS_GIO_BASE_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GIO_BASE_SRC))
#define GST_IS_GIO_BASE_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_GIO_BASE_SRC))

typedef struct _GstGioBaseSrc      GstGioBaseSrc;
typedef struct _GstGioBaseSrcClass GstGioBaseSrcClass;

struct _GstGioBaseSrc
{
  GstBaseSrc src;
  
  GCancellable *cancel;
  guint64 position;
  GInputStream *stream;
};

struct _GstGioBaseSrcClass 
{
  GstBaseSrcClass parent_class;
};

GType gst_gio_base_src_get_type (void);

void gst_gio_base_src_set_stream (GstGioBaseSrc *src, GInputStream *stream);

G_END_DECLS

#endif /* __GST_GIO_BASE_SRC_H__ */
