/* GStreamer
 * Copyright (C) 2006 Jan Schmidt <thaytan@noraisin.net>
 *
 * gstquark.c: Registered quarks for the _priv_gst_quark_table, private to 
 *   GStreamer
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

#include "gst_private.h"
#include "gstquark.h"

/* These strings must match order and number declared in the GstQuarkId
 * enum in gstquark.h! */
static const gchar *_quark_strings[] = {
  "format", "current", "duration", "rate",
  "seekable", "segment-start", "segment-end",
  "src_format", "src_value", "dest_format", "dest_value",
  "start_format", "start_value", "stop_format", "stop_value"
};

GQuark _priv_gst_quark_table[GST_QUARK_MAX];
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
_priv_gst_quarks_initialize (void)
{
  gint i;

  for (i = 0; i < GST_QUARK_MAX; i++) {
    _priv_gst_quark_table[i] = g_quark_from_static_string (_quark_strings[i]);
  }
}
