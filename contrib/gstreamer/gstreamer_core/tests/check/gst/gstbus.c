/* GStreamer message bus unit tests
 * Copyright (C) 2005 Andy Wingo <wingo@pobox.com>
 * Copyright (C) 2007 Tim-Philipp Müller <tim centricular net>
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

static GstBus *test_bus = NULL;
static GMainLoop *main_loop;

#define NUM_MESSAGES 1000
#define NUM_THREADS 10

static gpointer
pound_bus_with_messages (gpointer data)
{
  gint thread_id = GPOINTER_TO_INT (data);
  gint i;

  for (i = 0; i < NUM_MESSAGES; i++) {
    GstMessage *m;
    GstStructure *s;

    s = gst_structure_new ("test_message",
        "thread_id", G_TYPE_INT, thread_id, "msg_id", G_TYPE_INT, i, NULL);
    m = gst_message_new_application (NULL, s);
    gst_bus_post (test_bus, m);
  }
  return NULL;
}

static void
pull_messages (void)
{
  GstMessage *m;
  const GstStructure *s;
  guint message_ids[NUM_THREADS];
  gint i;

  for (i = 0; i < NUM_THREADS; i++)
    message_ids[i] = 0;

  while (1) {
    gint _t, _i;

    m = gst_bus_pop (test_bus);
    if (!m)
      break;
    g_return_if_fail (GST_MESSAGE_TYPE (m) == GST_MESSAGE_APPLICATION);

    s = gst_message_get_structure (m);
    if (!gst_structure_get_int (s, "thread_id", &_t))
      g_critical ("Invalid message");
    if (!gst_structure_get_int (s, "msg_id", &_i))
      g_critical ("Invalid message");

    g_return_if_fail (_t < NUM_THREADS);
    g_return_if_fail (_i == message_ids[_t]++);

    gst_message_unref (m);
  }

  for (i = 0; i < NUM_THREADS; i++)
    g_return_if_fail (message_ids[i] == NUM_MESSAGES);
}

GST_START_TEST (test_hammer_bus)
{
  GThread *threads[NUM_THREADS];
  gint i;

  test_bus = gst_bus_new ();

  for (i = 0; i < NUM_THREADS; i++)
    threads[i] = g_thread_create (pound_bus_with_messages, GINT_TO_POINTER (i),
        TRUE, NULL);

  for (i = 0; i < NUM_THREADS; i++)
    g_thread_join (threads[i]);

  pull_messages ();

  gst_object_unref ((GstObject *) test_bus);
}

GST_END_TEST;

static gboolean
message_func_eos (GstBus * bus, GstMessage * message, gpointer data)
{
  const GstStructure *s;
  gint i;

  g_return_val_if_fail (GST_MESSAGE_TYPE (message) == GST_MESSAGE_EOS, FALSE);

  GST_DEBUG ("got EOS message");

  s = gst_message_get_structure (message);
  if (!gst_structure_get_int (s, "msg_id", &i))
    g_critical ("Invalid message");

  return i != 9;
}

static gboolean
message_func_app (GstBus * bus, GstMessage * message, gpointer data)
{
  const GstStructure *s;
  gint i;

  g_return_val_if_fail (GST_MESSAGE_TYPE (message) == GST_MESSAGE_APPLICATION,
      FALSE);

  GST_DEBUG ("got APP message");

  s = gst_message_get_structure (message);
  if (!gst_structure_get_int (s, "msg_id", &i))
    g_critical ("Invalid message");

  return i != 9;
}

static gboolean
send_messages (gpointer data)
{
  GstMessage *m;
  GstStructure *s;
  gint i;

  for (i = 0; i < 10; i++) {
    s = gst_structure_new ("test_message", "msg_id", G_TYPE_INT, i, NULL);
    m = gst_message_new_application (NULL, s);
    gst_bus_post (test_bus, m);
    s = gst_structure_new ("test_message", "msg_id", G_TYPE_INT, i, NULL);
    m = gst_message_new_custom (GST_MESSAGE_EOS, NULL, s);
    gst_bus_post (test_bus, m);
  }

  return FALSE;
}

/* test if adding a signal watch for different message types calls the
 * respective callbacks. */
GST_START_TEST (test_watch)
{
  guint id;

  test_bus = gst_bus_new ();

  main_loop = g_main_loop_new (NULL, FALSE);

  id = gst_bus_add_watch (test_bus, gst_bus_async_signal_func, NULL);
  g_signal_connect (test_bus, "message::eos", (GCallback) message_func_eos,
      NULL);
  g_signal_connect (test_bus, "message::application",
      (GCallback) message_func_app, NULL);

  g_idle_add ((GSourceFunc) send_messages, NULL);
  while (g_main_context_pending (NULL))
    g_main_context_iteration (NULL, FALSE);

  g_source_remove (id);
  g_main_loop_unref (main_loop);

  gst_object_unref ((GstObject *) test_bus);
}

GST_END_TEST;

static gint messages_seen;

static void
message_func (GstBus * bus, GstMessage * message, gpointer data)
{
  g_return_if_fail (GST_MESSAGE_TYPE (message) == GST_MESSAGE_APPLICATION);

  messages_seen++;
}

static void
send_5app_1el_1err_2app_messages (guint interval_usecs)
{
  GstMessage *m;
  GstStructure *s;
  gint i;

  for (i = 0; i < 5; i++) {
    s = gst_structure_new ("test_message", "msg_id", G_TYPE_INT, i, NULL);
    m = gst_message_new_application (NULL, s);
    GST_LOG ("posting application message");
    gst_bus_post (test_bus, m);
    g_usleep (interval_usecs);
  }
  for (i = 0; i < 1; i++) {
    s = gst_structure_new ("test_message", "msg_id", G_TYPE_INT, i, NULL);
    m = gst_message_new_element (NULL, s);
    GST_LOG ("posting element message");
    gst_bus_post (test_bus, m);
    g_usleep (interval_usecs);
  }
  for (i = 0; i < 1; i++) {
    m = gst_message_new_error (NULL, NULL, "debug string");
    GST_LOG ("posting error message");
    gst_bus_post (test_bus, m);
    g_usleep (interval_usecs);
  }
  for (i = 0; i < 2; i++) {
    s = gst_structure_new ("test_message", "msg_id", G_TYPE_INT, i, NULL);
    m = gst_message_new_application (NULL, s);
    GST_LOG ("posting application message");
    gst_bus_post (test_bus, m);
    g_usleep (interval_usecs);
  }
}

static void
send_10_app_messages (void)
{
  GstMessage *m;
  GstStructure *s;
  gint i;

  for (i = 0; i < 10; i++) {
    s = gst_structure_new ("test_message", "msg_id", G_TYPE_INT, i, NULL);
    m = gst_message_new_application (NULL, s);
    gst_bus_post (test_bus, m);
  }
}

/* test that you get the same messages from a poll as from signal watches. */
GST_START_TEST (test_watch_with_poll)
{
  guint i;

  test_bus = gst_bus_new ();
  messages_seen = 0;

  gst_bus_add_signal_watch (test_bus);
  g_signal_connect (test_bus, "message", (GCallback) message_func, NULL);

  send_10_app_messages ();

  for (i = 0; i < 10; i++)
    gst_message_unref (gst_bus_poll (test_bus, GST_MESSAGE_APPLICATION,
            GST_CLOCK_TIME_NONE));

  fail_if (gst_bus_have_pending (test_bus), "unexpected messages on bus");
  fail_unless (messages_seen == 10, "signal handler didn't get 10 messages");

  gst_bus_remove_signal_watch (test_bus);

  gst_object_unref (test_bus);
}

GST_END_TEST;

/* test that you get the messages with pop. */
GST_START_TEST (test_timed_pop)
{
  guint i;

  test_bus = gst_bus_new ();

  send_10_app_messages ();

  for (i = 0; i < 10; i++)
    gst_message_unref (gst_bus_timed_pop (test_bus, GST_CLOCK_TIME_NONE));

  fail_if (gst_bus_have_pending (test_bus), "unexpected messages on bus");

  gst_object_unref (test_bus);
}

GST_END_TEST;

/* test that you get the messages with pop_filtered */
GST_START_TEST (test_timed_pop_filtered)
{
  GstMessage *msg;
  guint i;

  test_bus = gst_bus_new ();

  send_10_app_messages ();
  for (i = 0; i < 10; i++) {
    msg = gst_bus_timed_pop_filtered (test_bus, GST_CLOCK_TIME_NONE,
        GST_MESSAGE_ANY);
    fail_unless (msg != NULL);
    gst_message_unref (msg);
  }

  /* should flush all messages on the bus with types not matching */
  send_10_app_messages ();
  msg = gst_bus_timed_pop_filtered (test_bus, 0,
      GST_MESSAGE_ANY ^ GST_MESSAGE_APPLICATION);
  fail_unless (msg == NULL);
  msg = gst_bus_timed_pop_filtered (test_bus, GST_SECOND / 2,
      GST_MESSAGE_ANY ^ GST_MESSAGE_APPLICATION);
  fail_unless (msg == NULL);
  /* there should be nothing on the bus now */
  fail_if (gst_bus_have_pending (test_bus), "unexpected messages on bus");
  msg = gst_bus_timed_pop_filtered (test_bus, 0, GST_MESSAGE_ANY);
  fail_unless (msg == NULL);

  send_5app_1el_1err_2app_messages (0);
  msg = gst_bus_timed_pop_filtered (test_bus, 0,
      GST_MESSAGE_ANY ^ GST_MESSAGE_APPLICATION);
  fail_unless (msg != NULL);
  fail_unless_equals_int (GST_MESSAGE_TYPE (msg), GST_MESSAGE_ELEMENT);
  gst_message_unref (msg);
  fail_unless (gst_bus_have_pending (test_bus), "expected messages on bus");
  msg = gst_bus_timed_pop_filtered (test_bus, 0, GST_MESSAGE_APPLICATION);
  fail_unless (msg != NULL);
  fail_unless_equals_int (GST_MESSAGE_TYPE (msg), GST_MESSAGE_APPLICATION);
  gst_message_unref (msg);
  msg = gst_bus_timed_pop_filtered (test_bus, 0, GST_MESSAGE_ERROR);
  fail_unless (msg == NULL);

  gst_object_unref (test_bus);
}

GST_END_TEST;

static gpointer
post_delayed_thread (gpointer data)
{
  THREAD_START ();
  send_5app_1el_1err_2app_messages (1 * G_USEC_PER_SEC);
  return NULL;
}

/* test that you get the messages with pop_filtered if there's a timeout*/
GST_START_TEST (test_timed_pop_filtered_with_timeout)
{
  GstMessage *msg;

  MAIN_INIT ();

  test_bus = gst_bus_new ();

  MAIN_START_THREAD_FUNCTIONS (1, post_delayed_thread, NULL);

  MAIN_SYNCHRONIZE ();

  msg = gst_bus_timed_pop_filtered (test_bus, 2 * GST_SECOND,
      GST_MESSAGE_ERROR);
  fail_unless (msg == NULL, "Got unexpected %s message",
      (msg) ? GST_MESSAGE_TYPE_NAME (msg) : "");
  msg = gst_bus_timed_pop_filtered (test_bus, (3 + 1 + 1 + 1) * GST_SECOND,
      GST_MESSAGE_ERROR | GST_MESSAGE_ELEMENT);
  fail_unless (msg != NULL, "expected element message, but got nothing");
  fail_unless_equals_int (GST_MESSAGE_TYPE (msg), GST_MESSAGE_ELEMENT);
  gst_message_unref (msg);
  msg = gst_bus_timed_pop_filtered (test_bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_APPLICATION);
  fail_unless (msg != NULL, "expected application message, but got nothing");
  fail_unless_equals_int (GST_MESSAGE_TYPE (msg), GST_MESSAGE_APPLICATION);
  gst_message_unref (msg);
  msg = gst_bus_timed_pop_filtered (test_bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_APPLICATION);
  fail_unless (msg != NULL, "expected application message, but got nothing");
  fail_unless_equals_int (GST_MESSAGE_TYPE (msg), GST_MESSAGE_APPLICATION);
  gst_message_unref (msg);
  msg = gst_bus_timed_pop_filtered (test_bus, GST_SECOND / 4,
      GST_MESSAGE_TAG | GST_MESSAGE_ERROR);
  fail_unless (msg == NULL, "Got unexpected %s message",
      (msg) ? GST_MESSAGE_TYPE_NAME (msg) : "");

  MAIN_STOP_THREADS ();

  gst_object_unref (test_bus);
}

GST_END_TEST;

/* test that you get the messages with pop from another thread. */
static gpointer
pop_thread (gpointer data)
{
  GstBus *bus = GST_BUS_CAST (data);
  guint i;

  for (i = 0; i < 10; i++)
    gst_message_unref (gst_bus_timed_pop (bus, GST_CLOCK_TIME_NONE));

  return NULL;
}

GST_START_TEST (test_timed_pop_thread)
{
  GThread *thread;
  GError *error = NULL;

  test_bus = gst_bus_new ();

  thread = g_thread_create (pop_thread, test_bus, TRUE, &error);
  fail_if (error != NULL);

  send_10_app_messages ();

  g_thread_join (thread);

  fail_if (gst_bus_have_pending (test_bus), "unexpected messages on bus");

  /* try to pop a message without timeout. */
  fail_if (gst_bus_timed_pop (test_bus, 0) != NULL);

  /* with a small timeout */
  fail_if (gst_bus_timed_pop (test_bus, 1000) != NULL);

  gst_object_unref (test_bus);
}

GST_END_TEST;

static Suite *
gst_bus_suite (void)
{
  Suite *s = suite_create ("GstBus");
  TCase *tc_chain = tcase_create ("stresstest");

  tcase_set_timeout (tc_chain, 20);

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_hammer_bus);
  tcase_add_test (tc_chain, test_watch);
  tcase_add_test (tc_chain, test_watch_with_poll);
  tcase_add_test (tc_chain, test_timed_pop);
  tcase_add_test (tc_chain, test_timed_pop_thread);
  tcase_add_test (tc_chain, test_timed_pop_filtered);
  tcase_add_test (tc_chain, test_timed_pop_filtered_with_timeout);
  return s;
}

GST_CHECK_MAIN (gst_bus);
