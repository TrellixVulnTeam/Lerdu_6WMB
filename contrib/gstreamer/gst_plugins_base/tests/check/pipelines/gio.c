/* GStreamer
 *
 * unit test for GIO
 *
 * Copyright (C) 2007 Sebastian Dröge <slomo@circular-chaos.org>
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
#include <gio/gio.h>

static gboolean got_eos = FALSE;

static gboolean
message_handler (GstBus * bus, GstMessage * msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      got_eos = TRUE;
      g_main_loop_quit (loop);
      break;
    case GST_MESSAGE_ERROR:{
      gchar *debug;
      GError *err;

      gst_message_parse_error (msg, &err, &debug);
      g_free (debug);

      /* Will abort the check */
      g_warning ("Error: %s\n", err->message);
      g_error_free (err);

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}

GST_START_TEST (test_memory_stream)
{
  GMainLoop *loop;
  GstElement *bin;
  GstElement *src, *sink;
  GstBus *bus;

  GMemoryInputStream *input;
  GMemoryOutputStream *output;

  guint8 *in_data;
  guint8 *out_data;
  gint i;
  GstFormat fmt = GST_FORMAT_BYTES;
  gint64 duration;

  got_eos = FALSE;

  in_data = g_new (guint8, 512);
  out_data = g_new (guint8, 512);
  for (i = 0; i < 512; i++)
    in_data[i] = i % 256;

  input =
      G_MEMORY_INPUT_STREAM (g_memory_input_stream_new_from_data (in_data, 512,
          (GDestroyNotify) g_free));

  output = G_MEMORY_OUTPUT_STREAM (g_memory_output_stream_new (out_data, 512,
          (GReallocFunc) g_realloc, (GDestroyNotify) g_free));
  out_data = NULL;

  loop = g_main_loop_new (NULL, FALSE);

  bin = gst_pipeline_new ("bin");

  src = gst_element_factory_make ("giostreamsrc", "src");
  fail_unless (src != NULL);
  g_object_set (G_OBJECT (src), "stream", input, NULL);

  sink = gst_element_factory_make ("giostreamsink", "sink");
  fail_unless (sink != NULL);
  g_object_set (G_OBJECT (sink), "stream", output, NULL);

  gst_bin_add_many (GST_BIN (bin), src, sink, NULL);

  fail_unless (gst_element_link_many (src, sink, NULL));

  bus = gst_element_get_bus (bin);
  gst_bus_add_watch (bus, message_handler, loop);
  gst_object_unref (bus);

  gst_element_set_state (bin, GST_STATE_PAUSED);

  fail_unless (gst_element_query_duration (bin, &fmt, &duration));
  fail_unless_equals_int (duration, 512);

  gst_element_set_state (bin, GST_STATE_PLAYING);

  g_main_loop_run (loop);

  gst_element_set_state (bin, GST_STATE_NULL);
  gst_object_unref (bin);

  fail_unless (got_eos);

  out_data = g_memory_output_stream_get_data (G_MEMORY_OUTPUT_STREAM (output));

  for (i = 0; i < 512; i++)
    fail_unless_equals_int (in_data[i], out_data[i]);

  g_object_unref (input);
  g_object_unref (output);

  g_main_loop_unref (loop);
}

GST_END_TEST;

static Suite *
gio_testsuite (void)
{
  Suite *s = suite_create ("gio");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_memory_stream);

  return s;
}

int
main (int argc, char **argv)
{
  int nf;

  Suite *s = gio_testsuite ();
  SRunner *sr = srunner_create (s);

  gst_check_init (&argc, &argv);

  srunner_run_all (sr, CK_NORMAL);
  nf = srunner_ntests_failed (sr);
  srunner_free (sr);

  return nf;
}
