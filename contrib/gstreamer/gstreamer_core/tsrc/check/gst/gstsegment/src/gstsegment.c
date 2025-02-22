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
 * Copyright (C) 2005 Jan Schmidt <thaytan@mad.scientist.com>
 *
 * gstsegment.c: Unit test for segments
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

#define LOG_FILE "c:\\logs\\gstsegment_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__


#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical;
#endif

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log;
#endif


//char* xmlfile = "gstsegment";

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}

void segment_seek_nosize()
{
  GstSegment segment;
  gboolean res;
  gint64 cstart, cstop;
  gboolean update;
  
    xmlfile = "segment_seek_nosize";
  std_log(LOG_FILENAME_LINE, "Test Started segment_seek_nosize");

  gst_segment_init (&segment, GST_FORMAT_BYTES);

  /* configure segment to start 100 */
  gst_segment_set_seek (&segment, 1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_SET, 100, GST_SEEK_TYPE_NONE, -1, &update);
  fail_unless (segment.start == 100);
  fail_unless (segment.stop == -1);
  fail_unless (update == TRUE);

  /* configure segment to stop relative, should not do anything since 
   * size is unknown. */
  gst_segment_set_seek (&segment, 1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, 200, GST_SEEK_TYPE_CUR, -100, &update);
  fail_unless (segment.start == 100);
  fail_unless (segment.stop == -1);
  fail_unless (update == FALSE);

  /* do some clipping on the open range */
  /* completely outside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 0, 50, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* touching lower bound, still outside of the segment */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, 100, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* partially inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, 150, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == 150);

  /* inside, touching lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      100, 150, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == 150);

  /* special case, 0 duration */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      100, 100, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == 100);

  /* completely inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      150, 200, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 150);
  fail_unless (cstop == 200);

  /* invalid start */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, -1, 100, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* start outside, we don't know the stop */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == -1);

  /* start on lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 100, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == -1);

  /* start inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 150, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 150);
  fail_unless (cstop == -1);

  /* add 100 to start, set stop to 300 */
  gst_segment_set_seek (&segment, 1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_CUR, 100, GST_SEEK_TYPE_SET, 300, &update);
  fail_unless (segment.start == 200);
  fail_unless (segment.stop == 300);
  fail_unless (update == TRUE);

  update = FALSE;
  /* add 100 to start (to 300), set stop to 200, this is not allowed. 
   * nothing should be updated in the segment. A g_warning is
   * emited. */
  ASSERT_CRITICAL(gst_segment_set_seek (&segment, 1.0,
          GST_FORMAT_BYTES,
          GST_SEEK_FLAG_NONE,
          GST_SEEK_TYPE_CUR, 100, GST_SEEK_TYPE_SET, 200, &update));
          
  fail_unless (segment.start == 200);
  fail_unless (segment.stop == 300);
  /* update didn't change */
  fail_unless (update == FALSE);

  update = TRUE;
  /* seek relative to end, should not do anything since size is
   * unknown. */
  gst_segment_set_seek (&segment, 1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_END, -300, GST_SEEK_TYPE_END, -100, &update);
  fail_unless (segment.start == 200);
  fail_unless (segment.stop == 300);
  fail_unless (update == FALSE);

  /* completely outside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 0, 50, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* touching lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, 200, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* partially inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, 250, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 200);
  fail_unless (cstop == 250);

  /* inside, touching lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      200, 250, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 200);
  fail_unless (cstop == 250);

  /* completely inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      250, 290, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 250);
  fail_unless (cstop == 290);

  /* partially inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      250, 350, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 250);
  fail_unless (cstop == 300);

  /* invalid start */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, -1, 100, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* start outside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 200);
  fail_unless (cstop == 300);

  /* start on lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 200, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 200);
  fail_unless (cstop == 300);

  /* start inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 250, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 250);
  fail_unless (cstop == 300);

  /* start outside on boundary */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 300, -1, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* start completely outside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 350, -1, &cstart, &cstop);
  fail_unless (res == FALSE);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* mess with the segment structure in the bytes format */
void segment_seek_size()
{
  GstSegment segment;
  gboolean res;
  gint64 cstart, cstop;
  gboolean update;
  
      xmlfile = "segment_seek_size";
  std_log(LOG_FILENAME_LINE, "Test Started segment_seek_size");

  gst_segment_init (&segment, GST_FORMAT_BYTES);
  gst_segment_set_duration (&segment, GST_FORMAT_BYTES, 200);

  /* configure segment to start 100 */
  gst_segment_set_seek (&segment, 1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_SET, 100, GST_SEEK_TYPE_NONE, -1, &update);
  fail_unless (segment.start == 100);
  fail_unless (segment.stop == -1);
  fail_unless (update == TRUE);

  /* configure segment to stop relative, does not update stop
   * since we did not set it before. */
  gst_segment_set_seek (&segment, 1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, 200, GST_SEEK_TYPE_CUR, -100, &update);
  fail_unless (segment.start == 100);
  fail_unless (segment.stop == -1);
  fail_unless (update == FALSE);

  /* do some clipping on the open range */
  /* completely outside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 0, 50, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* touching lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, 100, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* partially inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, 150, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == 150);

  /* inside, touching lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      100, 150, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == 150);

  /* completely inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      150, 200, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 150);
  fail_unless (cstop == 200);

  /* partially inside, clip to size */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      150, 300, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 150);
  fail_unless (cstop == 200);

  /* invalid start */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, -1, 100, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* start outside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == -1);

  /* start on lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 100, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == -1);

  /* start inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 150, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 150);
  fail_unless (cstop == -1);

  /* add 100 to start, set stop to 300, stop clips to 200 */
  gst_segment_set_seek (&segment, 1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_CUR, 100, GST_SEEK_TYPE_SET, 300, &update);
  fail_unless (segment.start == 200);
  fail_unless (segment.stop == 200);

  /* add 100 to start (to 300), set stop to 200, this clips start
   * to duration */
  gst_segment_set_seek (&segment, 1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_CUR, 100, GST_SEEK_TYPE_SET, 200, &update);
  fail_unless (segment.start == 200);
  fail_unless (segment.stop == 200);
  fail_unless (update == FALSE);

  /* seek relative to end */
  gst_segment_set_seek (&segment, 1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_END, -100, GST_SEEK_TYPE_END, -20, &update);
  fail_unless (segment.start == 100);
  fail_unless (segment.stop == 180);
  fail_unless (update == TRUE);

  /* completely outside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 0, 50, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* touching lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, 100, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* partially inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, 150, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == 150);

  /* inside, touching lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      100, 150, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == 150);

  /* completely inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      150, 170, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 150);
  fail_unless (cstop == 170);

  /* partially inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES,
      150, 250, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 150);
  fail_unless (cstop == 180);

  /* invalid start */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, -1, 100, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* start outside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 50, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == 180);

  /* start on lower bound */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 100, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 100);
  fail_unless (cstop == 180);

  /* start inside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 150, -1, &cstart, &cstop);
  fail_unless (res == TRUE);
  fail_unless (cstart == 150);
  fail_unless (cstop == 180);

  /* start outside on boundary */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 180, -1, &cstart, &cstop);
  fail_unless (res == FALSE);

  /* start completely outside */
  res = gst_segment_clip (&segment, GST_FORMAT_BYTES, 250, -1, &cstart, &cstop);
  fail_unless (res == FALSE);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


void segment_seek_reverse()
{
  GstSegment segment;
  gboolean update;

  xmlfile = "segment_seek_reverse";
  std_log(LOG_FILENAME_LINE, "Test Started segment_seek_reverse");

  gst_segment_init (&segment, GST_FORMAT_BYTES);
  gst_segment_set_duration (&segment, GST_FORMAT_BYTES, 200);

  /* configure segment to stop 100 */
  gst_segment_set_seek (&segment, -1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_SET, 100, &update);
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 100);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.last_stop == 100);
  
  fail_unless (update == TRUE);
  

  /* update */
  gst_segment_set_seek (&segment, -1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_SET, 10, GST_SEEK_TYPE_CUR, -20, &update);
  fail_unless (segment.start == 10);
  
  fail_unless (segment.stop == 80);
  
  fail_unless (segment.time == 10);
  
  fail_unless (segment.last_stop == 80);
  
  fail_unless (update == TRUE);
  

  gst_segment_set_seek (&segment, -1.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_SET, 20, GST_SEEK_TYPE_NONE, 0, &update);
  fail_unless (segment.start == 20);
  
  fail_unless (segment.stop == 80);
  
  fail_unless (segment.time == 20);
  
  fail_unless (segment.last_stop == 80);
  
  fail_unless (update == FALSE);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* mess with the segment structure in the bytes format */
void segment_seek_rate()
{
  GstSegment segment;
  gboolean update;

  xmlfile = "segment_seek_rate";
  std_log(LOG_FILENAME_LINE, "Test Started segment_seek_rate");
  
  gst_segment_init (&segment, GST_FORMAT_BYTES);

  /* configure segment to rate 2.0, format does not matter when we don't specify
   * a start or stop position. */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_UNDEFINED,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_NONE, -1, &update);
  fail_unless (segment.format == GST_FORMAT_BYTES);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == -1);
  
  fail_unless (segment.rate == 2.0);
  
  fail_unless (update == FALSE);
  

  /* 0 is the same in all formats and should not fail */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, -1, &update);
  fail_unless (segment.format == GST_FORMAT_BYTES);
  

  /* set to -1 means start from 0 */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_SET, -1, GST_SEEK_TYPE_NONE, -1, &update);
  fail_unless (segment.format == GST_FORMAT_BYTES);
  
  fail_unless (segment.start == 0);
  

  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_CUR, 0, GST_SEEK_TYPE_NONE, -1, &update);

  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_END, 0, GST_SEEK_TYPE_NONE, -1, &update);

  /* -1 for end is fine too in all formats */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_SET, -1, &update);

  /* 0 as relative end is fine too */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_CUR, 0, &update);

  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_END, 0, &update);

  /* set a real stop position, this must happen in bytes */
  gst_segment_set_seek (&segment, 3.0,
      GST_FORMAT_BYTES,
      GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_SET, 100, &update);
  fail_unless (segment.format == GST_FORMAT_BYTES);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 100);
  
  fail_unless (segment.rate == 3.0);
  
  /* no seek should happen, we just updated the stop position in forward
   * playback mode.*/
  fail_unless (update == FALSE);
  

  /* 0 as relative end is fine too */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_CUR, 0, &update);
  fail_unless (segment.stop == 100);
  

  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_END, 0, &update);
  fail_unless (segment.stop == 100);
  

  /* -1 for end is fine too in all formats */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_SET, -1, &update);
  fail_unless (segment.stop == -1);
  

  /* set some duration, stop -1 END seeks will now work with the
   * duration, if the formats match */
  gst_segment_set_duration (&segment, GST_FORMAT_BYTES, 200);
  fail_unless (segment.duration == 200);
  

  /* seek to end in any format with 0 should set the stop to the
   * duration */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_TIME, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_END, 0, &update);
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.duration == 200);
  

  /* subtract 100 from the end */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_BYTES, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_END, -100, &update);
  fail_unless (segment.stop == 100);
  
  fail_unless (segment.duration == 200);
  

  /* add 100 to the duration, this should be clamped to the duration */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_BYTES, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_NONE, -1, GST_SEEK_TYPE_END, 100, &update);
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.duration == 200);
  

  /* add 300 to the start, this should be clamped to the duration */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_BYTES, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_CUR, 300, GST_SEEK_TYPE_END, 0, &update);
  fail_unless (segment.start == 200);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.duration == 200);
  

  /* subtract 300 from the start, this should be clamped to 0 */
  gst_segment_set_seek (&segment, 2.0,
      GST_FORMAT_BYTES, GST_SEEK_FLAG_NONE,
      GST_SEEK_TYPE_CUR, -300, GST_SEEK_TYPE_END, 0, &update);
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.duration == 200);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* mess with the segment structure in the bytes format */
void segment_newsegment_open()
{
  GstSegment segment;

  xmlfile = "segment_newsegment_open";
  std_log(LOG_FILENAME_LINE, "Test Started segment_newsegment_open");

  gst_segment_init (&segment, GST_FORMAT_BYTES);

  /* time should also work for starting from 0 */
  gst_segment_set_newsegment (&segment, FALSE, 1.0, GST_FORMAT_TIME, 0, -1, 0);

  fail_unless (segment.rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_BYTES);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == -1);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* we set stop but in the wrong format, stop stays open. */
  gst_segment_set_newsegment (&segment, FALSE, 1.0, GST_FORMAT_TIME, 0, 200, 0);

  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == -1);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  

  /* update, nothing changes */
  gst_segment_set_newsegment (&segment, TRUE, 1.0, GST_FORMAT_BYTES, 0, -1, 0);

  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == -1);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  

  /* update */
  gst_segment_set_newsegment (&segment, TRUE, 1.0,
      GST_FORMAT_BYTES, 100, -1, 100);

  fail_unless (segment.start == 100);
  
  fail_unless (segment.stop == -1);
  
  fail_unless (segment.time == 100);
  
  fail_unless (segment.accum == 100);
  

  /* last_stop 0, accum does not change */
  gst_segment_set_newsegment (&segment, FALSE, 1.0, GST_FORMAT_BYTES, 0, -1, 0);

  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == -1);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 100);
  

  gst_segment_set_last_stop (&segment, GST_FORMAT_BYTES, 200);

  /* last_stop 200, accum changes */
  gst_segment_set_newsegment (&segment, FALSE, 1.0, GST_FORMAT_BYTES, 0, -1, 0);

  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == -1);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 300);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);

}


/* mess with the segment structure in the bytes format */
void segment_newsegment_closed()
{
  GstSegment segment;

  xmlfile = "segment_newsegment_closed";
  std_log(LOG_FILENAME_LINE, "Test Started segment_newsegment_closed");

  gst_segment_init (&segment, GST_FORMAT_BYTES);

  gst_segment_set_newsegment (&segment, FALSE, 1.0,
      GST_FORMAT_BYTES, 0, 200, 0);

  fail_unless (segment.rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_BYTES);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* do an update */
  gst_segment_set_newsegment (&segment, TRUE, 1.0, GST_FORMAT_BYTES, 0, 300, 0);

  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 300);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  

  /* and a new accumulated one */
  gst_segment_set_newsegment (&segment, FALSE, 1.0,
      GST_FORMAT_BYTES, 100, 400, 300);

  fail_unless (segment.start == 100);
  
  fail_unless (segment.stop == 400);
  
  fail_unless (segment.time == 300);
  
  fail_unless (segment.accum == 300);
  

  /* and a new updated one */
  gst_segment_set_newsegment (&segment, TRUE, 1.0,
      GST_FORMAT_BYTES, 100, 500, 300);

  fail_unless (segment.start == 100);
  
  fail_unless (segment.stop == 500);
  
  fail_unless (segment.time == 300);
  
  fail_unless (segment.accum == 300);
  

  /* and a new partially updated one */
  gst_segment_set_newsegment (&segment, TRUE, 1.0,
      GST_FORMAT_BYTES, 200, 500, 400);

  fail_unless (segment.start == 200);
  
  fail_unless (segment.stop == 500);
  
  fail_unless (segment.time == 400);
  
  fail_unless (segment.accum == 400);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}



/* mess with the segment structure in the time format */
void segment_newsegment_streamtime()
{
  GstSegment segment;
  gint64 result;

  xmlfile = "segment_newsegment_streamtime";
  std_log(LOG_FILENAME_LINE, "Test Started segment_newsegment_streamtime");

  gst_segment_init (&segment, GST_FORMAT_TIME);

  /***************************
   * Normal segment
   ***************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, 1.0,
      GST_FORMAT_TIME, 0, 200, 0);

  fail_unless (segment.rate == 1.0);
  
  fail_unless (segment.applied_rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 0);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 100);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 200);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  

  /*********************
   * time shifted by 500
   *********************/
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, 1.0,
      GST_FORMAT_TIME, 0, 200, 500);

  fail_unless (segment.accum == 200);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 500);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 600);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 500);
  fail_unless (result == -1);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  

  /*********************
   * time offset by 500
   *********************/
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, 1.0,
      GST_FORMAT_TIME, 500, 700, 0);

  fail_unless (segment.accum == 400);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* before segment is invalid */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 400);
  fail_unless (result == -1);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 500);
  fail_unless (result == 0);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 600);
  fail_unless (result == 100);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 700);
  fail_unless (result == 200);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 800);
  fail_unless (result == -1);
  

  /*************************************
   * time offset by 500, shifted by 200
   *************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, 1.0,
      GST_FORMAT_TIME, 500, 700, 200);

  fail_unless (segment.accum == 600);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* before segment is invalid */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 400);
  fail_unless (result == -1);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 500);
  fail_unless (result == 200);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 600);
  fail_unless (result == 300);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 700);
  fail_unless (result == 400);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 800);
  fail_unless (result == -1);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* mess with the segment structure in the time format */
void segment_newsegment_streamtime_rate()
{
  GstSegment segment;
  gint64 result;

  xmlfile = "segment_newsegment_streamtime_rate";
  std_log(LOG_FILENAME_LINE, "Test Started segment_newsegment_streamtime_rate");

  gst_segment_init (&segment, GST_FORMAT_TIME);

  /***************************
   * Normal segment rate 2.0
   ***************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 2.0, 1.0,
      GST_FORMAT_TIME, 0, 200, 0);

  fail_unless (segment.rate == 2.0);
  
  fail_unless (segment.applied_rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 0);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 100);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 150);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 200);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  

  /***************************************
   * Normal segment rate 2.0, offset
   ***************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 2.0, 1.0,
      GST_FORMAT_TIME, 100, 300, 0);

  fail_unless (segment.accum == 100);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 0);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 100);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 250);
  fail_unless (result == 150);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == 200);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 400);
  fail_unless (result == -1);
  

  /***************************************
   * Normal segment rate -1.0, offset
   ***************************************/

  /* buffers will arrive from 300 to 100 in a sink, stream time
   * calculation is unaffected by the rate */
  gst_segment_set_newsegment_full (&segment, FALSE, -1.0, 1.0,
      GST_FORMAT_TIME, 100, 300, 0);

  fail_unless (segment.accum == 200);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 0);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 100);
  

  /***********************************************
   * Normal segment rate -1.0, offset, time = 200
   ***********************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, -1.0, 1.0,
      GST_FORMAT_TIME, 100, 300, 200);

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 200);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 300);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == 400);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 400);
  fail_unless (result == -1);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* mess with the segment structure in the time format */
void segment_newsegment_streamtime_applied_rate()
{
  GstSegment segment;
  gint64 result;

  xmlfile = "segment_newsegment_streamtime_applied_rate";
  std_log(LOG_FILENAME_LINE, "Test Started segment_newsegment_streamtime_applied_rate");

  gst_segment_init (&segment, GST_FORMAT_TIME);

  /***********************************************************
   * Normal segment rate 1.0, applied rate -1.0
   * This means the timestamps represents a stream going backwards
   * starting from @time to 0.
   ************************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, -1.0,
      GST_FORMAT_TIME, 0, 200, 200);

  fail_unless (segment.rate == 1.0);
  
  fail_unless (segment.applied_rate == -1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 200);
  
  fail_unless (segment.accum == 0);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* we count backwards from 200 */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 200);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 100);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 50);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 0);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  

  /***********************************************************
   * Normal segment rate 1.0, applied rate 2.0
   * This means the timestamps represents a stream at twice the
   * normal rate
   ************************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, 2.0,
      GST_FORMAT_TIME, 0, 200, 0);

  fail_unless (segment.rate == 1.0);
  
  fail_unless (segment.applied_rate == 2.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 200);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 0);
  

  /* the stream prepresents a stream going twice as fast, the position 
   * in the segment is therefore scaled by the applied rate */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 200);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 300);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 400);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  

  /***********************************************************
   * Normal segment rate 1.0, applied rate -2.0
   * This means the timestamps represents a stream at twice the
   * reverse rate
   ************************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, -2.0,
      GST_FORMAT_TIME, 0, 200, 400);

  fail_unless (segment.rate == 1.0);
  
  fail_unless (segment.applied_rate == -2.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 400);
  
  /* previous segment lasted 200, rate of 2.0 was already applied */
  fail_unless (segment.accum == 400);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* we count backwards from 400 */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 400);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 200);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 100);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 0);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  

  /***********************************************************
   * Normal segment rate 1.0, applied rate -2.0
   * This means the timestamps represents a stream at twice the
   * reverse rate, start time cannot compensate the complete
   * duration of the segment so we stop at 0
   ************************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, -2.0,
      GST_FORMAT_TIME, 0, 200, 200);

  fail_unless (segment.rate == 1.0);
  
  fail_unless (segment.applied_rate == -2.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 200);
  
  fail_unless (segment.accum == 600);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* we count backwards from 200 */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 200);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 0);
  

  /* clamp at 0 */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 0);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 0);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* mess with the segment structure in the time format */
void  segment_newsegment_streamtime_applied_rate_rate()
{
  GstSegment segment;
  gint64 result;

  xmlfile = "segment_newsegment_streamtime_applied_rate_rate";
  std_log(LOG_FILENAME_LINE, "Test Started segment_newsegment_streamtime_applied_rate_rate");

  gst_segment_init (&segment, GST_FORMAT_TIME);

  /***********************************************************
   * Segment rate 2.0, applied rate 2.0
   * this means we have a double speed stream that we should
   * speed up by a factor of 2.0 some more. the resulting
   * stream will be played at four times the speed. 
   ************************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 2.0, 2.0,
      GST_FORMAT_TIME, 0, 200, 0);

  fail_unless (segment.rate == 2.0);
  
  fail_unless (segment.applied_rate == 2.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* only applied rate affects our calculation of the stream time */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 0);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 200);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 300);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 400);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  

  /***********************************************************
   * Segment rate 2.0, applied rate -1.0
   * this means we have a reverse stream that we should
   * speed up by a factor of 2.0
   ************************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 2.0, -1.0,
      GST_FORMAT_TIME, 0, 200, 200);

  fail_unless (segment.rate == 2.0);
  
  fail_unless (segment.applied_rate == -1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 200);
  
  /* previous segment lasted 100 */
  fail_unless (segment.accum == 100);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* only applied rate affects our calculation of the stream time */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 200);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 100);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 50);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 0);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  

  /***********************************************************
   * Segment rate -1.0, applied rate -1.0
   * this means we have a reverse stream that we should
   * reverse to get the normal stream again.
   ************************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, -1.0, -1.0,
      GST_FORMAT_TIME, 0, 200, 200);

  fail_unless (segment.rate == -1.0);
  
  fail_unless (segment.applied_rate == -1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 200);
  
  /* accumulated 100 of previous segment to make 200 */
  fail_unless (segment.accum == 200);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* only applied rate affects our calculation of the stream time */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 200);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 100);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 50);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 0);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  

  /***********************************************************
   * Segment rate -1.0, applied rate -1.0
   * this means we have a reverse stream that we should
   * reverse to get the normal stream again.
   ************************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, -1.0, 2.0,
      GST_FORMAT_TIME, 0, 200, 0);

  fail_unless (segment.rate == -1.0);
  
  fail_unless (segment.applied_rate == 2.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 400);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* only applied rate affects our calculation of the stream time */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 0);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 200);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 300);
  

  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 400);
  

  /* outside of the segment */
  result = gst_segment_to_stream_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* mess with the segment structure in the time format */
void segment_newsegment_runningtime()
{
  GstSegment segment;
  gint64 result;

  xmlfile = "segment_newsegment_runningtime";
  std_log(LOG_FILENAME_LINE, "Test Started segment_newsegment_runningtime");

  gst_segment_init (&segment, GST_FORMAT_TIME);

  /***************************
   * Normal segment
   ***************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, 1.0,
      GST_FORMAT_TIME, 0, 200, 0);

  fail_unless (segment.rate == 1.0);
  
  fail_unless (segment.applied_rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 0);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 100);
  

  /* at edge is exactly the segment duration */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 200);
  

  /* outside of the segment */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 300);
  fail_unless (result == -1);
  

  /***********************************************************
   * time shifted by 500, check if accumulation worked.
   * Rate convert to twice the speed which means scaling down
   * all positions by 2.0 in this segment.
   * Then time argument is not used at all here.
   ***********************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 2.0, 1.0,
      GST_FORMAT_TIME, 0, 200, 500);

  /* normal speed gives elapsed of 200 */
  fail_unless (segment.accum == 200);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 0);
  fail_unless (result == 200);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 250);
  

  /* outside of the segment */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 500);
  fail_unless (result == -1);
  

  /********************************************
   * time offset by 500
   * applied rate is not used for running time
   ********************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, 2.0,
      GST_FORMAT_TIME, 500, 700, 0);

  /* previous segment played at double speed gives elapsed time of
   * 100 added to previous accum of 200 gives 300. */
  fail_unless (segment.accum == 300);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* before segment is invalid */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 400);
  fail_unless (result == -1);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 500);
  fail_unless (result == 300);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 600);
  fail_unless (result == 400);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 700);
  fail_unless (result == 500);
  

  /* outside of the segment */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 800);
  fail_unless (result == -1);
  

  /**********************************************************
   * time offset by 500, shifted by 200
   * Negative rate makes the running time go backwards 
   * relative to the segment stop position. again time
   * is ignored.
   **********************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, -1.0, 1.0,
      GST_FORMAT_TIME, 500, 700, 200);

  fail_unless (segment.accum == 500);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* before segment is invalid */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 400);
  fail_unless (result == -1);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 500);
  fail_unless (result == 700);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 600);
  fail_unless (result == 600);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 700);
  fail_unless (result == 500);
  

  /* outside of the segment */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 800);
  fail_unless (result == -1);
  

  /**********************************************************
   * time offset by 500, shifted by 200
   * Negative rate makes the running time go backwards at
   * twice speed relative to the segment stop position. again 
   * time is ignored.
   **********************************************************/
  gst_segment_set_newsegment_full (&segment, FALSE, -2.0, -2.0,
      GST_FORMAT_TIME, 500, 700, 200);

  fail_unless (segment.accum == 700);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  /* before segment is invalid */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 400);
  fail_unless (result == -1);
  

  /* total scaled segment time is 100, accum is 700, so we get 800 */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 500);
  fail_unless (result == 800);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 600);
  fail_unless (result == 750);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 700);
  fail_unless (result == 700);
  

  /* outside of the segment */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 800);
  fail_unless (result == -1);
  

  /* see if negative rate closed segment correctly */
  gst_segment_set_newsegment_full (&segment, FALSE, -2.0, -1.0,
      GST_FORMAT_TIME, 500, 700, 200);

  /* previous segment lasted 100, and was at 700 so we should get 800 */
  fail_unless (segment.accum == 800);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* mess with the segment structure in the time format */
void segment_newsegment_accum()
{
  GstSegment segment;
  gint64 result;

  xmlfile = "segment_newsegment_accum";
  std_log(LOG_FILENAME_LINE, "Test Started segment_newsegment_accum");

  gst_segment_init (&segment, GST_FORMAT_TIME);

  /***************************
   * Normal reverse segment
   ***************************/
  gst_segment_set_newsegment_full (&segment, FALSE, -1.0, 1.0,
      GST_FORMAT_TIME, 0, 200, 0);

  fail_unless (segment.rate == -1.0);
  
  fail_unless (segment.applied_rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 0);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 50);
  

  /* update segment, this accumulates 50 from the previous segment. */
  gst_segment_set_newsegment_full (&segment, TRUE, -2.0, 1.0,
      GST_FORMAT_TIME, 0, 150, 0);

  fail_unless (segment.rate == -2.0);
  
  fail_unless (segment.applied_rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 150);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 50);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 50);
  

  /* 50 accumulated + 50 / 2 */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 75);
  

  /* update segment, this does not accumulate anything. */
  gst_segment_set_newsegment_full (&segment, TRUE, 1.0, 1.0,
      GST_FORMAT_TIME, 100, 200, 100);

  fail_unless (segment.rate == 1.0);
  
  fail_unless (segment.applied_rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 100);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 100);
  
  fail_unless (segment.accum == 50);
  
  fail_unless (segment.last_stop == 100);
  
  fail_unless (segment.duration == -1);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 100);
  fail_unless (result == 50);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 100);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/* mess with the segment structure in the time format */
void segment_newsegment_accum2()
{
  GstSegment segment;
  gint64 result;

  xmlfile = "segment_newsegment_accum2";
  std_log(LOG_FILENAME_LINE, "Test Started segment_newsegment_accum2");

  gst_segment_init (&segment, GST_FORMAT_TIME);

  /***************************
   * Normal reverse segment
   ***************************/
  gst_segment_set_newsegment_full (&segment, FALSE, -1.0, 1.0,
      GST_FORMAT_TIME, 0, 200, 0);

  fail_unless (segment.rate == -1.0);
  
  fail_unless (segment.applied_rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 0);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  
  fail_unless (segment.last_stop == 0);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 0);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 50);
  

  /* close segment, this accumulates nothing. */
  gst_segment_set_newsegment_full (&segment, TRUE, -1.0, 1.0,
      GST_FORMAT_TIME, 150, 200, 0);

  fail_unless (segment.rate == -1.0);
  
  fail_unless (segment.applied_rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 150);
  
  fail_unless (segment.stop == 200);
  
  fail_unless (segment.time == 0);
  
  fail_unless (segment.accum == 0);
  
  fail_unless (segment.last_stop == 150);
  
  fail_unless (segment.duration == -1);
  

  /* new segment, this accumulates 50. */
  gst_segment_set_newsegment_full (&segment, FALSE, 1.0, 1.0,
      GST_FORMAT_TIME, 150, 300, 150);

  fail_unless (segment.rate == 1.0);
  
  fail_unless (segment.applied_rate == 1.0);
  
  fail_unless (segment.format == GST_FORMAT_TIME);
  
  fail_unless (segment.flags == 0);
  
  fail_unless (segment.start == 150);
  
  fail_unless (segment.stop == 300);
  
  fail_unless (segment.time == 150);
  
  fail_unless (segment.accum == 50);
  
  fail_unless (segment.last_stop == 150);
  
  fail_unless (segment.duration == -1);
  

  /* invalid time gives invalid result */
  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, -1);
  fail_unless (result == -1);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 150);
  fail_unless (result == 50);
  

  result = gst_segment_to_running_time (&segment, GST_FORMAT_TIME, 200);
  fail_unless (result == 100);
  
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
}


/*void main(int argc,char** argv)
{
        gst_init(&argc,&argv);
        segment_seek_reverse();
        segment_seek_rate();
        segment_newsegment_open();
        segment_newsegment_closed();
        segment_newsegment_streamtime();
        segment_newsegment_streamtime_rate();
        segment_newsegment_streamtime_applied_rate();
        segment_newsegment_streamtime_applied_rate_rate();
        segment_newsegment_runningtime();
        segment_newsegment_accum();
        segment_newsegment_accum2();
}*/


void (*fn[13]) (void) = {
        segment_seek_reverse,
        segment_seek_rate,
        segment_newsegment_open,
        segment_newsegment_closed,
        segment_newsegment_streamtime,
        segment_newsegment_streamtime_rate,
        segment_newsegment_streamtime_applied_rate,
        segment_newsegment_streamtime_applied_rate_rate,
        segment_newsegment_runningtime,
        segment_newsegment_accum,
        segment_newsegment_accum2,
        segment_seek_nosize,
        segment_seek_size
};

char *args[] = {
        "segment_seek_reverse",
        "segment_seek_rate",
        "segment_newsegment_open",
        "segment_newsegment_closed",
        "segment_newsegment_streamtime",
        "segment_newsegment_streamtime_rate",
        "segment_newsegment_streamtime_applied_rate",
        "segment_newsegment_streamtime_applied_rate_rate",
        "segment_newsegment_runningtime",
        "segment_newsegment_accum",
        "segment_newsegment_accum2",
        "segment_seek_nosize",
        "segment_seek_size"
};

GST_CHECK_MAIN (gst_segment);
