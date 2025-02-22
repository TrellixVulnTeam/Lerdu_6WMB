/* GStreamer
 * Copyright (C) 2006 Nokia <stefan.kost@nokia.com
 *
 * videoorientation.h: video flipping and centering interface
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

#ifndef __GST_VIDEO_ORIENTATION_H__
#define __GST_VIDEO_ORIENTATION_H__

#include <gst/gst.h>
#include <gst/interfaces/interfaces-enumtypes.h>

G_BEGIN_DECLS

#define GST_TYPE_VIDEO_ORIENTATION \
  (gst_video_orientation_get_type ())
#define GST_VIDEO_ORIENTATION(obj) \
  (GST_IMPLEMENTS_INTERFACE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_VIDEO_ORIENTATION, GstVideoOrientation))
#define GST_IS_VIDEO_ORIENTATION(obj) \
  (GST_IMPLEMENTS_INTERFACE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_VIDEO_ORIENTATION))
#define GST_VIDEO_ORIENTATION_GET_IFACE(inst) \
  (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GST_TYPE_VIDEO_ORIENTATION, GstVideoOrientationInterface))

/**
 * GstVideoOrientation:
 *
 * Opaque #GstVideoOrientation data structure.
 */
typedef struct _GstVideoOrientation GstVideoOrientation;

/**
 * GstVideoOrientationInterface:
 * @parent: parent interface type.
 * @get_hflip: virtual method to get horizontal flipping state
 * @get_vflip: virtual method to get vertical flipping state
 * @get_hcenter: virtual method to get horizontal centering state
 * @get_vcenter: virtual method to get vertical centering state
 * @set_hflip: virtual method to set horizontal flipping state
 * @set_vflip: virtual method to set vertical flipping state
 * @set_hcenter: virtual method to set horizontal centering state
 * @set_vcenter: virtual method to set vertical centering state
 *
 * #GstVideoOrientationInterface interface.
 */
typedef struct _GstVideoOrientationInterface {
  GTypeInterface parent;

  /* virtual functions */
  gboolean (* get_hflip)   (GstVideoOrientation *video_orientation, gboolean *flip);
  gboolean (* get_vflip)   (GstVideoOrientation *video_orientation, gboolean *flip);
  gboolean (* get_hcenter) (GstVideoOrientation *video_orientation, gint *center);
  gboolean (* get_vcenter) (GstVideoOrientation *video_orientation, gint *center);

  gboolean (* set_hflip)   (GstVideoOrientation *video_orientation, gboolean flip);
  gboolean (* set_vflip)   (GstVideoOrientation *video_orientation, gboolean flip);
  gboolean (* set_hcenter) (GstVideoOrientation *video_orientation, gint center);
  gboolean (* set_vcenter) (GstVideoOrientation *video_orientation, gint center);

  /*< private > */
  gpointer _gst_reserved[GST_PADDING];
} GstVideoOrientationInterface;
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GType           gst_video_orientation_get_type              (void);

/* virtual class function wrappers */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean gst_video_orientation_get_hflip (GstVideoOrientation *video_orientation, gboolean *flip);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean gst_video_orientation_get_vflip (GstVideoOrientation *video_orientation, gboolean *flip);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean gst_video_orientation_get_hcenter (GstVideoOrientation *video_orientation, gint *center);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean gst_video_orientation_get_vcenter (GstVideoOrientation *video_orientation, gint *center);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean gst_video_orientation_set_hflip (GstVideoOrientation *video_orientation, gboolean flip);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean gst_video_orientation_set_vflip (GstVideoOrientation *video_orientation, gboolean flip);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean gst_video_orientation_set_hcenter (GstVideoOrientation *video_orientation, gint center);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean gst_video_orientation_set_vcenter (GstVideoOrientation *video_orientation, gint center);

G_END_DECLS

#endif /* __GST_VIDEO_ORIENTATION_H__ */
