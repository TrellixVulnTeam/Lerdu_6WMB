/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wim.taymans@chello.be>
 *
 * gstformat.h: Header for GstFormat types used in queries and
 *              seeking.
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


#ifndef __GST_FORMAT_H__
#define __GST_FORMAT_H__

#include <glib.h>

#include <gst/gstiterator.h>

G_BEGIN_DECLS

/**
 * GstFormat:
 * @GST_FORMAT_UNDEFINED: undefined format
 * @GST_FORMAT_DEFAULT: the default format of the pad/element. This can be
 *    samples for raw audio, frames/fields for raw video.
 * @GST_FORMAT_BYTES: bytes
 * @GST_FORMAT_TIME: time in nanoseconds
 * @GST_FORMAT_BUFFERS: buffers
 * @GST_FORMAT_PERCENT: percentage of stream
 *
 * Standard predefined formats
 */
/* NOTE: don't forget to update the table in gstformat.c when changing
 * this enum */
typedef enum {
  GST_FORMAT_UNDEFINED 	=  0, /* must be first in list */
  GST_FORMAT_DEFAULT   	=  1,
  GST_FORMAT_BYTES   	=  2,
  GST_FORMAT_TIME 	=  3,
  GST_FORMAT_BUFFERS	=  4,
  GST_FORMAT_PERCENT	=  5
} GstFormat;

/* a percentage is always relative to 1000000 */
/**
 * GST_FORMAT_PERCENT_MAX:
 *
 * The PERCENT format is between 0 and this value
 */
#define	GST_FORMAT_PERCENT_MAX		G_GINT64_CONSTANT (1000000)
/**
 * GST_FORMAT_PERCENT_SCALE:
 *
 * The value used to scale down the reported PERCENT format value to
 * its real value.
 */
#define	GST_FORMAT_PERCENT_SCALE	G_GINT64_CONSTANT (10000)

typedef struct _GstFormatDefinition GstFormatDefinition;

/**
 * GstFormatDefinition:
 * @value: The unique id of this format
 * @nick: A short nick of the format
 * @description: A longer description of the format
 * @quark: A quark for the nick
 *
 * A format definition
 */
struct _GstFormatDefinition
{
  GstFormat  value;
  gchar     *nick;
  gchar     *description;
  GQuark     quark;
};
#ifdef __SYMBIAN32__
IMPORT_C
#endif


const gchar*    gst_format_get_name             (GstFormat format);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GQuark          gst_format_to_quark             (GstFormat format);

/* register a new format */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstFormat	gst_format_register		(const gchar *nick,
						 const gchar *description);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstFormat	gst_format_get_by_nick		(const gchar *nick);

/* check if a format is in an array of formats */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean	gst_formats_contains		(const GstFormat *formats, GstFormat format);

/* query for format details */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

G_CONST_RETURN GstFormatDefinition*
		gst_format_get_details		(GstFormat format);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstIterator* 	gst_format_iterate_definitions 	(void);

G_END_DECLS

#endif /* __GST_FORMAT_H__ */
