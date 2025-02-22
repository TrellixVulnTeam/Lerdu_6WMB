/* GStreamer
 *
 * addstream.c: sample application to dynamically add streams to a running
 * pipeline
 *
 * Copyright (C) <2007> Wim Taymans <wim dot taymans at gmail dot com>
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

#include <gst/gst_global.h>
#include <gst/gst.h>
#define LOG_FILE "c:\\logs\\addstream_logs.txt" 
#include "std_log_result.h" 
#include <glib_global.h>

#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}


static GstElement *pipeline;
static GstClock *theclock;
static GMainLoop *loop;
static GstElement *bin1, *bin2, *bin3, *bin4, *bin5;

/* start a bin with the given description */
static GstElement *
create_stream (const gchar * descr)
{
  GstElement *bin;
  GError *error = NULL;

  bin = gst_parse_launch (descr, &error);
  if (error) {
    g_print ("pipeline could not be constructed: %s\n", error->message);
    g_error_free (error);
    return NULL;
  }

  /* add the bin to the pipeline now, this will set the current base_time of the
   * pipeline on the new bin. */
  gst_bin_add (GST_BIN_CAST (pipeline), bin);

  return bin;
}

static gboolean
pause_play_stream (GstElement * bin, gint seconds)
{
  gboolean punch_in;
  GstStateChangeReturn ret;
  GstClockTime now, base_time, running_time;

  /* get current running time, we need this value to continue playback of
   * non-live pipelines. */
  now = gst_clock_get_time (theclock);
  base_time = gst_element_get_base_time (bin);

  running_time = now - base_time;

  /* set the new bin to PAUSED, the parent bin will notice (because of the ASYNC
   * message and will perform latency calculations again when going to PLAYING
   * later. */
  ret = gst_element_set_state (bin, GST_STATE_PAUSED);

  switch (ret) {
    case GST_STATE_CHANGE_NO_PREROLL:
      /* live source, timestamps are running_time of the pipeline clock. */
      punch_in = FALSE;
      break;
    case GST_STATE_CHANGE_SUCCESS:
      /* success, no async state changes, same as async, timestamps start
       * from 0 */
    case GST_STATE_CHANGE_ASYNC:
      /* no live source, bin will preroll. We have to punch it in because in
       * this situation timestamps start from 0.  */
      punch_in = TRUE;
      break;
    default:
    case GST_STATE_CHANGE_FAILURE:
      return FALSE;
  }

  if (seconds)
    g_usleep (seconds * G_USEC_PER_SEC);

  if (punch_in) {
    /* new bin has to be aligned with previous running_time. We do this by taking
     * the current absolute clock time and calculating the base time that would
     * give the previous running_time. We set this base_time on the bin before
     * setting it to PLAYING. */
    now = gst_clock_get_time (theclock);
    base_time = now - running_time;

    gst_element_set_base_time (bin, base_time);
  }

  /* now set the pipeline to PLAYING */
  gst_element_set_state (bin, GST_STATE_PLAYING);

  return TRUE;
}

static void
message_received (GstBus * bus, GstMessage * message, GstPipeline * pipeline)
{
  const GstStructure *s;

  s = gst_message_get_structure (message);
  g_print ("message from \"%s\" (%s): ",
      GST_STR_NULL (GST_ELEMENT_NAME (GST_MESSAGE_SRC (message))),
      gst_message_type_get_name (GST_MESSAGE_TYPE (message)));
  if (s) {
    gchar *sstr;

    sstr = gst_structure_to_string (s);
    g_print ("%s\n", sstr);
    g_free (sstr);
  } else {
    g_print ("no message details\n");
  }
}

static void
eos_message_received (GstBus * bus, GstMessage * message,
    GstPipeline * pipeline)
{
  message_received (bus, message, pipeline);
  g_main_loop_quit (loop);
}

static gboolean
perform_step (gpointer pstep)
{
  gint step = GPOINTER_TO_INT (pstep);
  GstElement *audiotestsrc,  *fakesink;

  switch (step) {
    case 0:
      /* live stream locks on to running_time, pipeline configures latency. */
      g_print ("creating bin1\n");
      bin1 =
          create_stream
          ("( v4l2src ! ffmpegcolorspace ! timeoverlay ! queue ! xvimagesink name=v4llive )");
      pause_play_stream (bin1, 0);
      g_timeout_add (1000, (GSourceFunc) perform_step, GINT_TO_POINTER (1));
      break;
    case 1:
      /* live stream locks on to running_time, pipeline reconfigures latency
       * together with the previously added bin so that they run synchronized. */
      g_print ("creating bin2\n");
      bin2 = create_stream ("( alsasrc ! queue ! alsasink name=alsalive )");
      pause_play_stream (bin2, 0);
      g_timeout_add (1000, (GSourceFunc) perform_step, GINT_TO_POINTER (2));
      break;
    case 2:
      /* non-live stream, need base_time to align with current running live sources. */
      g_print ("creating bin3\n");
     // bin3 = create_stream ("( audiotestsrc ! filesink name=sink )");
      bin3 = gst_pipeline_new ("audio-player");
      audiotestsrc = gst_element_factory_make ("audiotestsrc", NULL);
        fakesink = gst_element_factory_make ("fakesink", "sink");
        gst_bin_add_many (GST_BIN (bin3), audiotestsrc, fakesink,  NULL);
        gst_element_link_many (audiotestsrc, fakesink, NULL);
      
      pause_play_stream (bin3, 0);
      g_timeout_add (1000, (GSourceFunc) perform_step, GINT_TO_POINTER (6));
      break;
    case 3:
      g_print ("creating bin4\n");
      bin4 =
          create_stream
          ("( videotestsrc ! timeoverlay ! ffmpegcolorspace ! ximagesink name=vtnonlive )");
      pause_play_stream (bin4, 0);
      g_timeout_add (1000, (GSourceFunc) perform_step, GINT_TO_POINTER (4));
      break;
    case 4:
      /* live stream locks on to running_time */
      g_print ("creating bin5\n");
      bin5 =
          create_stream
          ("( videotestsrc is-live=1 ! timeoverlay ! ffmpegcolorspace ! ximagesink name=vtlive )");
      pause_play_stream (bin5, 0);
      g_timeout_add (1000, (GSourceFunc) perform_step, GINT_TO_POINTER (5));
      break;
    case 5:
      /* pause the fist live stream for 2 seconds */
      g_print ("PAUSE bin1 for 2 seconds\n");
      pause_play_stream (bin1, 2);
      /* pause the non-live stream for 2 seconds */
      g_print ("PAUSE bin4 for 2 seconds\n");
      pause_play_stream (bin4, 2);
      /* pause the pseudo live stream for 2 seconds */
      g_print ("PAUSE bin5 for 2 seconds\n");
      pause_play_stream (bin5, 2);
      g_print ("Waiting 5 seconds\n");
      g_timeout_add (5000, (GSourceFunc) perform_step, GINT_TO_POINTER (6));
      break;
    case 6:
      g_print ("quiting\n");
      g_main_loop_quit (loop);
      break;
    default:
      break;
  }
  return FALSE;
}

int
main (int argc, char *argv[])
{
  GstBus *bus;
  
  xmlfile = "addstream";
  std_log(LOG_FILENAME_LINE, "Test Started addstream");

  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, TRUE);

  pipeline = gst_pipeline_new ("pipeline");

  /* setup message handling */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_signal_watch_full (bus, G_PRIORITY_HIGH);
  g_signal_connect (bus, "message::error", (GCallback) message_received,
      pipeline);
  g_signal_connect (bus, "message::warning", (GCallback) message_received,
      pipeline);
  g_signal_connect (bus, "message::eos", (GCallback) eos_message_received,
      pipeline);

  /* we set the pipeline to PLAYING, this will distribute a default clock and
   * start running. no preroll is needed */
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* get the clock now. Since we never set the pipeline to PAUSED again, the
   * clock will not change, even when we add new clock providers later.  */
  theclock = gst_element_get_clock (pipeline);

  /* start our actions while we are in the mainloop so that we can catch errors
   * and other messages. */
  g_idle_add ((GSourceFunc) perform_step, GINT_TO_POINTER (2)),
      /* go to main loop */
      g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);

  gst_object_unref (bus);
  gst_object_unref (pipeline);
  gst_object_unref (theclock);

  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  return 0;
}
