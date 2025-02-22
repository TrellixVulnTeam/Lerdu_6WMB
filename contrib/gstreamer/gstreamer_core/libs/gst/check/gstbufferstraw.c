/* GStreamer
 *
 * unit testing helper lib
 *
 * Copyright (C) 2006 Andy Wingo <wingo at pobox.com>
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
 * SECTION:gstcheckbufferstraw
 * @short_description: Buffer interception code for GStreamer unit tests
 *
 * These macros and functions are for internal use of the unit tests found
 * inside the 'check' directories of various GStreamer packages.
 */

#include "gstbufferstraw.h"
#include <glib_global.h>

static GCond *cond = NULL;
static GMutex *lock = NULL;
static GstBuffer *buf = NULL;
static gulong id;

/* called for every buffer.  Waits until the global "buf" variable is unset,
 * then sets it to the buffer received, and signals. */
static gboolean
buffer_probe (GstPad * pad, GstBuffer * buffer, gpointer unused)
{
  g_mutex_lock (lock);

  while (buf != NULL)
    g_cond_wait (cond, lock);

  /* increase the refcount because we store it globally for others to use */
  buf = gst_buffer_ref (buffer);

  g_cond_signal (cond);

  g_mutex_unlock (lock);

  return TRUE;
}

/**
 * gst_buffer_straw_start_pipeline:
 * @bin: the pipeline to run
 * @pad: a pad on an element in @bin
 *
 * Sets up a pipeline for buffer sucking. This will allow you to call
 * gst_buffer_straw_get_buffer() to access buffers as they pass over @pad.
 *
 * This function is normally used in unit tests that want to verify that a
 * particular element is outputting correct buffers. For example, you would make
 * a pipeline via gst_parse_launch(), pull out the pad you want to monitor, then
 * call gst_buffer_straw_get_buffer() to get the buffers that pass through @pad.
 * The pipeline will block until you have sucked off the buffers.
 *
 * This function will set the state of @bin to PLAYING; to clean up, be sure to
 * call gst_buffer_straw_stop_pipeline().
 *
 * Note that you may not start two buffer straws at the same time. This function
 * is intended for unit tests, not general API use. In fact it calls fail_if
 * from libcheck, so you cannot use it outside unit tests.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_buffer_straw_start_pipeline (GstElement * bin, GstPad * pad)
{
  GstStateChangeReturn ret;

  id = gst_pad_add_buffer_probe (pad, G_CALLBACK (buffer_probe), NULL);

  cond = g_cond_new ();
  lock = g_mutex_new ();

  ret = gst_element_set_state (bin, GST_STATE_PLAYING);
  fail_if (ret == GST_STATE_CHANGE_FAILURE, "Could not start test pipeline");
  if (ret == GST_STATE_CHANGE_ASYNC) {
    ret = gst_element_get_state (bin, NULL, NULL, GST_CLOCK_TIME_NONE);
    fail_if (ret != GST_STATE_CHANGE_SUCCESS, "Could not start test pipeline");
  }
}

/**
 * gst_buffer_straw_get_buffer:
 * @bin: the pipeline previously started via gst_buffer_straw_start_pipeline()
 * @pad: the pad previously passed to gst_buffer_straw_start_pipeline()
 *
 * Get one buffer from @pad. Implemented via buffer probes. This function will
 * block until the pipeline passes a buffer over @pad, so for robust behavior
 * in unit tests, you need to use check's timeout to fail out in the case that a
 * buffer never arrives.
 *
 * You must have previously called gst_buffer_straw_start_pipeline() on
 * @pipeline and @pad.
 *
 * Returns: the captured #GstBuffer.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstBuffer *
gst_buffer_straw_get_buffer (GstElement * bin, GstPad * pad)
{
  GstBuffer *ret;

  g_mutex_lock (lock);

  while (buf == NULL)
    g_cond_wait (cond, lock);

  ret = buf;
  buf = NULL;

  g_cond_signal (cond);

  g_mutex_unlock (lock);

  return ret;
}

/**
 * gst_buffer_straw_stop_pipeline:
 * @bin: the pipeline previously started via gst_buffer_straw_start_pipeline()
 * @pad: the pad previously passed to gst_buffer_straw_start_pipeline()
 *
 * Set @bin to #GST_STATE_NULL and release resource allocated in
 * gst_buffer_straw_start_pipeline().
 *
 * You must have previously called gst_buffer_straw_start_pipeline() on
 * @pipeline and @pad.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_buffer_straw_stop_pipeline (GstElement * bin, GstPad * pad)
{
  GstStateChangeReturn ret;

  g_mutex_lock (lock);
  if (buf)
    gst_buffer_unref (buf);
  buf = NULL;
  gst_pad_remove_buffer_probe (pad, (guint) id);
  id = 0;
  g_cond_signal (cond);
  g_mutex_unlock (lock);

  ret = gst_element_set_state (bin, GST_STATE_NULL);
  fail_if (ret == GST_STATE_CHANGE_FAILURE, "Could not stop test pipeline");
  if (ret == GST_STATE_CHANGE_ASYNC) {
    ret = gst_element_get_state (bin, NULL, NULL, GST_CLOCK_TIME_NONE);
    fail_if (ret != GST_STATE_CHANGE_SUCCESS, "Could not stop test pipeline");
  }

  g_mutex_lock (lock);
  if (buf)
    gst_buffer_unref (buf);
  buf = NULL;
  g_mutex_unlock (lock);

  g_mutex_free (lock);
  g_cond_free (cond);

  lock = NULL;
  cond = NULL;
}
