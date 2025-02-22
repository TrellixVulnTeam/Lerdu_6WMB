/* GStreamer PropertyProbe
 * Copyright (C) 2003 David A. Schleef <ds@schleef.org>
 *
 * property_probe.h: property_probe interface design
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

#ifndef __GST_PROPERTY_PROBE_H__
#define __GST_PROPERTY_PROBE_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_PROPERTY_PROBE \
  (gst_property_probe_get_type ())
#define GST_PROPERTY_PROBE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_PROPERTY_PROBE, GstPropertyProbe))
#define GST_IS_PROPERTY_PROBE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_PROPERTY_PROBE))
#define GST_PROPERTY_PROBE_GET_IFACE(obj) \
  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GST_TYPE_PROPERTY_PROBE, GstPropertyProbeInterface))

typedef struct _GstPropertyProbe GstPropertyProbe; /* dummy typedef */

typedef struct _GstPropertyProbeInterface {
  GTypeInterface klass;

  /* signals */
  void          (*probe_needed)   (GstPropertyProbe *probe,
                                   const GParamSpec *pspec);

  /* virtual functions */
  const GList * (*get_properties) (GstPropertyProbe *probe);
  gboolean      (*needs_probe)    (GstPropertyProbe *probe,
                                   guint             prop_id,
                                   const GParamSpec *pspec);
  void          (*probe_property) (GstPropertyProbe *probe,
                                   guint             prop_id,
                                   const GParamSpec *pspec);
  GValueArray * (*get_values)     (GstPropertyProbe *probe,
                                   guint             prop_id,
                                   const GParamSpec *pspec);

  gpointer _gst_reserved[GST_PADDING];
} GstPropertyProbeInterface;
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GType        gst_property_probe_get_type       (void);

/* virtual class function wrappers */

/* returns list of GParamSpecs */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

const GList *     gst_property_probe_get_properties      (GstPropertyProbe *probe);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

const GParamSpec *gst_property_probe_get_property        (GstPropertyProbe *probe,
                                                          const gchar      *name);

/* probe one property */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void              gst_property_probe_probe_property      (GstPropertyProbe *probe,
                                                          const GParamSpec *pspec);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void              gst_property_probe_probe_property_name (GstPropertyProbe *probe,
                                                          const gchar      *name);

/* do we need a probe? */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean          gst_property_probe_needs_probe         (GstPropertyProbe *probe,
                                                          const GParamSpec *pspec);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean          gst_property_probe_needs_probe_name    (GstPropertyProbe *probe,
                                                          const gchar      *name);

/* returns list of GValues */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GValueArray *     gst_property_probe_get_values          (GstPropertyProbe *probe,
                                                          const GParamSpec *pspec);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GValueArray *     gst_property_probe_get_values_name     (GstPropertyProbe *probe,
                                                          const gchar      *name);

/* sugar */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GValueArray *     gst_property_probe_probe_and_get_values (GstPropertyProbe *probe,
                                                          const GParamSpec *pspec);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GValueArray *     gst_property_probe_probe_and_get_values_name (GstPropertyProbe *probe,
                                                          const gchar      *name);

G_END_DECLS

#endif /* __GST_PROPERTY_PROBE_H__ */
