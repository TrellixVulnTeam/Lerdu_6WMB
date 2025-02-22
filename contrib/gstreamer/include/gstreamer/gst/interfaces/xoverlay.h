/* GStreamer X-based Overlay
 * Copyright (C) 2003 Ronald Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2003 Julien Moutte <julien@moutte.net>
 *
 * x-overlay.h: X-based overlay interface design
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

#ifndef __GST_X_OVERLAY_H__
#define __GST_X_OVERLAY_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_X_OVERLAY \
  (gst_x_overlay_get_type ())
#define GST_X_OVERLAY(obj) \
  (GST_IMPLEMENTS_INTERFACE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_X_OVERLAY, \
                                                 GstXOverlay))
#define GST_X_OVERLAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_X_OVERLAY, GstXOverlayClass))
#define GST_IS_X_OVERLAY(obj) \
  (GST_IMPLEMENTS_INTERFACE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_X_OVERLAY))
#define GST_IS_X_OVERLAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_X_OVERLAY))
#define GST_X_OVERLAY_GET_CLASS(inst) \
  (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GST_TYPE_X_OVERLAY, GstXOverlayClass))

typedef struct _GstXOverlay GstXOverlay;

typedef struct _GstXOverlayClass {
  GTypeInterface klass;

  /* virtual functions */
  void (* set_xwindow_id) (GstXOverlay *overlay,
                           gulong       xwindow_id);

  void (* expose)         (GstXOverlay *overlay);
  
  void (* handle_events)  (GstXOverlay *overlay,
                           gboolean     handle_events);  

  /*< private >*/
  gpointer                 _gst_reserved[GST_PADDING - 1];
} GstXOverlayClass;
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GType   gst_x_overlay_get_type          (void);

/* virtual class function wrappers */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void gst_x_overlay_set_xwindow_id     (GstXOverlay *overlay, gulong xwindow_id);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void gst_x_overlay_expose             (GstXOverlay *overlay);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void gst_x_overlay_handle_events      (GstXOverlay *overlay,
                                       gboolean     handle_events);

/* public methods to dispatch bus messages */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void gst_x_overlay_got_xwindow_id     (GstXOverlay *overlay, gulong xwindow_id);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void gst_x_overlay_prepare_xwindow_id (GstXOverlay *overlay);

G_END_DECLS

#endif /* __GST_X_OVERLAY_H__ */
