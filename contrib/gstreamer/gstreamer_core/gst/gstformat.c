/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wim.taymans@chello.be>
 *                    2005 Wim Taymans <wim@fluendo.com>
 *
 * gstformat.c: GstFormat registration
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

/**
 * SECTION:gstformat
 * @short_description: Dynamically register new data formats
 * @see_also: #GstPad, #GstElement
 *
 * GstFormats functions are used to register a new format to the gstreamer
 * core.  Formats can be used to perform seeking or conversions/query
 * operations.
 */


#include "gst_private.h"
#include <string.h>
#include "gstformat.h"
#include "gstenumtypes.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#endif

static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
static GList *_gst_formats = NULL;
static GHashTable *_nick_to_format = NULL;
static GHashTable *_format_to_nick = NULL;
static guint32 _n_values = 1;   /* we start from 1 because 0 reserved for UNDEFINED */

static GstFormatDefinition standard_definitions[] = {
  {GST_FORMAT_DEFAULT, "default", "Default format for the media type", 0},
  {GST_FORMAT_BYTES, "bytes", "Bytes", 0},
  {GST_FORMAT_TIME, "time", "Time", 0},
  {GST_FORMAT_BUFFERS, "buffers", "Buffers", 0},
  {GST_FORMAT_PERCENT, "percent", "Percent", 0},
  {0, NULL, NULL, 0}
};
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
_gst_format_initialize (void)
{
  GstFormatDefinition *standards = standard_definitions;

  g_static_mutex_lock (&mutex);
  if (_nick_to_format == NULL) {
    _nick_to_format = g_hash_table_new (g_str_hash, g_str_equal);
    _format_to_nick = g_hash_table_new (NULL, NULL);
  }

  while (standards->nick) {
    standards->quark = g_quark_from_static_string (standards->nick);
    g_hash_table_insert (_nick_to_format, standards->nick, standards);
    g_hash_table_insert (_format_to_nick, GINT_TO_POINTER (standards->value),
        standards);

    _gst_formats = g_list_append (_gst_formats, standards);
    standards++;
    _n_values++;
  }
  /* getting the type registers the enum */
  g_type_class_ref (gst_format_get_type ());
  g_static_mutex_unlock (&mutex);
}

/**
 * gst_format_get_name:
 * @format: a #GstFormat
 *
 * Get a printable name for the given format. Do not modify or free.
 *
 * Returns: a reference to the static name of the format or NULL if
 * the format is unknown.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

const gchar *
gst_format_get_name (GstFormat format)
{
  const GstFormatDefinition *def;
  const gchar *result;

  if ((def = gst_format_get_details (format)) != NULL)
    result = def->nick;
  else
    result = NULL;

  return result;
}

/**
 * gst_format_to_quark:
 * @format: a #GstFormat
 *
 * Get the unique quark for the given format.
 *
 * Returns: the quark associated with the format or 0 if the format
 * is unknown.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GQuark
gst_format_to_quark (GstFormat format)
{
  const GstFormatDefinition *def;
  GQuark result;

  if ((def = gst_format_get_details (format)) != NULL)
    result = def->quark;
  else
    result = 0;

  return result;
}

/**
 * gst_format_register:
 * @nick: The nick of the new format
 * @description: The description of the new format
 *
 * Create a new GstFormat based on the nick or return an
 * already registered format with that nick.
 *
 * Returns: A new GstFormat or an already registered format
 * with the same nick.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstFormat
gst_format_register (const gchar * nick, const gchar * description)
{
  GstFormatDefinition *format;
  GstFormat query;

  g_return_val_if_fail (nick != NULL, 0);
  g_return_val_if_fail (description != NULL, 0);

  query = gst_format_get_by_nick (nick);
  if (query != GST_FORMAT_UNDEFINED)
    return query;

  g_static_mutex_lock (&mutex);
  format = g_new0 (GstFormatDefinition, 1);
  format->value = _n_values;
  format->nick = g_strdup (nick);
  format->description = g_strdup (description);
  format->quark = g_quark_from_static_string (format->nick);

  g_hash_table_insert (_nick_to_format, format->nick, format);
  g_hash_table_insert (_format_to_nick, GINT_TO_POINTER (format->value),
      format);
  _gst_formats = g_list_append (_gst_formats, format);
  _n_values++;
  g_static_mutex_unlock (&mutex);

  return format->value;
}

/**
 * gst_format_get_by_nick:
 * @nick: The nick of the format
 *
 * Return the format registered with the given nick.
 *
 * Returns: The format with @nick or GST_FORMAT_UNDEFINED
 * if the format was not registered.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstFormat
gst_format_get_by_nick (const gchar * nick)
{
  GstFormatDefinition *format;

  g_return_val_if_fail (nick != NULL, 0);

  g_static_mutex_lock (&mutex);
  format = g_hash_table_lookup (_nick_to_format, nick);
  g_static_mutex_unlock (&mutex);

  if (format != NULL)
    return format->value;
  else
    return GST_FORMAT_UNDEFINED;
}

/**
 * gst_formats_contains:
 * @formats: The format array to search
 * @format: the format to find
 *
 * See if the given format is inside the format array.
 *
 * Returns: TRUE if the format is found inside the array
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_formats_contains (const GstFormat * formats, GstFormat format)
{
  if (!formats)
    return FALSE;

  while (*formats) {
    if (*formats == format)
      return TRUE;

    formats++;
  }
  return FALSE;
}


/**
 * gst_format_get_details:
 * @format: The format to get details of
 *
 * Get details about the given format.
 *
 * Returns: The #GstFormatDefinition for @format or NULL on failure.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

const GstFormatDefinition *
gst_format_get_details (GstFormat format)
{
  const GstFormatDefinition *result;

  g_static_mutex_lock (&mutex);
  result = g_hash_table_lookup (_format_to_nick, GINT_TO_POINTER (format));
  g_static_mutex_unlock (&mutex);

  return result;
}

/**
 * gst_format_iterate_definitions:
 *
 * Iterate all the registered formats. The format definition is read
 * only.
 *
 * Returns: A GstIterator of #GstFormatDefinition.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstIterator *
gst_format_iterate_definitions (void)
{
  GstIterator *result;

  g_static_mutex_lock (&mutex);
  /* FIXME: register a boxed type for GstFormatDefinition */
  result = gst_iterator_new_list (G_TYPE_POINTER,
      g_static_mutex_get_mutex (&mutex), &_n_values, &_gst_formats,
      NULL, NULL, NULL);
  g_static_mutex_unlock (&mutex);

  return result;
}
