/* GStreamer
 *
 * appsrc-ra.c: example for using appsrc in random-access mode.
 *
 * Copyright (C) 2008 Wim Taymans <wim.taymans@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

GST_DEBUG_CATEGORY (appsrc_playbin_debug);
#define GST_CAT_DEFAULT appsrc_playbin_debug

/*
 * an example application of using appsrc in random-access mode. When the
 * appsrc requests data with the need-data signal, we retrieve a buffer of the
 * requested size and push it to appsrc.
 *
 * This is a good example how one would deal with a local file resource.
 *
 * Appsrc in random-access mode needs seeking support and we must thus connect
 * to the seek signal to perform any seeks when requested.
 *
 * In random-access mode we must set the size of the source material.
 */
typedef struct _App App;

struct _App
{
  GstElement *playbin;
  GstElement *appsrc;

  GMainLoop *loop;

  GMappedFile *file;
  guint8 *data;
  gsize length;
  guint64 offset;
};

App s_app;

/* This method is called by the need-data signal callback, we feed data into the
 * appsrc with the requested size.
 */
static void
feed_data (GstElement * appsrc, guint size, App * app)
{
  GstBuffer *buffer;
  GstFlowReturn ret;

  buffer = gst_buffer_new ();

  if (app->offset >= app->length) {
    /* we are EOS, send end-of-stream */
    g_signal_emit_by_name (app->appsrc, "end-of-stream", &ret);
    return;
  }

  /* read the amount of data, we are allowed to return less if we are EOS */
  if (app->offset + size > app->length)
    size = app->length - app->offset;

  GST_BUFFER_DATA (buffer) = app->data + app->offset;
  GST_BUFFER_SIZE (buffer) = size;
  /* we need to set an offset for random access */
  GST_BUFFER_OFFSET (buffer) = app->offset;
  GST_BUFFER_OFFSET_END (buffer) = app->offset + size;

  GST_DEBUG ("feed buffer %p, offset %" G_GUINT64_FORMAT "-%u", buffer,
      app->offset, size);
  g_signal_emit_by_name (app->appsrc, "push-buffer", buffer, &ret);
  gst_buffer_unref (buffer);

  app->offset += size;

  return;
}

/* called when appsrc wants us to return data from a new position with the next
 * call to push-buffer. */
static gboolean
seek_data (GstElement * appsrc, guint64 position, App * app)
{
  GST_DEBUG ("seek to offset %" G_GUINT64_FORMAT, position);
  app->offset = position;

  return TRUE;
}

/* this callback is called when playbin2 has constructed a source object to read
 * from. Since we provided the appsrc:// uri to playbin2, this will be the
 * appsrc that we must handle. We set up some signals to push data into appsrc
 * and one to perform a seek. */
static void
found_source (GObject * object, GObject * orig, GParamSpec * pspec, App * app)
{
  /* get a handle to the appsrc */
  g_object_get (orig, pspec->name, &app->appsrc, NULL);

  GST_DEBUG ("got appsrc %p", app->appsrc);

  /* we can set the length in appsrc. This allows some elements to estimate the
   * total duration of the stream. It's a good idea to set the property when you
   * can but it's not required. */
  g_object_set (app->appsrc, "size", (gint64) app->length, NULL);
  gst_util_set_object_arg (G_OBJECT (app->appsrc), "stream-type",
      "random-access");

  /* configure the appsrc, we will push a buffer to appsrc when it needs more
   * data */
  g_signal_connect (app->appsrc, "need-data", G_CALLBACK (feed_data), app);
  g_signal_connect (app->appsrc, "seek-data", G_CALLBACK (seek_data), app);
}

static gboolean
bus_message (GstBus * bus, GstMessage * message, App * app)
{
  GST_DEBUG ("got message %s",
      gst_message_type_get_name (GST_MESSAGE_TYPE (message)));

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:
      g_error ("received error");
      g_main_loop_quit (app->loop);
      break;
    case GST_MESSAGE_EOS:
      g_main_loop_quit (app->loop);
      break;
    default:
      break;
  }
  return TRUE;
}

int
main (int argc, char *argv[])
{
  App *app = &s_app;
  GError *error = NULL;
  GstBus *bus;

  gst_init (&argc, &argv);

  GST_DEBUG_CATEGORY_INIT (appsrc_playbin_debug, "appsrc-playbin", 0,
      "appsrc playbin example");

  if (argc < 2) {
    g_print ("usage: %s <filename>\n", argv[0]);
    return -1;
  }

  /* try to open the file as an mmapped file */
  app->file = g_mapped_file_new (argv[1], FALSE, &error);
  if (error) {
    g_print ("failed to open file: %s\n", error->message);
    g_error_free (error);
    return -2;
  }
  /* get some vitals, this will be used to read data from the mmapped file and
   * feed it to appsrc. */
  app->length = g_mapped_file_get_length (app->file);
  app->data = (guint8 *) g_mapped_file_get_contents (app->file);
  app->offset = 0;

  /* create a mainloop to get messages */
  app->loop = g_main_loop_new (NULL, TRUE);

  app->playbin = gst_element_factory_make ("playbin2", NULL);
  g_assert (app->playbin);

  bus = gst_pipeline_get_bus (GST_PIPELINE (app->playbin));

  /* add watch for messages */
  gst_bus_add_watch (bus, (GstBusFunc) bus_message, app);

  /* set to read from appsrc */
  g_object_set (app->playbin, "uri", "appsrc://", NULL);

  /* get notification when the source is created so that we get a handle to it
   * and can configure it */
  g_signal_connect (app->playbin, "deep-notify::source",
      (GCallback) found_source, app);

  /* go to playing and wait in a mainloop. */
  gst_element_set_state (app->playbin, GST_STATE_PLAYING);

  /* this mainloop is stopped when we receive an error or EOS */
  g_main_loop_run (app->loop);

  GST_DEBUG ("stopping");

  gst_element_set_state (app->playbin, GST_STATE_NULL);

  /* free the file */
  g_mapped_file_free (app->file);

  gst_object_unref (bus);
  g_main_loop_unref (app->loop);

  return 0;
}
