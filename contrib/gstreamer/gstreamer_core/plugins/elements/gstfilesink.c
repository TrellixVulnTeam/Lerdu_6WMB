/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *                    2006 Wim Taymans <wim@fluendo.com>
 *
 * gstfilesink.c:
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
 * SECTION:element-filesink
 * @short_description: write stream to a file
 * @see_also: #GstFileSrc
 *
 * Write incoming data to a file in the local file system.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#ifdef __SYMBIAN32__
#include <gst_global.h>
#include <gstpoll.h>
#include <gstbasesink.h> //rj
#endif

#include "../../gst/gst-i18n-lib.h"

#include <gst/gst.h>
#include <stdio.h>              /* for fseeko() */
#ifdef HAVE_STDIO_EXT_H
#include <stdio_ext.h>          /* for __fbufsize, for debugging */
#endif
#include <errno.h>
#include "gstfilesink.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

#define GST_TYPE_BUFFER_MODE (buffer_mode_get_type ())
static GType
buffer_mode_get_type (void)
{
  static GType buffer_mode_type = 0;
  static const GEnumValue buffer_mode[] = {
    {-1, "Default buffering", "default"},
    {_IOFBF, "Fully buffered", "full"},
    {_IOLBF, "Line buffered", "line"},
    {_IONBF, "Unbuffered", "unbuffered"},
    {0, NULL, NULL},
  };

  if (!buffer_mode_type) {
    buffer_mode_type =
        g_enum_register_static ("GstFileSinkBufferMode", buffer_mode);
  }
  return buffer_mode_type;
}

GST_DEBUG_CATEGORY_STATIC (gst_file_sink_debug);
#define GST_CAT_DEFAULT gst_file_sink_debug

#define DEFAULT_LOCATION 	NULL
#define DEFAULT_BUFFER_MODE 	-1
#define DEFAULT_BUFFER_SIZE 	64 * 1024

enum
{
  PROP_0,
  PROP_LOCATION,
  PROP_BUFFER_MODE,
  PROP_BUFFER_SIZE,
  PROP_LAST
};

static void gst_file_sink_dispose (GObject * object);

static void gst_file_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_file_sink_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_file_sink_open_file (GstFileSink * sink);
static void gst_file_sink_close_file (GstFileSink * sink);

static gboolean gst_file_sink_start (GstBaseSink * sink);
static gboolean gst_file_sink_stop (GstBaseSink * sink);
static gboolean gst_file_sink_event (GstBaseSink * sink, GstEvent * event);
static GstFlowReturn gst_file_sink_render (GstBaseSink * sink,
    GstBuffer * buffer);

static gboolean gst_file_sink_do_seek (GstFileSink * filesink,
    guint64 new_offset);
static gboolean gst_file_sink_get_current_offset (GstFileSink * filesink,
    guint64 * p_pos);

static gboolean gst_file_sink_query (GstPad * pad, GstQuery * query);

static void gst_file_sink_uri_handler_init (gpointer g_iface,
    gpointer iface_data);


static void
_do_init (GType filesink_type)
{
  static const GInterfaceInfo urihandler_info = {
    gst_file_sink_uri_handler_init,
    NULL,
    NULL
  };

  g_type_add_interface_static (filesink_type, GST_TYPE_URI_HANDLER,
      &urihandler_info);
  GST_DEBUG_CATEGORY_INIT (gst_file_sink_debug, "filesink", 0,
      "filesink element");
}

GST_BOILERPLATE_FULL (GstFileSink, gst_file_sink, GstBaseSink,
    GST_TYPE_BASE_SINK, _do_init);

static void
gst_file_sink_base_init (gpointer g_class)
{
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_set_details_simple (gstelement_class,
      "File Sink",
      "Sink/File", "Write stream to a file", "Thomas <thomas@apestaart.org>");
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sinktemplate));
}

static void
gst_file_sink_class_init (GstFileSinkClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseSinkClass *gstbasesink_class = GST_BASE_SINK_CLASS (klass);

  gobject_class->dispose = gst_file_sink_dispose;

  gobject_class->set_property = gst_file_sink_set_property;
  gobject_class->get_property = gst_file_sink_get_property;

  g_object_class_install_property (gobject_class, PROP_LOCATION,
      g_param_spec_string ("location", "File Location",
          "Location of the file to write", NULL, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_BUFFER_MODE,
      g_param_spec_enum ("buffer-mode", "Buffering mode",
          "The buffering mode to use", GST_TYPE_BUFFER_MODE,
          DEFAULT_BUFFER_MODE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_BUFFER_SIZE,
      g_param_spec_uint ("buffer-size", "Buffering size",
          "Size of buffer in number of bytes for line or full buffer-mode", 0,
          G_MAXUINT, DEFAULT_BUFFER_SIZE, G_PARAM_READWRITE));

  gstbasesink_class->get_times = NULL;
  gstbasesink_class->start = GST_DEBUG_FUNCPTR (gst_file_sink_start);
  gstbasesink_class->stop = GST_DEBUG_FUNCPTR (gst_file_sink_stop);
  gstbasesink_class->render = GST_DEBUG_FUNCPTR (gst_file_sink_render);
  gstbasesink_class->event = GST_DEBUG_FUNCPTR (gst_file_sink_event);

  if (sizeof (off_t) < 8) {
    GST_LOG ("No large file support, sizeof (off_t) = %" G_GSIZE_FORMAT "!",
        sizeof (off_t));
  }
}

static void
gst_file_sink_init (GstFileSink * filesink, GstFileSinkClass * g_class)
{
  GstPad *pad;

  pad = GST_BASE_SINK_PAD (filesink);

  gst_pad_set_query_function (pad, GST_DEBUG_FUNCPTR (gst_file_sink_query));

  filesink->filename = NULL;
  filesink->file = NULL;
  filesink->buffer_mode = DEFAULT_BUFFER_MODE;
  filesink->buffer_size = DEFAULT_BUFFER_SIZE;
  filesink->buffer = NULL;

  gst_base_sink_set_sync (GST_BASE_SINK (filesink), FALSE);
}

static void
gst_file_sink_dispose (GObject * object)
{
  GstFileSink *sink = GST_FILE_SINK (object);

  G_OBJECT_CLASS (parent_class)->dispose (object);

  g_free (sink->uri);
  sink->uri = NULL;
  g_free (sink->filename);
  sink->filename = NULL;
  g_free (sink->buffer);
  sink->buffer = NULL;
  sink->buffer_size = 0;
}

static gboolean
gst_file_sink_set_location (GstFileSink * sink, const gchar * location)
{
  if (sink->file)
    goto was_open;

  g_free (sink->filename);
  g_free (sink->uri);
  if (location != NULL) {
    sink->filename = g_strdup (location);
    sink->uri = gst_uri_construct ("file", location);
  } else {
    sink->filename = NULL;
    sink->uri = NULL;
  }

  return TRUE;

  /* ERRORS */
was_open:
  {
    g_warning ("Changing the `location' property on filesink when "
        "a file is open not supported.");
    return FALSE;
  }
}
static void
gst_file_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstFileSink *sink = GST_FILE_SINK (object);

  switch (prop_id) {
    case PROP_LOCATION:
      gst_file_sink_set_location (sink, g_value_get_string (value));
      break;
    case PROP_BUFFER_MODE:
      sink->buffer_mode = g_value_get_enum (value);
      break;
    case PROP_BUFFER_SIZE:
      sink->buffer_size = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_file_sink_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  GstFileSink *sink = GST_FILE_SINK (object);

  switch (prop_id) {
    case PROP_LOCATION:
      g_value_set_string (value, sink->filename);
      break;
    case PROP_BUFFER_MODE:
      g_value_set_enum (value, sink->buffer_mode);
      break;
    case PROP_BUFFER_SIZE:
      g_value_set_uint (value, sink->buffer_size);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gboolean
gst_file_sink_open_file (GstFileSink * sink)
{
  gint mode;

  /* open the file */
  if (sink->filename == NULL || sink->filename[0] == '\0')
    goto no_filename;

  sink->file = fopen (sink->filename, "wb");
  if (sink->file == NULL)
    goto open_failed;

  /* see if we are asked to perform a specific kind of buffering */
  if ((mode = sink->buffer_mode) != -1) {
    gsize buffer_size;

    /* free previous buffer if any */
    g_free (sink->buffer);

    if (mode == _IONBF) {
      /* no buffering */
      sink->buffer = NULL;
      buffer_size = 0;
    } else {
      /* allocate buffer */
      sink->buffer = g_malloc (sink->buffer_size);
      buffer_size = sink->buffer_size;
    }
#ifdef HAVE_STDIO_EXT_H
    GST_DEBUG_OBJECT (sink, "change buffer size %d to %d, mode %d",
        __fbufsize (sink->file), buffer_size, mode);
#else
    GST_DEBUG_OBJECT (sink, "change  buffer size to %d, mode %d",
        sink->buffer_size, mode);
#endif
    if (setvbuf (sink->file, sink->buffer, mode, buffer_size) != 0) {
      GST_WARNING_OBJECT (sink, "warning: setvbuf failed: %s",
          g_strerror (errno));
    }
  }

  sink->current_pos = 0;
  /* try to seek in the file to figure out if it is seekable */
  sink->seekable = gst_file_sink_do_seek (sink, 0);

  GST_DEBUG_OBJECT (sink, "opened file %s, seekable %d",
      sink->filename, sink->seekable);

  return TRUE;

  /* ERRORS */
no_filename:
  {
    GST_ELEMENT_ERROR (sink, RESOURCE, NOT_FOUND,
        (_("No file name specified for writing.")), (NULL));
    return FALSE;
  }
open_failed:
  {
    GST_ELEMENT_ERROR (sink, RESOURCE, OPEN_WRITE,
        (_("Could not open file \"%s\" for writing."), sink->filename),
        GST_ERROR_SYSTEM);
    return FALSE;
  }
}

static void
gst_file_sink_close_file (GstFileSink * sink)
{
  if (sink->file) {
    if (fclose (sink->file) != 0)
      goto close_failed;

    GST_DEBUG_OBJECT (sink, "closed file");
    sink->file = NULL;

    g_free (sink->buffer);
    sink->buffer = NULL;
  }
  return;

  /* ERRORS */
close_failed:
  {
    GST_ELEMENT_ERROR (sink, RESOURCE, CLOSE,
        (_("Error closing file \"%s\"."), sink->filename), GST_ERROR_SYSTEM);
    return;
  }
}

static gboolean
gst_file_sink_query (GstPad * pad, GstQuery * query)
{
  GstFileSink *self;
  GstFormat format;

  self = GST_FILE_SINK (GST_PAD_PARENT (pad));

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_POSITION:
      gst_query_parse_position (query, &format, NULL);
      switch (format) {
        case GST_FORMAT_DEFAULT:
        case GST_FORMAT_BYTES:
          gst_query_set_position (query, GST_FORMAT_BYTES, self->current_pos);
          return TRUE;
        default:
          return FALSE;
      }

    case GST_QUERY_FORMATS:
      gst_query_set_formats (query, 2, GST_FORMAT_DEFAULT, GST_FORMAT_BYTES);
      return TRUE;

    default:
      return gst_pad_query_default (pad, query);
  }
}

#ifdef HAVE_FSEEKO
# define __GST_STDIO_SEEK_FUNCTION "fseeko"
#elif defined (G_OS_UNIX)
# define __GST_STDIO_SEEK_FUNCTION "lseek"
#else
# define __GST_STDIO_SEEK_FUNCTION "fseek"
#endif

static gboolean
gst_file_sink_do_seek (GstFileSink * filesink, guint64 new_offset)
{
  GST_DEBUG_OBJECT (filesink, "Seeking to offset %" G_GUINT64_FORMAT
      " using " __GST_STDIO_SEEK_FUNCTION, new_offset);

  if (fflush (filesink->file))
    goto flush_failed;

#ifdef HAVE_FSEEKO
  if (fseeko (filesink->file, (off_t) new_offset, SEEK_SET) != 0)
    goto seek_failed;
#elif defined (G_OS_UNIX)
  if (lseek (fileno (filesink->file), (off_t) new_offset,
          SEEK_SET) == (off_t) - 1)
    goto seek_failed;
#else
  if (fseek (filesink->file, (long) new_offset, SEEK_SET) != 0)
    goto seek_failed;
#endif

  /* adjust position reporting after seek;
   * presumably this should basically yield new_offset */
  gst_file_sink_get_current_offset (filesink, &filesink->current_pos);

  return TRUE;

  /* ERRORS */
flush_failed:
  {
    GST_DEBUG_OBJECT (filesink, "Flush failed: %s", g_strerror (errno));
    return FALSE;
  }
seek_failed:
  {
    GST_DEBUG_OBJECT (filesink, "Seeking failed: %s", g_strerror (errno));
    return FALSE;
  }
}

/* handle events (search) */
static gboolean
gst_file_sink_event (GstBaseSink * sink, GstEvent * event)
{
  GstEventType type;
  GstFileSink *filesink;

  filesink = GST_FILE_SINK (sink);

  type = GST_EVENT_TYPE (event);

  switch (type) {
    case GST_EVENT_NEWSEGMENT:
    {
      gint64 start, stop, pos;
      GstFormat format;

      gst_event_parse_new_segment (event, NULL, NULL, &format, &start,
          &stop, &pos);

      if (format == GST_FORMAT_BYTES) {
        /* only try to seek and fail when we are going to a different
         * position */
        if (filesink->current_pos != start) {
          /* FIXME, the seek should be performed on the pos field, start/stop are
           * just boundaries for valid bytes offsets. We should also fill the file
           * with zeroes if the new position extends the current EOF (sparse streams
           * and segment accumulation). */
          if (!gst_file_sink_do_seek (filesink, (guint64) start))
            goto seek_failed;
        } else {
          GST_DEBUG_OBJECT (filesink, "Ignored NEWSEGMENT, no seek needed");
        }
      } else {
        GST_DEBUG_OBJECT (filesink,
            "Ignored NEWSEGMENT event of format %u (%s)", (guint) format,
            gst_format_get_name (format));
      }
      break;
    }
    case GST_EVENT_EOS:
      if (fflush (filesink->file))
        goto flush_failed;
      break;
    default:
      break;
  }

  return TRUE;

  /* ERRORS */
seek_failed:
  {
    GST_ELEMENT_ERROR (filesink, RESOURCE, SEEK,
        (_("Error while seeking in file \"%s\"."), filesink->filename),
        GST_ERROR_SYSTEM);
    return FALSE;
  }
flush_failed:
  {
    GST_ELEMENT_ERROR (filesink, RESOURCE, WRITE,
        (_("Error while writing to file \"%s\"."), filesink->filename),
        GST_ERROR_SYSTEM);
    return FALSE;
  }
}

static gboolean
gst_file_sink_get_current_offset (GstFileSink * filesink, guint64 * p_pos)
{
  off_t ret;

#ifdef HAVE_FTELLO
  ret = ftello (filesink->file);
#elif defined (G_OS_UNIX)
  if (fflush (filesink->file)) {
    GST_DEBUG_OBJECT (filesink, "Flush failed: %s", g_strerror (errno));
    /* ignore and continue */
  }
  ret = lseek (fileno (filesink->file), 0, SEEK_CUR);
#else
  ret = (off_t) ftell (filesink->file);
#endif

  if (ret != (off_t) - 1)
    *p_pos = (guint64) ret;

  return (ret != (off_t) - 1);
}

static GstFlowReturn
gst_file_sink_render (GstBaseSink * sink, GstBuffer * buffer)
{
  GstFileSink *filesink;
  guint size;

  size = GST_BUFFER_SIZE (buffer);

  filesink = GST_FILE_SINK (sink);

  GST_DEBUG_OBJECT (filesink, "writing %u bytes at %" G_GUINT64_FORMAT,
      size, filesink->current_pos);

  if (size > 0 && GST_BUFFER_DATA (buffer) != NULL) {
    if (fwrite (GST_BUFFER_DATA (buffer), size, 1, filesink->file) != 1)
      goto handle_error;

    filesink->current_pos += size;
  }

  return GST_FLOW_OK;

handle_error:
  {
    switch (errno) {
      case ENOSPC:{
        GST_ELEMENT_ERROR (filesink, RESOURCE, NO_SPACE_LEFT, (NULL), (NULL));
        break;
      }
      default:{
        GST_ELEMENT_ERROR (filesink, RESOURCE, WRITE,
            (_("Error while writing to file \"%s\"."), filesink->filename),
            ("%s", g_strerror (errno)));
      }
    }
    return GST_FLOW_ERROR;
  }
}

static gboolean
gst_file_sink_start (GstBaseSink * basesink)
{
  return gst_file_sink_open_file (GST_FILE_SINK (basesink));
}

static gboolean
gst_file_sink_stop (GstBaseSink * basesink)
{
  gst_file_sink_close_file (GST_FILE_SINK (basesink));
  return TRUE;
}

/*** GSTURIHANDLER INTERFACE *************************************************/
#ifdef __SYMBIAN32__
GstURIType
#else
static guint
#endif 
gst_file_sink_uri_get_type (void)
{
  return GST_URI_SINK;
}
static gchar **
gst_file_sink_uri_get_protocols (void)
{
  static gchar *protocols[] = { "file", NULL };

  return protocols;
}
static const gchar *
gst_file_sink_uri_get_uri (GstURIHandler * handler)
{
  GstFileSink *sink = GST_FILE_SINK (handler);

  return sink->uri;
}

static gboolean
gst_file_sink_uri_set_uri (GstURIHandler * handler, const gchar * uri)
{
  gchar *protocol, *location;
  gboolean ret;
  GstFileSink *sink = GST_FILE_SINK (handler);

  protocol = gst_uri_get_protocol (uri);
  if (strcmp (protocol, "file") != 0) {
    g_free (protocol);
    return FALSE;
  }
  g_free (protocol);

  /* allow file://localhost/foo/bar by stripping localhost but fail
   * for every other hostname */
  if (g_str_has_prefix (uri, "file://localhost/")) {
    char *tmp;

    /* 16 == strlen ("file://localhost") */
    tmp = g_strconcat ("file://", uri + 16, NULL);
    /* we use gst_uri_get_location() although we already have the
     * "location" with uri + 16 because it provides unescaping */
    location = gst_uri_get_location (tmp);
    g_free (tmp);
  } else if (strcmp (uri, "file://") == 0) {
    /* Special case for "file://" as this is used by some applications
     *  to test with gst_element_make_from_uri if there's an element
     *  that supports the URI protocol. */
    gst_file_sink_set_location (sink, NULL);
    return TRUE;
  } else {
    location = gst_uri_get_location (uri);
  }

  if (!location)
    return FALSE;
  if (!g_path_is_absolute (location)) {
    g_free (location);
    return FALSE;
  }

  ret = gst_file_sink_set_location (sink, location);
  g_free (location);

  return ret;
}

static void
gst_file_sink_uri_handler_init (gpointer g_iface, gpointer iface_data)
{
  GstURIHandlerInterface *iface = (GstURIHandlerInterface *) g_iface;

  iface->get_type = gst_file_sink_uri_get_type;
  iface->get_protocols = gst_file_sink_uri_get_protocols;
  iface->get_uri = gst_file_sink_uri_get_uri;
  iface->set_uri = gst_file_sink_uri_set_uri;
}
