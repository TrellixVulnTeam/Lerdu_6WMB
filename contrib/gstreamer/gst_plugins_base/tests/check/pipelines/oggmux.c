/* GStreamer
 *
 * unit tests for oggmux
 *
 * Copyright (C) 2006 James Livingston <doclivingston at gmail.com>
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

#include <string.h>

#include <gst/check/gstcheck.h>
#include <ogg/ogg.h>


typedef enum
{
  CODEC_UNKNOWN,
  CODEC_VORBIS,
  CODEC_THEORA,
  CODEC_SPEEX,
} ChainCodec;

typedef struct
{
  gboolean eos;
  gulong serialno;
  gint64 last_granule;
  ChainCodec codec;
} ChainState;

static ogg_sync_state oggsync;
static GHashTable *eos_chain_states;
static gulong probe_id;


static ChainCodec
get_page_codec (ogg_page * page)
{
  ChainCodec codec = CODEC_UNKNOWN;
  ogg_stream_state state;
  ogg_packet packet;

  ogg_stream_init (&state, ogg_page_serialno (page));
  if (ogg_stream_pagein (&state, page) == 0) {
    if (ogg_stream_packetpeek (&state, &packet) > 0) {
      if (strncmp ((char *) &packet.packet[1], "vorbis",
              strlen ("vorbis")) == 0)
        codec = CODEC_VORBIS;
      else if (strncmp ((char *) &packet.packet[1], "theora",
              strlen ("theora")) == 0)
        codec = CODEC_THEORA;
      else if (strncmp ((char *) &packet.packet[0], "Speex   ",
              strlen ("Speex   ")) == 0)
        codec = CODEC_SPEEX;
    }
  }
  ogg_stream_clear (&state);

  return codec;
}

static gboolean
check_chain_final_state (gpointer key, ChainState * state, gpointer data)
{
  fail_unless (state->eos, "missing EOS flag on chain %u", state->serialno);

  /* return TRUE to empty the chain table */
  return TRUE;
}

static void
fail_if_audio (gpointer key, ChainState * state, gpointer data)
{
  fail_if (state->codec == CODEC_VORBIS,
      "vorbis BOS occurred before theora BOS");
  fail_if (state->codec == CODEC_SPEEX, "speex BOS occurred before theora BOS");
}

static ChainState *
validate_ogg_page (ogg_page * page)
{
  gulong serialno;
  gint64 granule;
  ChainState *state;

  serialno = ogg_page_serialno (page);
  granule = ogg_page_granulepos (page);
  state = g_hash_table_lookup (eos_chain_states, GINT_TO_POINTER (serialno));

  fail_if (ogg_page_packets (page) == 0 && granule != -1,
      "Must have granulepos -1 when page has no packets, has %" G_GINT64_FORMAT,
      granule);

  if (ogg_page_bos (page)) {
    fail_unless (state == NULL, "Extraneous BOS flag on chain %u", serialno);

    state = g_new0 (ChainState, 1);
    g_hash_table_insert (eos_chain_states, GINT_TO_POINTER (serialno), state);
    state->serialno = serialno;
    state->last_granule = granule;
    state->codec = get_page_codec (page);

    /* check for things like BOS ordering, etc */
    switch (state->codec) {
      case CODEC_THEORA:
        /* check we have no vorbis/speex chains yet */
        g_hash_table_foreach (eos_chain_states, (GHFunc) fail_if_audio, NULL);
        break;
      case CODEC_VORBIS:
      case CODEC_SPEEX:
        /* no checks (yet) */
        break;
      case CODEC_UNKNOWN:
      default:
        break;
    }
  } else if (ogg_page_eos (page)) {
    fail_unless (state != NULL, "Missing BOS flag on chain %u", serialno);
    state->eos = TRUE;
  } else {
    fail_unless (state != NULL, "Missing BOS flag on chain %u", serialno);
    fail_unless (!state->eos, "Data after EOS flag on chain %u", serialno);
  }

  if (granule != -1) {
    fail_unless (granule >= state->last_granule,
        "Granulepos out-of-order for chain %u: old=%" G_GINT64_FORMAT ", new="
        G_GINT64_FORMAT, serialno, state->last_granule, granule);
    state->last_granule = granule;
  }

  return state;
}

static void
is_video (gpointer key, ChainState * state, gpointer data)
{
  if (state->codec == CODEC_THEORA)
    *((gboolean *) data) = TRUE;
}


static gboolean
eos_buffer_probe (GstPad * pad, GstBuffer * buffer, gpointer unused)
{
  gint ret;
  gint size;
  guint8 *data;
  gchar *oggbuffer;
  ChainState *state = NULL;
  gboolean has_video = FALSE;

  size = GST_BUFFER_SIZE (buffer);
  data = GST_BUFFER_DATA (buffer);

  oggbuffer = ogg_sync_buffer (&oggsync, size);
  memcpy (oggbuffer, data, size);
  ogg_sync_wrote (&oggsync, size);

  do {
    ogg_page page;

    ret = ogg_sync_pageout (&oggsync, &page);
    if (ret > 0)
      state = validate_ogg_page (&page);
  }
  while (ret != 0);

  if (state) {
    /* Now, we can do buffer-level checks...
     * If we have video somewhere, then we should have DELTA_UNIT set on all
     * non-header (not IN_CAPS), non-video buffers
     */
    g_hash_table_foreach (eos_chain_states, (GHFunc) is_video, &has_video);
    if (has_video && state->codec != CODEC_THEORA) {
      if (!GST_BUFFER_FLAG_IS_SET (buffer, GST_BUFFER_FLAG_IN_CAPS))
        fail_unless (GST_BUFFER_FLAG_IS_SET (buffer,
                GST_BUFFER_FLAG_DELTA_UNIT),
            "Non-video buffer doesn't have DELTA_UNIT in stream with video");
    }
  }

  return TRUE;
}

static void
start_pipeline (GstElement * bin, GstPad * pad)
{
  GstStateChangeReturn ret;

  ogg_sync_init (&oggsync);

  eos_chain_states =
      g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);
  probe_id =
      gst_pad_add_buffer_probe (pad, G_CALLBACK (eos_buffer_probe), NULL);

  ret = gst_element_set_state (bin, GST_STATE_PLAYING);
  fail_if (ret == GST_STATE_CHANGE_FAILURE, "Could not start test pipeline");
  if (ret == GST_STATE_CHANGE_ASYNC) {
    ret = gst_element_get_state (bin, NULL, NULL, GST_CLOCK_TIME_NONE);
    fail_if (ret != GST_STATE_CHANGE_SUCCESS, "Could not start test pipeline");
  }
}

static void
stop_pipeline (GstElement * bin, GstPad * pad)
{
  GstStateChangeReturn ret;

  ret = gst_element_set_state (bin, GST_STATE_NULL);
  fail_if (ret == GST_STATE_CHANGE_FAILURE, "Could not stop test pipeline");
  if (ret == GST_STATE_CHANGE_ASYNC) {
    ret = gst_element_get_state (bin, NULL, NULL, GST_CLOCK_TIME_NONE);
    fail_if (ret != GST_STATE_CHANGE_SUCCESS, "Could not stop test pipeline");
  }

  gst_pad_remove_buffer_probe (pad, (guint) probe_id);
  ogg_sync_clear (&oggsync);

  /* check end conditions, such as EOS flags */
  g_hash_table_foreach_remove (eos_chain_states,
      (GHRFunc) check_chain_final_state, NULL);
}

static gboolean
eos_watch (GstBus * bus, GstMessage * message, GMainLoop * loop)
{
  if (GST_MESSAGE_TYPE (message) == GST_MESSAGE_EOS) {
    g_main_loop_quit (loop);
  }
  return TRUE;
}

static void
test_pipeline (const char *pipeline)
{
  GstElement *bin, *sink;
  GstPad *pad, *sinkpad;
  GstBus *bus;
  GError *error = NULL;
  GMainLoop *loop;
  GstPadLinkReturn linkret;

  bin = gst_parse_launch (pipeline, &error);
  fail_unless (bin != NULL, "Error parsing pipeline: %s",
      error ? error->message : "(invalid error)");
  pad = gst_bin_find_unconnected_pad (GST_BIN (bin), GST_PAD_SRC);
  fail_unless (pad != NULL, "Could not locate free src pad");

  /* connect the fake sink */
  sink = gst_element_factory_make ("fakesink", "fake_sink");
  fail_unless (sink != NULL, "Could create fakesink");
  fail_unless (gst_bin_add (GST_BIN (bin), sink), "Could not insert fakesink");
  sinkpad = gst_element_get_pad (sink, "sink");
  fail_unless (sinkpad != NULL, "Could not get fakesink src pad");

  linkret = gst_pad_link (pad, sinkpad);
  fail_unless (GST_PAD_LINK_SUCCESSFUL (linkret),
      "Could not link to fake sink");
  gst_object_unref (sinkpad);

  /* run until we receive EOS */
  loop = g_main_loop_new (NULL, FALSE);
  bus = gst_element_get_bus (bin);
  gst_bus_add_watch (bus, (GstBusFunc) eos_watch, loop);
  gst_object_unref (bus);

  start_pipeline (bin, pad);
  g_main_loop_run (loop);
  stop_pipeline (bin, pad);

  /* clean up */
  g_main_loop_unref (loop);
  gst_object_unref (pad);
  gst_object_unref (bin);
}

GST_START_TEST (test_vorbis)
{
  test_pipeline
      ("audiotestsrc num-buffers=5 ! audioconvert ! vorbisenc ! oggmux");
}

GST_END_TEST;

GST_START_TEST (test_theora)
{
  test_pipeline
      ("videotestsrc num-buffers=5 ! ffmpegcolorspace ! theoraenc ! oggmux");
}

GST_END_TEST;

GST_START_TEST (test_theora_vorbis)
{
  test_pipeline
      ("videotestsrc num-buffers=10 ! ffmpegcolorspace ! theoraenc ! queue ! oggmux name=mux "
      "audiotestsrc num-buffers=2 ! audioconvert ! vorbisenc ! queue ! mux.");
}

GST_END_TEST;

GST_START_TEST (test_vorbis_theora)
{
  test_pipeline
      ("videotestsrc num-buffers=2 ! ffmpegcolorspace ! theoraenc ! queue ! oggmux name=mux "
      "audiotestsrc num-buffers=10 ! audioconvert ! vorbisenc ! queue ! mux.");
}

GST_END_TEST;

GST_START_TEST (test_simple_cleanup)
{
  GstElement *oggmux;

  oggmux = gst_element_factory_make ("oggmux", NULL);
  gst_object_unref (oggmux);
}

GST_END_TEST;

GST_START_TEST (test_request_pad_cleanup)
{
  GstElement *oggmux;
  GstPad *pad;

  oggmux = gst_element_factory_make ("oggmux", NULL);
  pad = gst_element_get_request_pad (oggmux, "sink_%d");
  fail_unless (pad != NULL);
  gst_object_unref (pad);
  pad = gst_element_get_request_pad (oggmux, "sink_%d");
  fail_unless (pad != NULL);
  gst_object_unref (pad);
  gst_object_unref (oggmux);
}

GST_END_TEST;

static Suite *
oggmux_suite (void)
{
  Suite *s = suite_create ("oggmux");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
#ifdef HAVE_VORBIS
  tcase_add_test (tc_chain, test_vorbis);
#endif

#ifdef HAVE_THEORA
  tcase_add_test (tc_chain, test_theora);
#endif

#if (defined (HAVE_THEORA) && defined (HAVE_VORBIS))
  tcase_add_test (tc_chain, test_vorbis_theora);
  tcase_add_test (tc_chain, test_theora_vorbis);
#endif

  tcase_add_test (tc_chain, test_simple_cleanup);
  tcase_add_test (tc_chain, test_request_pad_cleanup);
  return s;
}

GST_CHECK_MAIN (oggmux);
