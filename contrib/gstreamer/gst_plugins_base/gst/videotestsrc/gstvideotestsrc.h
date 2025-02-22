/* GStreamer
 * Copyright (C) <2002> David A. Schleef <ds@schleef.org>
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
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

#ifndef __GST_VIDEO_TEST_SRC_H__
#define __GST_VIDEO_TEST_SRC_H__

#include <gst/gst.h>
#include <gst/base/gstpushsrc.h>

G_BEGIN_DECLS

#define GST_TYPE_VIDEO_TEST_SRC \
  (gst_video_test_src_get_type())
#define GST_VIDEO_TEST_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_VIDEO_TEST_SRC,GstVideoTestSrc))
#define GST_VIDEO_TEST_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_VIDEO_TEST_SRC,GstVideoTestSrcClass))
#define GST_IS_VIDEO_TEST_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_VIDEO_TEST_SRC))
#define GST_IS_VIDEO_TEST_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_VIDEO_TEST_SRC))

/**
 * GstVideoTestSrcPattern:
 * @GST_VIDEO_TEST_SRC_SMPTE: A standard SMPTE test pattern
 * @GST_VIDEO_TEST_SRC_SNOW: Random noise
 * @GST_VIDEO_TEST_SRC_BLACK: A black image
 * @GST_VIDEO_TEST_SRC_WHITE: A white image
 * @GST_VIDEO_TEST_SRC_RED: A red image
 * @GST_VIDEO_TEST_SRC_GREEN: A green image
 * @GST_VIDEO_TEST_SRC_BLUE: A blue image
 * @GST_VIDEO_TEST_SRC_CHECKERS1: Checkers pattern (1px)
 * @GST_VIDEO_TEST_SRC_CHECKERS2: Checkers pattern (2px)
 * @GST_VIDEO_TEST_SRC_CHECKERS4: Checkers pattern (4px)
 * @GST_VIDEO_TEST_SRC_CHECKERS8: Checkers pattern (8px)
 * @GST_VIDEO_TEST_SRC_CIRCULAR: Circular pattern
 * @GST_VIDEO_TEST_SRC_BLINK: Alternate between black and white
 *
 * The test pattern to produce.
 */
typedef enum {
  GST_VIDEO_TEST_SRC_SMPTE,
  GST_VIDEO_TEST_SRC_SNOW,
  GST_VIDEO_TEST_SRC_BLACK,
  GST_VIDEO_TEST_SRC_WHITE,
  GST_VIDEO_TEST_SRC_RED,
  GST_VIDEO_TEST_SRC_GREEN,
  GST_VIDEO_TEST_SRC_BLUE,
  GST_VIDEO_TEST_SRC_CHECKERS1,
  GST_VIDEO_TEST_SRC_CHECKERS2,
  GST_VIDEO_TEST_SRC_CHECKERS4,
  GST_VIDEO_TEST_SRC_CHECKERS8,
  GST_VIDEO_TEST_SRC_CIRCULAR,
  GST_VIDEO_TEST_SRC_BLINK
} GstVideoTestSrcPattern;

typedef struct _GstVideoTestSrc GstVideoTestSrc;
typedef struct _GstVideoTestSrcClass GstVideoTestSrcClass;

/**
 * GstVideoTestSrc:
 *
 * Opaque data structure.
 */
struct _GstVideoTestSrc {
  GstPushSrc element;

  /*< private >*/

  /* type of output */
  GstVideoTestSrcPattern pattern_type;

  /* video state */
  char *format_name;
  gint width;
  gint height;
  struct fourcc_list_struct *fourcc;
  gint bpp;
  gint rate_numerator;
  gint rate_denominator;

  /* private */
  gint64 timestamp_offset;              /* base offset */
  GstClockTime running_time;            /* total running time */
  gint64 n_frames;                      /* total frames sent */

  void (*make_image) (GstVideoTestSrc *v, unsigned char *dest, int w, int h);
};

struct _GstVideoTestSrcClass {
  GstPushSrcClass parent_class;
};

GType gst_video_test_src_get_type (void);

G_END_DECLS

#endif /* __GST_VIDEO_TEST_SRC_H__ */
