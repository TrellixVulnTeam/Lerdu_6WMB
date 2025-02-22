/* GStreamer
 *
 * unit test for videorate
 *
 * Copyright (C) 2006 Thomas Vander Stichele <thomas at apestaart dot org>
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

#include <unistd.h>

#include <gst/check/gstcheck.h>

/* For ease of programming we use globals to keep refs for our floating
 * src and sink pads we create; otherwise we always have to do get_pad,
 * get_peer, and then remove references in every test function */
static GstPad *mysrcpad, *mysinkpad;


#define VIDEO_CAPS_TEMPLATE_STRING     \
    "video/x-raw-yuv"

#define VIDEO_CAPS_STRING               \
    "video/x-raw-yuv, "                 \
    "width = (int) 320, "               \
    "height = (int) 240, "              \
    "framerate = (fraction) 25/1 , "    \
    "format = (fourcc) I420"

#define VIDEO_CAPS_NO_FRAMERATE_STRING  \
    "video/x-raw-yuv, "                 \
    "width = (int) 320, "               \
    "height = (int) 240, "              \
    "format = (fourcc) I420"

#define VIDEO_CAPS_NEWSIZE_STRING       \
    "video/x-raw-yuv, "                 \
    "width = (int) 240, "               \
    "height = (int) 120, "              \
    "framerate = (fraction) 25/1 , "	\
    "format = (fourcc) I420"

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (VIDEO_CAPS_TEMPLATE_STRING)
    );
static GstStaticPadTemplate srctemplate = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (VIDEO_CAPS_TEMPLATE_STRING)
    );

static void
assert_videorate_stats (GstElement * videorate, gchar * reason,
    guint64 xin, guint64 xout, guint64 xdropped, guint64 xduplicated)
{
  guint64 in, out, dropped, duplicated;

  g_object_get (videorate, "in", &in, "out", &out, "drop", &dropped,
      "duplicate", &duplicated, NULL);
#define _assert_equals_uint64(a, b)                                     \
G_STMT_START {                                                          \
  guint64 first = a;                                                    \
  guint64 second = b;                                                   \
  fail_unless(first == second,                                          \
    "%s: '" #a "' (%" G_GUINT64_FORMAT ") is not equal to "		\
    "expected '" #a"' (%" G_GUINT64_FORMAT ")", reason, first, second); \
} G_STMT_END;


  _assert_equals_uint64 (in, xin);
  _assert_equals_uint64 (out, xout);
  _assert_equals_uint64 (dropped, xdropped);
  _assert_equals_uint64 (duplicated, xduplicated);
}

static GstElement *
setup_videorate (void)
{
  GstElement *videorate;

  GST_DEBUG ("setup_videorate");
  videorate = gst_check_setup_element ("videorate");
  mysrcpad = gst_check_setup_src_pad (videorate, &srctemplate, NULL);
  mysinkpad = gst_check_setup_sink_pad (videorate, &sinktemplate, NULL);
  gst_pad_set_active (mysrcpad, TRUE);
  gst_pad_set_active (mysinkpad, TRUE);

  return videorate;
}

static void
cleanup_videorate (GstElement * videorate)
{
  GST_DEBUG ("cleanup_videorate");

  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;

  gst_element_set_state (videorate, GST_STATE_NULL);
  gst_element_get_state (videorate, NULL, NULL, GST_CLOCK_TIME_NONE);
  gst_pad_set_active (mysrcpad, FALSE);
  gst_pad_set_active (mysinkpad, FALSE);
  gst_check_teardown_src_pad (videorate);
  gst_check_teardown_sink_pad (videorate);
  gst_check_teardown_element (videorate);
}

GST_START_TEST (test_one)
{
  GstElement *videorate;
  GstBuffer *inbuffer;
  GstCaps *caps;

  videorate = setup_videorate ();
  fail_unless (gst_element_set_state (videorate,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  inbuffer = gst_buffer_new_and_alloc (4);
  memset (GST_BUFFER_DATA (inbuffer), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (inbuffer, caps);
  GST_BUFFER_TIMESTAMP (inbuffer) = 0;
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  /* pushing gives away my reference ... */
  fail_unless (gst_pad_push (mysrcpad, inbuffer) == GST_FLOW_OK);
  /* ... and it is now stuck inside videorate */
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);
  fail_unless_equals_int (g_list_length (buffers), 0);

  /* cleanup */
  cleanup_videorate (videorate);
}

GST_END_TEST;

GST_START_TEST (test_more)
{
  GstElement *videorate;
  GstBuffer *first, *second, *third, *outbuffer;
  GList *l;
  GstCaps *caps;
  GRand *rand;

  videorate = setup_videorate ();
  fail_unless (gst_element_set_state (videorate,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");
  assert_videorate_stats (videorate, "creation", 0, 0, 0, 0);

  rand = g_rand_new ();

  /* first buffer */
  first = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (first) = 0;
  /* it shouldn't matter what the offsets are, videorate produces perfect
     streams */
  GST_BUFFER_OFFSET (first) = g_rand_int (rand);
  GST_BUFFER_OFFSET_END (first) = g_rand_int (rand);
  memset (GST_BUFFER_DATA (first), 1, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (first, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (first, "first", 1);
  gst_buffer_ref (first);

  /* pushing gives away my reference ... */
  fail_unless (gst_pad_push (mysrcpad, first) == GST_FLOW_OK);
  /* ... and it is now stuck inside videorate */
  ASSERT_BUFFER_REFCOUNT (first, "first", 2);
  fail_unless_equals_int (g_list_length (buffers), 0);
  assert_videorate_stats (videorate, "first buffer", 1, 0, 0, 0);

  /* second buffer; inbetween second and third output frame's timestamp */
  second = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (second) = GST_SECOND * 3 / 50;
  GST_BUFFER_OFFSET (first) = g_rand_int (rand);
  GST_BUFFER_OFFSET_END (first) = g_rand_int (rand);
  memset (GST_BUFFER_DATA (second), 2, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (second, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (second, "second", 1);
  gst_buffer_ref (second);

  /* pushing gives away one of my references ... */
  fail_unless (gst_pad_push (mysrcpad, second) == GST_FLOW_OK);
  /* ... and it is now stuck inside videorate */
  ASSERT_BUFFER_REFCOUNT (second, "second", 2);

  /* ... and the first one is pushed out, with timestamp 0 */
  fail_unless_equals_int (g_list_length (buffers), 1);
  assert_videorate_stats (videorate, "second buffer", 2, 1, 0, 0);
  ASSERT_BUFFER_REFCOUNT (first, "first", 2);

  outbuffer = buffers->data;
  fail_unless_equals_uint64 (GST_BUFFER_TIMESTAMP (outbuffer), 0);

  /* third buffer */
  third = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (third) = GST_SECOND * 12 / 50;
  GST_BUFFER_OFFSET (first) = g_rand_int (rand);
  GST_BUFFER_OFFSET_END (first) = g_rand_int (rand);
  memset (GST_BUFFER_DATA (third), 3, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (third, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (third, "third", 1);
  gst_buffer_ref (third);

  /* pushing gives away my reference ... */
  fail_unless (gst_pad_push (mysrcpad, third) == GST_FLOW_OK);
  /* ... and it is now stuck inside videorate */
  ASSERT_BUFFER_REFCOUNT (third, "third", 2);

  /* submitting the third buffer has triggered flushing of three more frames */
  assert_videorate_stats (videorate, "third buffer", 3, 4, 0, 2);

  /* check timestamp and source correctness */
  l = buffers;
  fail_unless_equals_uint64 (GST_BUFFER_TIMESTAMP (l->data), 0);
  fail_unless_equals_int (GST_BUFFER_DATA (l->data)[0], 1);
  fail_unless_equals_uint64 (GST_BUFFER_OFFSET (l->data), 0);
  fail_unless_equals_uint64 (GST_BUFFER_OFFSET_END (l->data), 1);

  l = g_list_next (l);
  fail_unless_equals_uint64 (GST_BUFFER_TIMESTAMP (l->data), GST_SECOND / 25);
  fail_unless_equals_int (GST_BUFFER_DATA (l->data)[0], 2);
  fail_unless_equals_uint64 (GST_BUFFER_OFFSET (l->data), 1);
  fail_unless_equals_uint64 (GST_BUFFER_OFFSET_END (l->data), 2);

  l = g_list_next (l);
  fail_unless_equals_uint64 (GST_BUFFER_TIMESTAMP (l->data),
      GST_SECOND * 2 / 25);
  fail_unless_equals_int (GST_BUFFER_DATA (l->data)[0], 2);
  fail_unless_equals_uint64 (GST_BUFFER_OFFSET (l->data), 2);
  fail_unless_equals_uint64 (GST_BUFFER_OFFSET_END (l->data), 3);

  l = g_list_next (l);
  fail_unless_equals_uint64 (GST_BUFFER_TIMESTAMP (l->data),
      GST_SECOND * 3 / 25);
  fail_unless_equals_int (GST_BUFFER_DATA (l->data)[0], 2);
  fail_unless_equals_uint64 (GST_BUFFER_OFFSET (l->data), 3);
  fail_unless_equals_uint64 (GST_BUFFER_OFFSET_END (l->data), 4);

  fail_unless_equals_int (g_list_length (buffers), 4);
  /* one held by us, three held by each output frame taken from the second */
  ASSERT_BUFFER_REFCOUNT (second, "second", 4);

  /* now send EOS */
  fail_unless (gst_pad_push_event (mysrcpad, gst_event_new_eos ()));

  /* submitting eos should flush out two more frames for tick 8 and 10 */
  /* FIXME: right now it only flushes out one, so out is 5 instead of 6 ! */
  assert_videorate_stats (videorate, "eos", 3, 5, 0, 2);
  fail_unless_equals_int (g_list_length (buffers), 5);

  /* cleanup */
  g_rand_free (rand);
  gst_buffer_unref (first);
  gst_buffer_unref (second);
  gst_buffer_unref (third);
  cleanup_videorate (videorate);
}

GST_END_TEST;

/* frames at 1, 0, 2 -> second one should be ignored */
GST_START_TEST (test_wrong_order_from_zero)
{
  GstElement *videorate;
  GstBuffer *first, *second, *third, *outbuffer;
  GstCaps *caps;

  videorate = setup_videorate ();
  fail_unless (gst_element_set_state (videorate,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");
  assert_videorate_stats (videorate, "start", 0, 0, 0, 0);

  /* first buffer */
  first = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (first) = GST_SECOND;
  memset (GST_BUFFER_DATA (first), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (first, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (first, "first", 1);
  gst_buffer_ref (first);

  GST_DEBUG ("pushing first buffer");
  /* pushing gives away my reference ... */
  fail_unless (gst_pad_push (mysrcpad, first) == GST_FLOW_OK);
  /* ... and it is now stuck inside videorate */
  ASSERT_BUFFER_REFCOUNT (first, "first", 2);
  fail_unless_equals_int (g_list_length (buffers), 0);
  assert_videorate_stats (videorate, "first", 1, 0, 0, 0);

  /* second buffer */
  second = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (second) = 0;
  memset (GST_BUFFER_DATA (second), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (second, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (second, "second", 1);
  gst_buffer_ref (second);

  /* pushing gives away my reference ... */
  fail_unless (gst_pad_push (mysrcpad, second) == GST_FLOW_OK);
  /* ... and it is now dropped because it is too old */
  ASSERT_BUFFER_REFCOUNT (second, "second", 1);
  fail_unless_equals_int (g_list_length (buffers), 0);

  /* ... and the first one is still there */
  assert_videorate_stats (videorate, "second", 2, 0, 1, 0);
  ASSERT_BUFFER_REFCOUNT (first, "first", 2);

  /* third buffer */
  third = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (third) = 2 * GST_SECOND;
  memset (GST_BUFFER_DATA (third), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (third, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (third, "third", 1);
  gst_buffer_ref (third);

  /* pushing gives away my reference ... */
  fail_unless (gst_pad_push (mysrcpad, third) == GST_FLOW_OK);
  /* ... and it is now stuck inside videorate */
  ASSERT_BUFFER_REFCOUNT (third, "third", 2);

  /* and now the first one should be pushed once and dupped 24 + 13 times, to
   * reach the half point between 1 s (first) and 2 s (third) */
  fail_unless_equals_int (g_list_length (buffers), 38);
  ASSERT_BUFFER_REFCOUNT (first, "first", 39);
  ASSERT_BUFFER_REFCOUNT (second, "second", 1);
  ASSERT_BUFFER_REFCOUNT (third, "third", 2);
  assert_videorate_stats (videorate, "third", 3, 38, 1, 37);

  /* verify last buffer */
  outbuffer = g_list_last (buffers)->data;
  fail_unless (GST_IS_BUFFER (outbuffer));
  fail_unless_equals_uint64 (GST_BUFFER_TIMESTAMP (outbuffer),
      GST_SECOND * 37 / 25);

  /* cleanup */
  gst_buffer_unref (first);
  gst_buffer_unref (second);
  gst_buffer_unref (third);
  cleanup_videorate (videorate);
}

GST_END_TEST;

/* send frames with 0, 1, 2, 0 seconds */
GST_START_TEST (test_wrong_order)
{
  GstElement *videorate;
  GstBuffer *first, *second, *third, *fourth, *outbuffer;
  GstCaps *caps;

  videorate = setup_videorate ();
  fail_unless (gst_element_set_state (videorate,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");
  assert_videorate_stats (videorate, "start", 0, 0, 0, 0);

  /* first buffer */
  first = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (first) = 0;
  memset (GST_BUFFER_DATA (first), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (first, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (first, "first", 1);
  gst_buffer_ref (first);

  GST_DEBUG ("pushing first buffer");
  /* pushing gives away my reference ... */
  fail_unless (gst_pad_push (mysrcpad, first) == GST_FLOW_OK);
  /* ... and it is now stuck inside videorate */
  ASSERT_BUFFER_REFCOUNT (first, "first", 2);
  fail_unless_equals_int (g_list_length (buffers), 0);
  assert_videorate_stats (videorate, "first", 1, 0, 0, 0);

  /* second buffer */
  second = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (second) = GST_SECOND;
  memset (GST_BUFFER_DATA (second), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (second, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (second, "second", 1);
  gst_buffer_ref (second);

  /* pushing gives away my reference ... */
  fail_unless (gst_pad_push (mysrcpad, second) == GST_FLOW_OK);
  /* ... and it is now stuck inside videorate */
  ASSERT_BUFFER_REFCOUNT (second, "second", 2);
  /* and it created 13 output buffers as copies of the first frame */
  fail_unless_equals_int (g_list_length (buffers), 13);
  assert_videorate_stats (videorate, "second", 2, 13, 0, 12);
  ASSERT_BUFFER_REFCOUNT (first, "first", 14);

  /* third buffer */
  third = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (third) = 2 * GST_SECOND;
  memset (GST_BUFFER_DATA (third), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (third, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (third, "third", 1);
  gst_buffer_ref (third);

  /* pushing gives away my reference ... */
  fail_unless (gst_pad_push (mysrcpad, third) == GST_FLOW_OK);
  /* ... and it is now stuck inside videorate */
  ASSERT_BUFFER_REFCOUNT (third, "third", 2);

  /* submitting a frame with 2 seconds triggers output of 25 more frames */
  fail_unless_equals_int (g_list_length (buffers), 38);
  ASSERT_BUFFER_REFCOUNT (first, "first", 14);
  ASSERT_BUFFER_REFCOUNT (second, "second", 26);
  /* three frames submitted; two of them output as is, and 36 duplicated */
  assert_videorate_stats (videorate, "third", 3, 38, 0, 36);

  /* fourth buffer */
  fourth = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (fourth) = 0;
  memset (GST_BUFFER_DATA (fourth), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (fourth, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (fourth, "fourth", 1);
  gst_buffer_ref (fourth);

  /* pushing gives away my reference ... */
  fail_unless (gst_pad_push (mysrcpad, fourth) == GST_FLOW_OK);
  /* ... and it is dropped */
  ASSERT_BUFFER_REFCOUNT (fourth, "fourth", 1);

  fail_unless_equals_int (g_list_length (buffers), 38);
  ASSERT_BUFFER_REFCOUNT (first, "first", 14);
  ASSERT_BUFFER_REFCOUNT (second, "second", 26);
  assert_videorate_stats (videorate, "fourth", 4, 38, 1, 36);

  /* verify last buffer */
  outbuffer = g_list_last (buffers)->data;
  fail_unless (GST_IS_BUFFER (outbuffer));
  fail_unless_equals_uint64 (GST_BUFFER_TIMESTAMP (outbuffer),
      GST_SECOND * 37 / 25);


  /* cleanup */
  gst_buffer_unref (first);
  gst_buffer_unref (second);
  gst_buffer_unref (third);
  gst_buffer_unref (fourth);
  cleanup_videorate (videorate);
}

GST_END_TEST;


/* if no framerate is negotiated, we should not be able to push a buffer */
GST_START_TEST (test_no_framerate)
{
  GstElement *videorate;
  GstBuffer *inbuffer;
  GstCaps *caps;

  videorate = setup_videorate ();
  fail_unless (gst_element_set_state (videorate,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  inbuffer = gst_buffer_new_and_alloc (4);
  memset (GST_BUFFER_DATA (inbuffer), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_NO_FRAMERATE_STRING);
  gst_buffer_set_caps (inbuffer, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  /* take a ref so we can later check refcount */
  gst_buffer_ref (inbuffer);

  /* no framerate is negotiated so pushing should fail */
  fail_if (gst_pad_push (mysrcpad, inbuffer) == GST_FLOW_OK);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);
  gst_buffer_unref (inbuffer);
  fail_unless_equals_int (g_list_length (buffers), 0);

  /* cleanup */
  cleanup_videorate (videorate);
}

GST_END_TEST;

/* This test outputs 2 buffers of same dimensions (320x240), then 1 buffer of 
 * differing dimensions (240x120), and then another buffer of previous 
 * dimensions (320x240) and checks that the 3 buffers output as a result have 
 * correct caps (first 2 with 320x240 and 3rd with 240x120).
 */
GST_START_TEST (test_changing_size)
{
  GstElement *videorate;
  GstBuffer *first;
  GstBuffer *second;
  GstBuffer *third;
  GstBuffer *fourth;
  GstBuffer *fifth;
  GstBuffer *outbuf;
  GstEvent *newsegment;
  GstCaps *caps, *caps_newsize;

  videorate = setup_videorate ();
  fail_unless (gst_element_set_state (videorate,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  newsegment = gst_event_new_new_segment (FALSE, 1.0, GST_FORMAT_TIME, 0, -1,
      0);
  fail_unless (gst_pad_push_event (mysrcpad, newsegment) == TRUE);

  first = gst_buffer_new_and_alloc (4);
  memset (GST_BUFFER_DATA (first), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  GST_BUFFER_TIMESTAMP (first) = 0;
  gst_buffer_set_caps (first, caps);

  GST_DEBUG ("pushing first buffer");
  fail_unless (gst_pad_push (mysrcpad, first) == GST_FLOW_OK);

  /* second buffer */
  second = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (second) = GST_SECOND / 25;
  memset (GST_BUFFER_DATA (second), 0, 4);
  gst_buffer_set_caps (second, caps);

  fail_unless (gst_pad_push (mysrcpad, second) == GST_FLOW_OK);
  fail_unless_equals_int (g_list_length (buffers), 1);
  outbuf = buffers->data;
  /* first buffer should be output here */
  fail_unless (gst_caps_is_equal (GST_BUFFER_CAPS (outbuf), caps));
  fail_unless (GST_BUFFER_TIMESTAMP (outbuf) == 0);

  /* third buffer with new size */
  third = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (third) = 2 * GST_SECOND / 25;
  memset (GST_BUFFER_DATA (third), 0, 4);
  caps_newsize = gst_caps_from_string (VIDEO_CAPS_NEWSIZE_STRING);
  gst_buffer_set_caps (third, caps_newsize);

  fail_unless (gst_pad_push (mysrcpad, third) == GST_FLOW_OK);
  /* new caps flushed the internal state, no new output yet */
  fail_unless_equals_int (g_list_length (buffers), 1);
  outbuf = g_list_last (buffers)->data;
  /* first buffer should be output here */
  fail_unless (gst_caps_is_equal (GST_BUFFER_CAPS (outbuf), caps));
  fail_unless (GST_BUFFER_TIMESTAMP (outbuf) == 0);

  /* fourth buffer with original size */
  fourth = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (fourth) = 3 * GST_SECOND / 25;
  memset (GST_BUFFER_DATA (fourth), 0, 4);
  gst_buffer_set_caps (fourth, caps);

  fail_unless (gst_pad_push (mysrcpad, fourth) == GST_FLOW_OK);
  fail_unless_equals_int (g_list_length (buffers), 1);

  /* fifth buffer with original size */
  fifth = gst_buffer_new_and_alloc (4);
  GST_BUFFER_TIMESTAMP (fifth) = 4 * GST_SECOND / 25;
  memset (GST_BUFFER_DATA (fifth), 0, 4);
  gst_buffer_set_caps (fifth, caps);

  fail_unless (gst_pad_push (mysrcpad, fifth) == GST_FLOW_OK);
  /* all four missing buffers here, dups of fourth buffer */
  fail_unless_equals_int (g_list_length (buffers), 4);
  outbuf = g_list_last (buffers)->data;
  /* third buffer should be output here */
  fail_unless (GST_BUFFER_TIMESTAMP (outbuf) == 3 * GST_SECOND / 25);
  fail_unless (gst_caps_is_equal (GST_BUFFER_CAPS (outbuf), caps));

  gst_caps_unref (caps);
  gst_caps_unref (caps_newsize);
  cleanup_videorate (videorate);
}

GST_END_TEST;

GST_START_TEST (test_non_ok_flow)
{
  GstElement *videorate;
  GstClockTime ts;
  GstBuffer *buf;
  GstCaps *caps;

  videorate = setup_videorate ();
  fail_unless (gst_element_set_state (videorate,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  buf = gst_buffer_new_and_alloc (4);
  memset (GST_BUFFER_DATA (buf), 0, 4);
  caps = gst_caps_from_string (VIDEO_CAPS_STRING);
  gst_buffer_set_caps (buf, caps);
  gst_caps_unref (caps);
  ASSERT_BUFFER_REFCOUNT (buf, "inbuffer", 1);

  /* push a few 'normal' buffers */
  for (ts = 0; ts < 100 * GST_SECOND; ts += GST_SECOND / 33) {
    GstBuffer *inbuf;

    inbuf = gst_buffer_copy (buf);
    GST_BUFFER_TIMESTAMP (inbuf) = ts;

    fail_unless_equals_int (gst_pad_push (mysrcpad, inbuf), GST_FLOW_OK);
  }

  /* we should have buffers according to the output framerate of 25/1 */
  fail_unless_equals_int (g_list_length (buffers), 100 * 25);

  /* now deactivate pad so we get a WRONG_STATE flow return */
  gst_pad_set_active (mysinkpad, FALSE);

  /* push buffer on deactivated pad */
  fail_unless (gst_buffer_is_metadata_writable (buf));
  GST_BUFFER_TIMESTAMP (buf) = ts;

  /* pushing gives away our reference */
  fail_unless_equals_int (gst_pad_push (mysrcpad, buf), GST_FLOW_WRONG_STATE);

  /* cleanup */
  cleanup_videorate (videorate);
}

GST_END_TEST;

static Suite *
videorate_suite (void)
{
  Suite *s = suite_create ("videorate");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_one);
  tcase_add_test (tc_chain, test_more);
  tcase_add_test (tc_chain, test_wrong_order_from_zero);
  tcase_add_test (tc_chain, test_wrong_order);
  tcase_add_test (tc_chain, test_no_framerate);
  tcase_add_test (tc_chain, test_changing_size);
  tcase_add_test (tc_chain, test_non_ok_flow);

  return s;
}

GST_CHECK_MAIN (videorate)
