/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:
*
*/

/* GStreamer
 * Copyright (C) 2005 Wim Taymans <wim@fluendo.com>
 *
 * gstsystemclock.c: Unit test for GstSystemClock
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


#include <gst/gst_global.h>
#include <gst/check/gstcheck.h>

#define LOG_FILE "c:\\logs\\gstsystemclock_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

//char* xmlfile = "gstsystemclock";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}


/* see if the defines make sense */
void test_range()
{
  GstClockTime time, time2;
  
  xmlfile = "test_range";
  std_log(LOG_FILENAME_LINE, "Test Started test_range");

  time = GST_SECOND;
  fail_unless (time == G_GUINT64_CONSTANT (1000000000));
  

  time2 = time / 1000;
  fail_unless (time2 == 1000000);
  
  fail_unless (time2 == GST_MSECOND);
  
  fail_unless (time2 == GST_TIME_AS_USECONDS (time));
  

  time2 = time / 1000000;
  fail_unless (time2 == 1000);
  
  fail_unless (time2 == GST_USECOND);
  
  fail_unless (time2 == GST_TIME_AS_MSECONDS (time));
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


#define TIME_UNIT (GST_SECOND / 5)
static void
gst_clock_debug (GstClock * clock)
{
  GstClockTime time;

  time = gst_clock_get_time (clock);
  GST_DEBUG ("Clock info: time %" GST_TIME_FORMAT, GST_TIME_ARGS (time));
}

static gboolean
ok_callback (GstClock * clock, GstClockTime time,
    GstClockID id, gpointer user_data)
{
  GST_LOG ("unlocked async id %p", id);
  return FALSE;
}

static gboolean
error_callback (GstClock * clock, GstClockTime time,
    GstClockID id, gpointer user_data)
{
  GST_WARNING ("unlocked unscheduled async id %p, this is wrong", id);
  fail_if (TRUE);

  return FALSE;
}

static gboolean
store_callback (GstClock * clock, GstClockTime time,
    GstClockID id, gpointer user_data)
{
  GList **list = user_data;

  GST_DEBUG ("unlocked async id %p", id);
  *list = g_list_append (*list, id);
  return FALSE;
}

static gboolean
notify_callback (GstClock * clock, GstClockTime time,
    GstClockID id, gpointer user_data)
{
  gboolean *ret = (gboolean *) user_data;

  if (ret != NULL)
    *ret = TRUE;

  return FALSE;
}

void test_single_shot()
{
  GstClock *clock;
  GstClockID id, id2;
  GstClockTime base;
  GstClockReturn result;
  
	xmlfile = "test_single_shot";
  std_log(LOG_FILENAME_LINE, "Test Started test_single_shot");

  clock = gst_system_clock_obtain ();
  fail_unless (clock != NULL, "Could not create instance of GstSystemClock");
  

  gst_clock_debug (clock);
  base = gst_clock_get_time (clock);

  id = gst_clock_new_single_shot_id (clock, base + TIME_UNIT);
  fail_unless (id != NULL, "Could not create single shot id");
  

  GST_DEBUG ("waiting one time unit");
  result = gst_clock_id_wait (id, NULL);
  gst_clock_debug (clock);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK (result=%d)",
      result);
  
  fail_unless (gst_clock_get_time (clock) > (base + TIME_UNIT),
      "target time has not been reached");
      

  GST_DEBUG ("waiting in the past");
  result = gst_clock_id_wait (id, NULL);
  gst_clock_debug (clock);
  fail_unless (result == GST_CLOCK_EARLY,
      "Waiting did not return EARLY(result=%d)", result);
      
  gst_clock_id_unref (id);

  id = gst_clock_new_single_shot_id (clock, base + 2 * TIME_UNIT);
  GST_DEBUG ("waiting one second async id %p", id);
  result = gst_clock_id_wait_async (id, ok_callback, NULL);
  gst_clock_id_unref (id);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  g_usleep (TIME_UNIT / (2 * 1000));

  id = gst_clock_new_single_shot_id (clock, base + 5 * TIME_UNIT);
  GST_DEBUG ("waiting one second async, with cancel on id %p", id);
  result = gst_clock_id_wait_async (id, error_callback, NULL);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  g_usleep (TIME_UNIT / (2 * 1000));
  GST_DEBUG ("cancel id %p after half a time unit", id);
  gst_clock_id_unschedule (id);
  gst_clock_id_unref (id);
  GST_DEBUG ("canceled id %p", id);

  GST_DEBUG ("waiting multiple one second async, with cancel");
  id = gst_clock_new_single_shot_id (clock, base + 5 * TIME_UNIT);
  id2 = gst_clock_new_single_shot_id (clock, base + 6 * TIME_UNIT);
  GST_DEBUG ("waiting id %p", id);
  result = gst_clock_id_wait_async (id, ok_callback, NULL);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  gst_clock_id_unref (id);
  GST_DEBUG ("waiting id %p", id2);
  result = gst_clock_id_wait_async (id2, error_callback, NULL);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  g_usleep (TIME_UNIT / (2 * 1000));
  GST_DEBUG ("cancel id %p after half a time unit", id2);
  gst_clock_id_unschedule (id2);
  GST_DEBUG ("canceled id %p", id2);
  gst_clock_id_unref (id2);
  g_usleep (TIME_UNIT / (2 * 1000));
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


void test_periodic_shot()
{
  GstClock *clock;
  GstClockID id, id2;
  GstClockTime base;
  GstClockReturn result;

	xmlfile = "test_periodic_shot";
  std_log(LOG_FILENAME_LINE, "Test Started test_periodic_shot");
  
  clock = gst_system_clock_obtain ();
  fail_unless (clock != NULL, "Could not create instance of GstSystemClock");
  

  gst_clock_debug (clock);
  base = gst_clock_get_time (clock);

  // signal every half a time unit 
  id = gst_clock_new_periodic_id (clock, base + TIME_UNIT, TIME_UNIT / 2);
  fail_unless (id != NULL, "Could not create periodic id");
  

  GST_DEBUG ("waiting one time unit");
  result = gst_clock_id_wait (id, NULL);
  gst_clock_debug (clock);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  

  GST_DEBUG ("waiting for the next");
  result = gst_clock_id_wait (id, NULL);
  gst_clock_debug (clock);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  

  GST_DEBUG ("waiting for the next async %p", id);
  result = gst_clock_id_wait_async (id, ok_callback, NULL);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  g_usleep (TIME_UNIT / (2 * 1000));

  GST_DEBUG ("waiting some more for the next async %p", id);
  result = gst_clock_id_wait_async (id, ok_callback, NULL);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  g_usleep (TIME_UNIT / (2 * 1000));

  id2 = gst_clock_new_periodic_id (clock, base + TIME_UNIT, TIME_UNIT / 2);
  fail_unless (id2 != NULL, "Could not create second periodic id");
  

  GST_DEBUG ("waiting some more for another async %p", id2);
  result = gst_clock_id_wait_async (id2, ok_callback, NULL);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  g_usleep (TIME_UNIT / (2 * 1000));

  GST_DEBUG ("unschedule %p", id);
  gst_clock_id_unschedule (id);

  // entry cannot be used again 
  result = gst_clock_id_wait_async (id, error_callback, NULL);
  fail_unless (result == GST_CLOCK_UNSCHEDULED,
      "Waiting did not return UNSCHEDULED");
      
  result = gst_clock_id_wait (id, NULL);
  fail_unless (result == GST_CLOCK_UNSCHEDULED,
      "Waiting did not return UNSCHEDULED");
      
  g_usleep (TIME_UNIT / (2 * 1000));

  // clean up 
  gst_clock_id_unref (id);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


void test_async_order()
{
  GstClock *clock;
  GstClockID id1, id2;
  GList *cb_list = NULL, *next;
  GstClockTime base;
  GstClockReturn result;

	xmlfile = "test_async_order";
  std_log(LOG_FILENAME_LINE, "Test Started test_async_order");
  
  
  clock = gst_system_clock_obtain ();
  fail_unless (clock != NULL, "Could not create instance of GstSystemClock");
  

  gst_clock_debug (clock);
  base = gst_clock_get_time (clock);

  id1 = gst_clock_new_single_shot_id (clock, base + 2 * TIME_UNIT);
  id2 = gst_clock_new_single_shot_id (clock, base + 1 * TIME_UNIT);
  result = gst_clock_id_wait_async (id1, store_callback, &cb_list);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  g_usleep (TIME_UNIT / (2 * 1000));
  result = gst_clock_id_wait_async (id2, store_callback, &cb_list);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  g_usleep (TIME_UNIT / 1000);
  // at this point at least one of the timers should have timed out 
  fail_unless (cb_list != NULL, "expected notification");
  
  fail_unless (cb_list->data == id2,
      "Expected notification for id2 to come first");
      
  g_usleep (TIME_UNIT / 1000);
  // now both should have timed out 
  next = g_list_next (cb_list);
  fail_unless (next != NULL, "expected second notification");
  
  fail_unless (next->data == id1, "Missing notification for id1");
  
  gst_clock_id_unref (id1);
  gst_clock_id_unref (id2);
  g_list_free (cb_list);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


void test_periodic_multi()
{
  GstClock *clock;
  GstClockID clock_id;
  GstClockTime base;
  GstClockReturn result;
  gboolean got_callback = FALSE;
  
  xmlfile = "test_periodic_multi";
  std_log(LOG_FILENAME_LINE, "Test Started test_periodic_multi");


  clock = gst_system_clock_obtain ();
  fail_unless (clock != NULL, "Could not create instance of GstSystemClock");
  

  gst_clock_debug (clock);
  base = gst_clock_get_time (clock);

  clock_id = gst_clock_new_periodic_id (clock, base + TIME_UNIT, TIME_UNIT);
  gst_clock_id_wait (clock_id, NULL);
  fail_unless (gst_clock_get_time (clock) >= base + TIME_UNIT);
  
  fail_unless (gst_clock_get_time (clock) < base + 2 * TIME_UNIT);
  

  // now perform a concurrent wait and wait_async 

  result = gst_clock_id_wait_async (clock_id, notify_callback, &got_callback);
  fail_unless (result == GST_CLOCK_OK, "Async waiting did not return OK");
  
  fail_unless (got_callback == FALSE);
  
  result = gst_clock_id_wait (clock_id, NULL);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  fail_unless (gst_clock_get_time (clock) >= base + 2 * TIME_UNIT);
  
  // give the async thread some time to call our callback: 
  g_usleep (TIME_UNIT / (10 * 1000));
  fail_unless (got_callback == TRUE, "got no async callback (1)");
  
  fail_unless (gst_clock_get_time (clock) < base + 3 * TIME_UNIT);
  
  got_callback = FALSE;

  result = gst_clock_id_wait (clock_id, NULL);
  fail_unless (result == GST_CLOCK_OK, "Waiting did not return OK");
  
  fail_unless (gst_clock_get_time (clock) >= base + 3 * TIME_UNIT);
  
  // give the async thread some time to call our callback: 
  g_usleep (TIME_UNIT / (10 * 1000));
  fail_unless (got_callback == TRUE, "got no async callback (2)");
  
  fail_unless (gst_clock_get_time (clock) < base + 4 * TIME_UNIT);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* test if a blocking wait, unblocked by an async entry continues to be
 * scheduled */
typedef struct
{
  GstClock *clock;
  GstClockID id;
  GstClockTimeDiff jitter;
  GstClockReturn ret;
} MixedInfo;

static gpointer
mixed_thread (MixedInfo * info)
{
  info->ret = gst_clock_id_wait (info->id, &info->jitter);
  return NULL;
}

static gboolean
mixed_async_cb (GstClock * clock, GstClockTime time,
    GstClockID id, gpointer user_data)
{
  return TRUE;
}

void test_mixed()
{
  GThread *thread;
  GError *error = NULL;
  MixedInfo info;
  GstClockTime base;
  GstClockID id;
  
  xmlfile = "test_mixed";
  std_log(LOG_FILENAME_LINE, "Test Started test_mixed");


  info.clock = gst_system_clock_obtain ();
  fail_unless (info.clock != NULL,
      "Could not create instance of GstSystemClock");
      

  /* get current time of the clock as base time */
  base = gst_clock_get_time (info.clock);

  /* create entry to wait for 1 second */
  info.id = gst_clock_new_single_shot_id (info.clock, base + GST_SECOND);

  /* make and start an entry that is scheduled every 10ms */
  id = gst_clock_new_periodic_id (info.clock, base, 10 * GST_MSECOND);

  /* start waiting for the entry */
  thread = g_thread_create ((GThreadFunc) mixed_thread, &info, TRUE, &error);
  fail_unless (error == NULL, "error creating thread");
  
  fail_unless (thread != NULL, "Could not create thread");
  

  /* wait half a second so we are sure to be in the thread */
  g_usleep (G_USEC_PER_SEC / 2);

  /* start scheduling the entry */
  gst_clock_id_wait_async (id, mixed_async_cb, NULL);

  /* wait for thread to finish */
  g_thread_join (thread);
  /* entry must have timed out correctly */
  fail_unless (info.ret == GST_CLOCK_OK, "clock return was %d", info.ret);
  

  gst_clock_id_unschedule (id);
  gst_clock_id_unref (id);
  gst_clock_id_unref (info.id);
  gst_object_unref (info.clock);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}

void test_diff()
{


  GstClockTime time1[] = { 0, (GstClockTime) - 1, 0, 1, 2 * GST_SECOND,(GstClockTime) - GST_SECOND, (GstClockTime) - GST_SECOND};
  GstClockTime time2[] = { 0, 1, 1, 0, 1 * GST_SECOND, (GstClockTime) - GST_SECOND, GST_SECOND };
  GstClockTimeDiff d[] = { 0, 2, 1, -1, -GST_SECOND, 0, 2 * GST_SECOND };
  guint i;
  
    xmlfile = "test_diff";
  std_log(LOG_FILENAME_LINE, "Test Started test_diff");

  for (i = 0; i < G_N_ELEMENTS (d); i++) {
    fail_if (d[i] != GST_CLOCK_DIFF (time1[i], time2[i]));
  }
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}

void test_signedness()
{

  
  GstClockTime time[] = { 0, 1, G_MAXUINT64 / GST_SECOND };
  GstClockTimeDiff diff[] = { 0, 1, -1, G_MAXINT64 / GST_SECOND, G_MININT64 / GST_SECOND };
  guint i;
  
  	xmlfile = "test_signedness";
  std_log(LOG_FILENAME_LINE, "Test Started test_signedness");

  for (i = 0; i < G_N_ELEMENTS (time); i++) {
    fail_if (time[i] != (time[i] * GST_SECOND / GST_SECOND));
  }
  for (i = 0; i < G_N_ELEMENTS (diff); i++) {
    fail_if (diff[i] != (diff[i] * GST_SECOND / GST_SECOND));
  }
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
} 



/*void main(int argc,char** argv)
{
        gst_init(&argc,&argv);
        test_range();
        test_mixed();
        test_diff();
        test_signedness();
        
//        test_single_shot();
//        test_periodic_shot();
//        test_periodic_multi();
//        test_async_order();
        
}*/


void (*fn[8]) (void) = {
        test_range,
        test_diff,
        test_signedness,
        test_single_shot,
        test_periodic_shot,
        test_periodic_multi,
        test_async_order,
        test_mixed
};

char *args[] = {
        "test_range",
        "test_diff",
        "test_signedness",
        "test_single_shot",
        "test_periodic_shot",
        "test_periodic_multi",
        "test_async_order",
        "test_mixed",
};

GST_CHECK_MAIN (gst_systemclock);





