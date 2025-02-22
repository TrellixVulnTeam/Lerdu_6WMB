/* GStreamer unit tests for textoverlay
 *
 * Copyright (C) 2006 Tim-Philipp Müller <tim centricular net>
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

#define I420_Y_ROWSTRIDE(width) (GST_ROUND_UP_4(width))
#define I420_U_ROWSTRIDE(width) (GST_ROUND_UP_8(width)/2)
#define I420_V_ROWSTRIDE(width) ((GST_ROUND_UP_8(I420_Y_ROWSTRIDE(width)))/2)

#define I420_Y_OFFSET(w,h) (0)
#define I420_U_OFFSET(w,h) (I420_Y_OFFSET(w,h)+(I420_Y_ROWSTRIDE(w)*GST_ROUND_UP_2(h)))
#define I420_V_OFFSET(w,h) (I420_U_OFFSET(w,h)+(I420_U_ROWSTRIDE(w)*GST_ROUND_UP_2(h)/2))

#define I420_SIZE(w,h)     (I420_V_OFFSET(w,h)+(I420_V_ROWSTRIDE(w)*GST_ROUND_UP_2(h)/2))

#define WIDTH 240
#define HEIGHT 120

/* For ease of programming we use globals to keep refs for our floating
 * src and sink pads we create; otherwise we always have to do get_pad,
 * get_peer, and then remove references in every test function */
static GstPad *myvideosrcpad, *mytextsrcpad, *mysinkpad;

#define VIDEO_CAPS_STRING               \
    "video/x-raw-yuv, "                 \
    "format = (fourcc) I420, "          \
    "framerate = (fraction) 1/1, "      \
    "width = (int) 240, "               \
    "height = (int) 120"

#define VIDEO_CAPS_TEMPLATE_STRING      \
    "video/x-raw-yuv, "                 \
    "format = (fourcc) I420"

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (VIDEO_CAPS_TEMPLATE_STRING)
    );
static GstStaticPadTemplate text_srctemplate = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("text/plain")
    );

static GstStaticPadTemplate video_srctemplate = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (VIDEO_CAPS_TEMPLATE_STRING)
    );

/* much like gst_check_setup_src_pad(), but with possibility to give a hint
 * which sink template of the element to use, if there are multiple ones */
static GstPad *
notgst_check_setup_src_pad2 (GstElement * element,
    GstStaticPadTemplate * template, GstCaps * caps,
    const gchar * sink_template_name)
{
  GstPad *srcpad, *sinkpad;

  if (sink_template_name == NULL)
    sink_template_name = "sink";

  /* sending pad */
  srcpad = gst_pad_new_from_static_template (template, "src");
  GST_DEBUG_OBJECT (element, "setting up sending pad %p", srcpad);
  fail_if (srcpad == NULL, "Could not create a srcpad");
  ASSERT_OBJECT_REFCOUNT (srcpad, "srcpad", 1);

  sinkpad = gst_element_get_pad (element, sink_template_name);
  fail_if (sinkpad == NULL, "Could not get sink pad from %s",
      GST_ELEMENT_NAME (element));
  ASSERT_OBJECT_REFCOUNT (sinkpad, "sinkpad", 2);
  if (caps)
    fail_unless (gst_pad_set_caps (srcpad, caps));
  fail_unless (gst_pad_link (srcpad, sinkpad) == GST_PAD_LINK_OK,
      "Could not link source and %s sink pads", GST_ELEMENT_NAME (element));
  gst_object_unref (sinkpad);   /* because we got it higher up */
  ASSERT_OBJECT_REFCOUNT (sinkpad, "sinkpad", 1);

  return srcpad;
}

static void
notgst_check_teardown_src_pad2 (GstElement * element,
    const gchar * sink_template_name)
{
  GstPad *srcpad, *sinkpad;

  if (sink_template_name == NULL)
    sink_template_name = "sink";

  /* clean up floating src pad */
  sinkpad = gst_element_get_pad (element, sink_template_name);
  ASSERT_OBJECT_REFCOUNT (sinkpad, "sinkpad", 2);
  srcpad = gst_pad_get_peer (sinkpad);

  gst_pad_unlink (srcpad, sinkpad);

  /* caps could have been set, make sure they get unset */
  gst_pad_set_caps (srcpad, NULL);

  /* pad refs held by both creator and this function (through _get) */
  ASSERT_OBJECT_REFCOUNT (sinkpad, "element sinkpad", 2);
  gst_object_unref (sinkpad);
  /* one more ref is held by element itself */

  /* pad refs held by both creator and this function (through _get_peer) */
  ASSERT_OBJECT_REFCOUNT (srcpad, "check srcpad", 2);
  gst_object_unref (srcpad);
  gst_object_unref (srcpad);
}

static GstElement *
setup_textoverlay (gboolean video_only_no_text)
{
  GstElement *textoverlay;

  GST_DEBUG ("setup_textoverlay");
  textoverlay = gst_check_setup_element ("textoverlay");
  mysinkpad = gst_check_setup_sink_pad (textoverlay, &sinktemplate, NULL);
  myvideosrcpad =
      notgst_check_setup_src_pad2 (textoverlay, &video_srctemplate, NULL,
      "video_sink");

  if (!video_only_no_text) {
    mytextsrcpad =
        notgst_check_setup_src_pad2 (textoverlay, &text_srctemplate, NULL,
        "text_sink");
    gst_pad_set_active (mytextsrcpad, TRUE);
  } else {
    mytextsrcpad = NULL;
  }

  gst_pad_set_active (myvideosrcpad, TRUE);
  gst_pad_set_active (mysinkpad, TRUE);

  return textoverlay;
}

static gboolean
buffer_is_all_black (GstBuffer * buf)
{
  GstStructure *s;
  gint x, y, w, h;

  fail_unless (buf != NULL);
  fail_unless (GST_BUFFER_CAPS (buf) != NULL);
  s = gst_caps_get_structure (GST_BUFFER_CAPS (buf), 0);
  fail_unless (s != NULL);
  fail_unless (gst_structure_get_int (s, "width", &w));
  fail_unless (gst_structure_get_int (s, "height", &h));

  for (y = 0; y < h; ++y) {
    guint8 *data = GST_BUFFER_DATA (buf) + (y * GST_ROUND_UP_4 (w));

    for (x = 0; x < w; ++x) {
      if (data[x] != 0x00) {
        GST_LOG ("non-black pixel at (x,y) %d,%d", x, y);
        return FALSE;
      }
    }
  }

  return TRUE;
}

static GstBuffer *
create_black_buffer (const gchar * caps_string)
{
  GstStructure *s;
  GstBuffer *buffer;
  GstCaps *caps;
  gint w, h, size;

  fail_unless (caps_string != NULL);

  caps = gst_caps_from_string (caps_string);
  fail_unless (caps != NULL);
  fail_unless (gst_caps_is_fixed (caps));

  s = gst_caps_get_structure (caps, 0);
  fail_unless (gst_structure_get_int (s, "width", &w));
  fail_unless (gst_structure_get_int (s, "height", &h));

  GST_LOG ("creating buffer (%dx%d)", w, h);
  size = I420_SIZE (w, h);
  buffer = gst_buffer_new_and_alloc (size);
  /* we're only checking the Y plane later, so just zero it all out,
   * even if it's not the blackest black there is */
  memset (GST_BUFFER_DATA (buffer), 0, size);

  gst_buffer_set_caps (buffer, caps);
  gst_caps_unref (caps);

  /* double check to make sure it's been created right */
  fail_unless (buffer_is_all_black (buffer));

  return buffer;
}

static GstBuffer *
create_text_buffer (const gchar * txt, GstClockTime ts, GstClockTime duration)
{
  GstBuffer *buffer;
  GstCaps *caps;
  guint txt_len;

  fail_unless (txt != NULL);

  txt_len = strlen (txt);

  buffer = gst_buffer_new_and_alloc (txt_len);
  memcpy (GST_BUFFER_DATA (buffer), txt, txt_len);

  GST_BUFFER_TIMESTAMP (buffer) = ts;
  GST_BUFFER_DURATION (buffer) = duration;

  caps = gst_caps_new_simple ("text/plain", NULL);
  gst_buffer_set_caps (buffer, caps);
  gst_caps_unref (caps);

  return buffer;
}

static void
cleanup_textoverlay (GstElement * textoverlay)
{
  GST_DEBUG ("cleanup_textoverlay");

  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;

  gst_element_set_state (textoverlay, GST_STATE_NULL);
  gst_element_get_state (textoverlay, NULL, NULL, GST_CLOCK_TIME_NONE);
  gst_pad_set_active (myvideosrcpad, FALSE);
  gst_pad_set_active (mysinkpad, FALSE);
  notgst_check_teardown_src_pad2 (textoverlay, "video_sink");
  if (mytextsrcpad) {
    notgst_check_teardown_src_pad2 (textoverlay, "text_sink");
  }
  gst_check_teardown_sink_pad (textoverlay);
  gst_check_teardown_element (textoverlay);
}

GST_START_TEST (test_video_passthrough)
{
  GstElement *textoverlay;
  GstBuffer *inbuffer;

  textoverlay = setup_textoverlay (TRUE);
  fail_unless (gst_element_set_state (textoverlay,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  inbuffer = create_black_buffer (VIDEO_CAPS_STRING);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  /* ========== (1) video buffer without timestamp => should be dropped ==== */

  /* take additional ref to keep it alive */
  gst_buffer_ref (inbuffer);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 2);

  /* pushing gives away one of the two references we have ... */
  fail_unless (gst_pad_push (myvideosrcpad, inbuffer) == GST_FLOW_OK);

  /* should have been discarded as out-of-segment since it has no timestamp */
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);
  fail_unless_equals_int (g_list_length (buffers), 0);

  /* ========== (2) buffer with 0 timestamp => simple passthrough ========== */

  /* now try again, this time with timestamp (segment defaults to 0 start) */
  GST_BUFFER_TIMESTAMP (inbuffer) = 0;
  GST_BUFFER_DURATION (inbuffer) = GST_CLOCK_TIME_NONE;

  /* take additional ref to keep it alive */
  gst_buffer_ref (inbuffer);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 2);

  /* pushing gives away one of the two references we have ... */
  fail_unless (gst_pad_push (myvideosrcpad, inbuffer) == GST_FLOW_OK);

  /* text pad is not linked, timestamp is in segment, no static text to
   * render, should have gone through right away without modification */
  fail_unless_equals_int (g_list_length (buffers), 1);
  fail_unless (GST_BUFFER_CAST (buffers->data) == inbuffer);
  fail_unless (buffer_is_all_black (inbuffer));
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 2);

  /* and clean up */
  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  /* ========== (3) buffer with 0 timestamp and no duration, with the
   *                segment starting from 1sec => should be discarded */

  gst_pad_push_event (myvideosrcpad,
      gst_event_new_new_segment (FALSE, 1.0, GST_FORMAT_TIME, 1 * GST_SECOND,
          -1, 0));

  GST_BUFFER_TIMESTAMP (inbuffer) = 0;
  GST_BUFFER_DURATION (inbuffer) = GST_CLOCK_TIME_NONE;

  /* take additional ref to keep it alive */
  gst_buffer_ref (inbuffer);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 2);

  /* pushing gives away one of the two references we have ... */
  fail_unless (gst_pad_push (myvideosrcpad, inbuffer) == GST_FLOW_OK);

  /* should have been discarded as out-of-segment */
  fail_unless_equals_int (g_list_length (buffers), 0);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  /* ========== (4) buffer with 0 timestamp and small defined duration, with
   *                segment starting from 1sec => should be discarded */

  gst_pad_push_event (myvideosrcpad,
      gst_event_new_new_segment (FALSE, 1.0, 1 * GST_FORMAT_TIME, GST_SECOND,
          -1, 0));

  GST_BUFFER_DURATION (inbuffer) = GST_SECOND / 10;

  /* take additional ref to keep it alive */
  gst_buffer_ref (inbuffer);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 2);

  /* pushing gives away one of the two references we have ... */
  fail_unless (gst_pad_push (myvideosrcpad, inbuffer) == GST_FLOW_OK);

  /* should have been discareded as out-of-segment since it has no timestamp */
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);
  fail_unless_equals_int (g_list_length (buffers), 0);

  /* ========== (5) buffer partially overlapping into the segment => should
   *                be pushed through, but with adjusted stamp values */

  gst_pad_push_event (myvideosrcpad,
      gst_event_new_new_segment (FALSE, 1.0, 1 * GST_FORMAT_TIME, GST_SECOND,
          -1, 0));

  GST_BUFFER_TIMESTAMP (inbuffer) = GST_SECOND / 4;
  GST_BUFFER_DURATION (inbuffer) = GST_SECOND;

  /* take additional ref to keep it alive */
  gst_buffer_ref (inbuffer);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 2);

  /* pushing gives away one of the two references we have ... */
  fail_unless (gst_pad_push (myvideosrcpad, inbuffer) == GST_FLOW_OK);

  /* should be the parent for a new subbuffer for the stamp fix-up */
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 2);
  fail_unless_equals_int (g_list_length (buffers), 1);
  fail_unless (GST_BUFFER_CAST (buffers->data) != inbuffer);
  fail_unless (GST_BUFFER_TIMESTAMP (GST_BUFFER_CAST (buffers->data)) ==
      GST_SECOND);
  fail_unless (GST_BUFFER_DURATION (GST_BUFFER_CAST (buffers->data)) ==
      (GST_SECOND / 4));
  fail_unless (buffer_is_all_black (GST_BUFFER_CAST (buffers->data)));
  /* and clean up */
  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  /* cleanup */
  cleanup_textoverlay (textoverlay);
  gst_buffer_unref (inbuffer);
}

GST_END_TEST;

GST_START_TEST (test_video_render_static_text)
{
  GstElement *textoverlay;
  GstBuffer *inbuffer;

  textoverlay = setup_textoverlay (TRUE);

  /* set static text to render */
  g_object_set (textoverlay, "text", "XLX", NULL);

  fail_unless (gst_element_set_state (textoverlay,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  inbuffer = create_black_buffer (VIDEO_CAPS_STRING);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  GST_BUFFER_TIMESTAMP (inbuffer) = 0;
  GST_BUFFER_DURATION (inbuffer) = GST_SECOND / 10;

  /* take additional ref to keep it alive */
  gst_buffer_ref (inbuffer);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 2);

  /* pushing gives away one of the two references we have ... */
  fail_unless (gst_pad_push (myvideosrcpad, inbuffer) == GST_FLOW_OK);

  /* should have been dropped in favour of a new writable buffer */
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);
  fail_unless_equals_int (g_list_length (buffers), 1);
  fail_unless (GST_BUFFER_CAST (buffers->data) != inbuffer);

  /* there should be text rendered */
  fail_unless (buffer_is_all_black (GST_BUFFER_CAST (buffers->data)) == FALSE);

  fail_unless (GST_BUFFER_TIMESTAMP (GST_BUFFER_CAST (buffers->data)) == 0);
  fail_unless (GST_BUFFER_DURATION (GST_BUFFER_CAST (buffers->data)) ==
      (GST_SECOND / 10));

  /* and clean up */
  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  /* cleanup */
  cleanup_textoverlay (textoverlay);
  gst_buffer_unref (inbuffer);
}

GST_END_TEST;

static gpointer
test_video_waits_for_text_send_text_newsegment_thread (gpointer data)
{
  g_usleep (1 * G_USEC_PER_SEC);

  /* send an update newsegment; the video buffer should now be pushed through 
   * even though there is no text buffer queued at the moment */
  GST_INFO ("Sending newsegment update on text pad");
  gst_pad_push_event (mytextsrcpad,
      gst_event_new_new_segment (TRUE, 1.0, GST_FORMAT_TIME,
          35 * GST_SECOND, -1, 35 * GST_SECOND));

  return NULL;
}

static gpointer
test_video_waits_for_text_shutdown_element (gpointer data)
{
  g_usleep (1 * G_USEC_PER_SEC);

  GST_INFO ("Trying to shut down textoverlay element ...");
  /* set to NULL state to make sure we can shut it down while it's
   * blocking in the video chain function waiting for a text buffer */
  gst_element_set_state (GST_ELEMENT (data), GST_STATE_NULL);
  GST_INFO ("Done.");

  return NULL;
}

GST_START_TEST (test_video_waits_for_text)
{
  GstElement *textoverlay;
  GstBuffer *inbuffer, *tbuf;
  GThread *thread;

  textoverlay = setup_textoverlay (FALSE);

  fail_unless (gst_element_set_state (textoverlay,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  tbuf = create_text_buffer ("XLX", 1 * GST_SECOND, 5 * GST_SECOND);
  gst_buffer_ref (tbuf);
  ASSERT_BUFFER_REFCOUNT (tbuf, "tbuf", 2);

  GST_LOG ("pushing text buffer");
  fail_unless (gst_pad_push (mytextsrcpad, tbuf) == GST_FLOW_OK);

  /* it should be stuck in textoverlay until it gets a text buffer or a
   * newsegment event that indicates it's not needed any longer */
  fail_unless_equals_int (g_list_length (buffers), 0);

  inbuffer = create_black_buffer (VIDEO_CAPS_STRING);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  GST_BUFFER_TIMESTAMP (inbuffer) = 0;
  GST_BUFFER_DURATION (inbuffer) = GST_SECOND / 2;

  /* take additional ref to keep it alive */
  gst_buffer_ref (inbuffer);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 2);

  /* pushing gives away one of the two references we have ... */
  GST_LOG ("pushing video buffer 1");
  fail_unless (gst_pad_push (myvideosrcpad, inbuffer) == GST_FLOW_OK);

  /* video buffer should have gone through untainted, since the text is later */
  fail_unless_equals_int (g_list_length (buffers), 1);

  /* text should still be stuck in textoverlay */
  ASSERT_BUFFER_REFCOUNT (tbuf, "tbuf", 2);

  /* there should be no text rendered */
  fail_unless (buffer_is_all_black (GST_BUFFER_CAST (buffers->data)));

  /* now, another video buffer */
  inbuffer = gst_buffer_make_metadata_writable (inbuffer);
  GST_BUFFER_TIMESTAMP (inbuffer) = GST_SECOND;
  GST_BUFFER_DURATION (inbuffer) = GST_SECOND / 2;

  /* pushing gives away one of the two references we have ... */
  GST_LOG ("pushing video buffer 2");
  gst_buffer_ref (inbuffer);
  fail_unless (gst_pad_push (myvideosrcpad, inbuffer) == GST_FLOW_OK);

  /* video buffer should have gone right away, with text rendered on it */
  fail_unless_equals_int (g_list_length (buffers), 2);

  /* text should still be stuck in textoverlay */
  ASSERT_BUFFER_REFCOUNT (tbuf, "tbuf", 2);

  /* there should be text rendered */
  fail_unless (buffer_is_all_black (GST_BUFFER_CAST (buffers->next->data)) ==
      FALSE);

  /* a third video buffer */
  inbuffer = gst_buffer_make_metadata_writable (inbuffer);
  GST_BUFFER_TIMESTAMP (inbuffer) = 30 * GST_SECOND;
  GST_BUFFER_DURATION (inbuffer) = GST_SECOND / 2;

  /* video buffer #3: should not go through, it should discard the current
   * text buffer as too old and then wait for the next text buffer (or a
   * newsegment event to arrive); we spawn a background thread to send such
   * a newsegment event after a second or so so we get back control */
  thread =
      g_thread_create (test_video_waits_for_text_send_text_newsegment_thread,
      NULL, FALSE, NULL);
  fail_unless (thread != NULL);

  GST_LOG ("pushing video buffer 3");
  gst_buffer_ref (inbuffer);
  fail_unless (gst_pad_push (myvideosrcpad, inbuffer) == GST_FLOW_OK);

  /* but the text should no longer be stuck in textoverlay */
  ASSERT_BUFFER_REFCOUNT (tbuf, "tbuf", 1);

  /* video buffer should have gone through after newsegment event */
  fail_unless_equals_int (g_list_length (buffers), 3);

  /* ... and there should not be any text rendered on it */
  fail_unless (buffer_is_all_black (GST_BUFFER_CAST (buffers->next->next->
              data)));

  /* a fourth video buffer */
  inbuffer = gst_buffer_make_metadata_writable (inbuffer);
  GST_BUFFER_TIMESTAMP (inbuffer) = 35 * GST_SECOND;
  GST_BUFFER_DURATION (inbuffer) = GST_SECOND;

  /* video buffer #4: should not go through, it should wait for the next
   * text buffer (or a newsegment event) to arrive; we spawn a background
   * thread to shut down the element while it's waiting to make sure that
   * works ok */
  thread = g_thread_create (test_video_waits_for_text_shutdown_element,
      textoverlay, FALSE, NULL);
  fail_unless (thread != NULL);

  GST_LOG ("pushing video buffer 4");
  gst_buffer_ref (inbuffer);
  fail_unless (gst_pad_push (myvideosrcpad, inbuffer) == GST_FLOW_WRONG_STATE);

  /* and clean up */
  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  /* cleanup */
  cleanup_textoverlay (textoverlay);
  gst_buffer_unref (inbuffer);

  /* give up our ref, textoverlay should've cleared its queued buffer by now */
  ASSERT_BUFFER_REFCOUNT (tbuf, "tbuf", 1);
  gst_buffer_unref (tbuf);
}

GST_END_TEST;

static gpointer
test_render_continuity_push_video_buffers_thread (gpointer data)
{
  /* push video buffers at 1fps */
  guint frame_count = 0;

  do {
    GstBuffer *vbuf;

    vbuf = create_black_buffer (VIDEO_CAPS_STRING);
    ASSERT_BUFFER_REFCOUNT (vbuf, "vbuf", 1);

    GST_BUFFER_TIMESTAMP (vbuf) = frame_count * GST_SECOND;
    GST_BUFFER_DURATION (vbuf) = GST_SECOND;

    /* pushing gives away one of the two references we have ... */
    GST_LOG ("pushing video buffer %u @ %" GST_TIME_FORMAT, frame_count,
        GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (vbuf)));
    fail_unless (gst_pad_push (myvideosrcpad, vbuf) == GST_FLOW_OK);

    ++frame_count;
  } while (frame_count < 15);

  return NULL;
}


GST_START_TEST (test_render_continuity)
{
  GThread *thread;
  GstElement *textoverlay;
  GstBuffer *tbuf;

  textoverlay = setup_textoverlay (FALSE);

  fail_unless (gst_element_set_state (textoverlay,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  thread = g_thread_create (test_render_continuity_push_video_buffers_thread,
      NULL, FALSE, NULL);
  fail_unless (thread != NULL);

  tbuf = create_text_buffer ("XLX", 2 * GST_SECOND, GST_SECOND);
  GST_LOG ("pushing text buffer @ %" GST_TIME_FORMAT,
      GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (tbuf)));
  fail_unless (gst_pad_push (mytextsrcpad, tbuf) == GST_FLOW_OK);

  tbuf = create_text_buffer ("XLX", 3 * GST_SECOND, 2 * GST_SECOND);
  GST_LOG ("pushing text buffer @ %" GST_TIME_FORMAT,
      GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (tbuf)));
  fail_unless (gst_pad_push (mytextsrcpad, tbuf) == GST_FLOW_OK);

  tbuf = create_text_buffer ("XLX", 7 * GST_SECOND, GST_SECOND);
  GST_LOG ("pushing text buffer @ %" GST_TIME_FORMAT,
      GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (tbuf)));
  fail_unless (gst_pad_push (mytextsrcpad, tbuf) == GST_FLOW_OK);

  tbuf = create_text_buffer ("XLX", 8 * GST_SECOND, GST_SECOND);
  GST_LOG ("pushing text buffer @ %" GST_TIME_FORMAT,
      GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (tbuf)));
  fail_unless (gst_pad_push (mytextsrcpad, tbuf) == GST_FLOW_OK);

  tbuf = create_text_buffer ("XLX", 9 * GST_SECOND, GST_SECOND);
  GST_LOG ("pushing text buffer @ %" GST_TIME_FORMAT,
      GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (tbuf)));
  fail_unless (gst_pad_push (mytextsrcpad, tbuf) == GST_FLOW_OK);

  tbuf = create_text_buffer ("XLX", 10 * GST_SECOND, 30 * GST_SECOND);
  GST_LOG ("pushing text buffer @ %" GST_TIME_FORMAT,
      GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (tbuf)));
  fail_unless (gst_pad_push (mytextsrcpad, tbuf) == GST_FLOW_OK);

  GST_LOG ("give the other thread some time to push through the remaining"
      "video buffers");
  g_usleep (G_USEC_PER_SEC);
  GST_LOG ("done");

  /* we should have 15 buffers each with one second length now */
  fail_unless_equals_int (g_list_length (buffers), 15);

  /* buffers 0 + 1 should be black */
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers, 0))));
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers, 1))));

  /* buffers 2 - 4 should have text */
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  2))) == FALSE);
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  3))) == FALSE);
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  4))) == FALSE);

  /* buffers 5 + 6 should be black */
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers, 5))));
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers, 6))));

  /* buffers 7 - last should have text */
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  7))) == FALSE);
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  8))) == FALSE);
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  9))) == FALSE);
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  10))) == FALSE);
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  11))) == FALSE);
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  12))) == FALSE);
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  13))) == FALSE);
  fail_unless (buffer_is_all_black (GST_BUFFER (g_list_nth_data (buffers,
                  14))) == FALSE);

  /* and clean up */
  g_list_foreach (buffers, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (buffers);
  buffers = NULL;

  /* cleanup */
  cleanup_textoverlay (textoverlay);
}

GST_END_TEST;

static Suite *
textoverlay_suite (void)
{
  Suite *s = suite_create ("textoverlay");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);

  tcase_add_test (tc_chain, test_video_passthrough);
  tcase_add_test (tc_chain, test_video_render_static_text);
  tcase_add_test (tc_chain, test_render_continuity);
  tcase_add_test (tc_chain, test_video_waits_for_text);

  return s;
}

GST_CHECK_MAIN (textoverlay);
