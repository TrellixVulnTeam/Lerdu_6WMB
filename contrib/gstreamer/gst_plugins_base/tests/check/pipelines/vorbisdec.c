/* GStreamer
 *
 * unit test for vorbisdec
 *
 * Copyright (C) 2007 Thomas Vander Stichele <thomas at apestaart dot org>
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

#include <gst/check/gstcheck.h>
#include <gst/check/gstbufferstraw.h>

#ifndef GST_DISABLE_PARSE

static GMainLoop *loop;
static gint messages = 0;

static void
element_message_cb (GstBus * bus, GstMessage * message, gpointer user_data)
{
  gchar *s;

  s = gst_structure_to_string (gst_message_get_structure (message));
  GST_DEBUG ("Received message: %s", s);
  g_free (s);

  messages++;
}

static void
eos_message_cb (GstBus * bus, GstMessage * message, gpointer user_data)
{
  GST_DEBUG ("Received eos");
  g_main_loop_quit (loop);
}

GST_START_TEST (test_timestamps)
{
  GstElement *pipeline;
  gchar *pipe_str;
  GstBus *bus;
  GError *error = NULL;

  pipe_str = g_strdup_printf ("audiotestsrc num-buffers=100"
      " ! audio/x-raw-int,rate=44100 ! audioconvert ! vorbisenc ! vorbisdec"
      " ! identity check-imperfect-timestamp=TRUE ! fakesink");

  pipeline = gst_parse_launch (pipe_str, &error);
  fail_unless (pipeline != NULL, "Error parsing pipeline: %s",
      error ? error->message : "(invalid error)");
  g_free (pipe_str);

  bus = gst_element_get_bus (pipeline);
  fail_if (bus == NULL);
  gst_bus_add_signal_watch (bus);
  g_signal_connect (bus, "message::element", (GCallback) element_message_cb,
      NULL);
  g_signal_connect (bus, "message::eos", (GCallback) eos_message_cb, NULL);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* run until we receive EOS */
  loop = g_main_loop_new (NULL, FALSE);

  g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);

  /* FIXME: there seems to be a bug in vorbisdec on decoding the last packet
   * where it calculates the timestamp based on the granulepos of the incoming
   * packet and subtracting the number of samples it can decode, which can
   * result in a discontinuity in timestamps.
   * See http://bugzilla.gnome.org/show_bug.cgi?id=423086 
   * Fix that bug and drop this number to 0.
   */
  fail_if (messages > 1, "Received imperfect timestamp messages");
  gst_object_unref (pipeline);
}

GST_END_TEST;
#endif /* #ifndef GST_DISABLE_PARSE */

static Suite *
vorbisenc_suite (void)
{
  Suite *s = suite_create ("vorbisenc");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
#ifndef GST_DISABLE_PARSE
  tcase_add_test (tc_chain, test_timestamps);
#endif

  return s;
}

int
main (int argc, char **argv)
{
  int nf;

  Suite *s = vorbisenc_suite ();
  SRunner *sr = srunner_create (s);

  gst_check_init (&argc, &argv);

  srunner_run_all (sr, CK_NORMAL);
  nf = srunner_ntests_failed (sr);
  srunner_free (sr);

  return nf;
}
