/* GStreamer
 *
 * unit test for cddabasesrc
 *
 * Copyright (C) <2005> Tim-Philipp Müller <tim centricular net>
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

/* TODO:
 *  - test different modes (when seeking to tracks in track mode, buffer
 *    timestamps should start from 0, when seeking to tracks in disc mode,
 *    buffer timestamps should increment, etc.)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include <gst/check/gstcheck.h>
#include <gst/check/gstbufferstraw.h>

#include <gst/cdda/gstcddabasesrc.h>
#include <string.h>

#define CD_FRAMESIZE_RAW 2352

#define GST_TYPE_CD_FOO_SRC            (gst_cd_foo_src_get_type())
#define GST_CD_FOO_SRC(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CD_FOO_SRC,GstCdFooSrc))
#define GST_CD_FOO_SRC_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CD_FOO_SRC,GstCdFooSrcClass))
#define GST_IS_CD_FOO_SRC(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CD_FOO_SRC))
#define GST_IS_CD_FOO_SRC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CD_FOO_SRC))
#define GST_CD_FOO_SRC_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_CDDA_BASAE_SRC, GstCdFooSrcClass))

typedef struct _GstCdFooSrc GstCdFooSrc;
typedef struct _GstCdFooSrcClass GstCdFooSrcClass;


/* Neue Heimat (CD 2) */
static GstCddaBaseSrcTrack nh_cd2_tracks[] = {
  {TRUE, 1, 0, 20664, NULL,},
  {TRUE, 2, 20665, 52377, NULL,},
  {TRUE, 3, 52378, 84100, NULL,},
  {TRUE, 4, 84101, 105401, NULL,},
  {TRUE, 5, 105402, 123060, NULL,},
  {TRUE, 6, 123061, 146497, NULL,},
  {TRUE, 7, 146498, 175693, NULL,},
  {TRUE, 8, 175694, 203272, NULL,},
  {TRUE, 9, 203273, 217909, NULL,},
  {TRUE, 10, 217910, 240938, NULL,},
  {TRUE, 11, 240939, 256169, NULL,},
  {TRUE, 12, 256170, 282237, NULL,},
  {TRUE, 13, 282238, 307606, NULL,},
  {TRUE, 14, 307607, 337245, NULL,}
};

/* Offspring - Smash */
static GstCddaBaseSrcTrack offspring_tracks[] = {
  {TRUE, 1, 0, 1924, NULL,},
  {TRUE, 2, 1925, 12947, NULL,},
  {TRUE, 3, 12948, 29739, NULL,},
  {TRUE, 4, 29740, 47202, NULL,},
  {TRUE, 5, 47203, 63134, NULL,},
  {TRUE, 6, 63135, 77954, NULL,},
  {TRUE, 7, 77955, 92789, NULL,},
  {TRUE, 8, 92790, 112127, NULL,},
  {TRUE, 9, 112128, 124372, NULL,},
  {TRUE, 10, 124373, 133574, NULL,},
  {TRUE, 11, 133575, 143484, NULL,},
  {TRUE, 12, 143485, 149279, NULL,},
  {TRUE, 13, 149280, 162357, NULL,},
  {TRUE, 14, 162358, 210372, NULL,}
};

/* this matches the sample TOC from the DiscIDCalculation
 * page in the Musicbrainz wiki. It's a tricky one because
 * it's got a data track as well. */
static GstCddaBaseSrcTrack mb_sample_tracks[] = {
  {TRUE, 1, 0, 18640, NULL,},
  {TRUE, 2, 18641, 34666, NULL,},
  {TRUE, 3, 34667, 56349, NULL,},
  {TRUE, 4, 56350, 77005, NULL,},
  {TRUE, 5, 77006, 106093, NULL,},
  {TRUE, 6, 106094, 125728, NULL,},
  {TRUE, 7, 125729, 149784, NULL,},
  {TRUE, 8, 149785, 168884, NULL,},
  {TRUE, 9, 168885, 185909, NULL,},
  {TRUE, 10, 185910, 205828, NULL,},
  {TRUE, 11, 205829, 230141, NULL,},
  {TRUE, 12, 230142, 246658, NULL,},
  {TRUE, 13, 246659, 265613, NULL,},
  {TRUE, 14, 265614, 289478, NULL,},
  {FALSE, 15, 289479, 325731, NULL,}
};

/* Nicola Conte - Other Directions (also
 * tricky due to the extra data track) */
static GstCddaBaseSrcTrack nconte_odir_tracks[] = {
  {TRUE, 1, 0, 17852, NULL,},
  {TRUE, 2, 17853, 39956, NULL,},
  {TRUE, 3, 39957, 68449, NULL,},
  {TRUE, 4, 68450, 88725, NULL,},
  {TRUE, 5, 88726, 106413, NULL,},
  {TRUE, 6, 106414, 131966, NULL,},
  {TRUE, 7, 131967, 152372, NULL,},
  {TRUE, 8, 152373, 168602, NULL,},
  {TRUE, 9, 168603, 190348, NULL,},
  {TRUE, 10, 190349, 209044, NULL,},
  {TRUE, 11, 209045, 235586, NULL,},
  {TRUE, 12, 235587, 253830, NULL,},
  {TRUE, 13, 253831, 272213, NULL,},
  {FALSE, 14, 272214, 332849, NULL,}
};

/* Pink Martini - Sympathique (11 track version) */
static GstCddaBaseSrcTrack pm_symp_tracks[] = {
  {TRUE, 1, 0, 21667, NULL,},
  {TRUE, 2, 21668, 49576, NULL,},
  {TRUE, 3, 49577, 62397, NULL,},
  {TRUE, 4, 62398, 81087, NULL,},
  {TRUE, 5, 81088, 106595, NULL,},
  {TRUE, 6, 106596, 122012, NULL,},
  {TRUE, 7, 122013, 138469, NULL,},
  {TRUE, 8, 138470, 157306, NULL,},
  {TRUE, 9, 157307, 179635, NULL,},
  {TRUE, 10, 179636, 203673, NULL,},
  {TRUE, 11, 203674, 213645, NULL,}
};

#define NUM_TEST_DISCS 5

struct _test_disc
{
  GstCddaBaseSrcTrack *tracks;
  guint num_tracks;
  guint32 cddb_discid;
  const gchar *musicbrainz_discid;
};

/* FIXME: now we just need to find out how to treat
 * data tracks for the cddb id calculation .... */
static struct _test_disc test_discs[NUM_TEST_DISCS] = {
  {nh_cd2_tracks, G_N_ELEMENTS (nh_cd2_tracks), 0xae11900e,
      NULL},
  {mb_sample_tracks, G_N_ELEMENTS (mb_sample_tracks), 0x00000000,
      "MUtMmKN402WPj3_VFsgUelxpc8U-"},
  {offspring_tracks, G_N_ELEMENTS (offspring_tracks), 0xc20af40e,
      "ahg7JUcfR3vCYBphSDIogOOWrr0-"},
  {nconte_odir_tracks, G_N_ELEMENTS (nconte_odir_tracks), 0x00000000,
        /* hKx_PejjG47X161ND_Sh0HyqaS0- according to libmusicbrainz, but that's
         * wrong according to the wiki docs (or not?) (neither discid is listed) */
      "fboaOQtfqwENv8WyXa9tRyvyUbQ-"},
  {pm_symp_tracks, G_N_ELEMENTS (pm_symp_tracks), 0xa00b200b,
      "iP0DOLdr4vt_IfKSIXoRUR.q_Wc-"}
};

struct _GstCdFooSrc
{
  GstCddaBaseSrc cddabasesrc;

  struct _test_disc *cur_test;
  guint cur_disc;
};

struct _GstCdFooSrcClass
{
  GstCddaBaseSrcClass parent_class;
};

GST_BOILERPLATE (GstCdFooSrc, gst_cd_foo_src, GstCddaBaseSrc,
    GST_TYPE_CDDA_BASE_SRC);

static GstBuffer *gst_cd_foo_src_read_sector (GstCddaBaseSrc * src,
    gint sector);
static gboolean gst_cd_foo_src_open (GstCddaBaseSrc * src,
    const gchar * device);
static void gst_cd_foo_src_close (GstCddaBaseSrc * src);

static const GstElementDetails cdfoo_details =
GST_ELEMENT_DETAILS ("CD Audio (cdda) Source, FooBar",
    "Source/File",
    "Read audio from CD",
    "Foo Bar <foo@bar.com>");

static void
gst_cd_foo_src_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_set_details (element_class, &cdfoo_details);
}

static void
gst_cd_foo_src_init (GstCdFooSrc * src, GstCdFooSrcClass * klass)
{
  src->cur_disc = 0;
}

static void
gst_cd_foo_src_class_init (GstCdFooSrcClass * klass)
{
  GstCddaBaseSrcClass *cddabasesrc_class = GST_CDDA_BASE_SRC_CLASS (klass);

  cddabasesrc_class->open = gst_cd_foo_src_open;
  cddabasesrc_class->close = gst_cd_foo_src_close;
  cddabasesrc_class->read_sector = gst_cd_foo_src_read_sector;
}

static gboolean
gst_cd_foo_src_open (GstCddaBaseSrc * cddabasesrc, const gchar * device)
{
  GstCddaBaseSrcTrack *tracks;
  GstCdFooSrc *src;
  gint i;

  src = GST_CD_FOO_SRC (cddabasesrc);

  /* if this fails, the test is wrong */
  g_assert (src->cur_disc < NUM_TEST_DISCS);

  src->cur_test = &test_discs[src->cur_disc];

  /* add tracks */
  tracks = src->cur_test->tracks;
  for (i = 0; i < src->cur_test->num_tracks; ++i) {
    gst_cdda_base_src_add_track (GST_CDDA_BASE_SRC (src), &tracks[i]);
  }

  return TRUE;
}

static void
gst_cd_foo_src_close (GstCddaBaseSrc * cddabasesrc)
{
  GstCdFooSrc *src = GST_CD_FOO_SRC (cddabasesrc);

  if (src->cur_test->cddb_discid != 0) {
    g_assert (cddabasesrc->discid == src->cur_test->cddb_discid);
  }

  if (src->cur_test->musicbrainz_discid != NULL) {
    g_assert (g_str_equal (cddabasesrc->mb_discid,
            src->cur_test->musicbrainz_discid));
  }
}

static GstBuffer *
gst_cd_foo_src_read_sector (GstCddaBaseSrc * cddabasesrc, gint sector)
{
  GstBuffer *buf;

  buf = gst_buffer_new_and_alloc (CD_FRAMESIZE_RAW);
  memset (GST_BUFFER_DATA (buf), 0, CD_FRAMESIZE_RAW);

  return buf;
}

GST_START_TEST (test_discid_calculations)
{
  GstElement *foosrc;
  gint i;

  fail_unless (gst_element_register (NULL, "cdfoosrc", GST_RANK_SECONDARY,
          GST_TYPE_CD_FOO_SRC));

  foosrc = gst_element_factory_make ("cdfoosrc", "cdfoosrc");

  for (i = 0; i < G_N_ELEMENTS (test_discs); ++i) {
    GST_LOG ("Testing disc layout %u ...", i);
    GST_CD_FOO_SRC (foosrc)->cur_disc = i;
    gst_element_set_state (foosrc, GST_STATE_PLAYING);
    gst_element_get_state (foosrc, NULL, NULL, -1);
    gst_element_set_state (foosrc, GST_STATE_NULL);
  }

  gst_object_unref (foosrc);

  gst_task_cleanup_all ();
}

GST_END_TEST;

GST_START_TEST (test_buffer_timestamps)
{
  GstElement *foosrc, *pipeline, *fakesink;
  GstClockTime prev_ts, prev_duration, ts;
  GstPad *sinkpad;
  gint i;

  fail_unless (gst_element_register (NULL, "cdfoosrc", GST_RANK_SECONDARY,
          GST_TYPE_CD_FOO_SRC));

  pipeline = gst_pipeline_new ("pipeline");
  foosrc = gst_element_factory_make ("cdfoosrc", "cdfoosrc");
  fakesink = gst_element_factory_make ("fakesink", "fakesink");
  gst_bin_add_many (GST_BIN (pipeline), foosrc, fakesink, NULL);
  fail_unless (gst_element_link (foosrc, fakesink));
  sinkpad = gst_element_get_pad (fakesink, "sink");

  GST_CD_FOO_SRC (foosrc)->cur_disc = 0;

  gst_buffer_straw_start_pipeline (pipeline, sinkpad);

  prev_ts = GST_CLOCK_TIME_NONE;
  prev_duration = GST_CLOCK_TIME_NONE;

  for (i = 0; i < 100; ++i) {
    GstBuffer *buf;

    buf = gst_buffer_straw_get_buffer (pipeline, sinkpad);
    GST_LOG ("buffer, ts=%" GST_TIME_FORMAT ", dur=%" GST_TIME_FORMAT,
        GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (buf)),
        GST_TIME_ARGS (GST_BUFFER_DURATION (buf)));
    ts = GST_BUFFER_TIMESTAMP (buf);
    fail_unless (GST_CLOCK_TIME_IS_VALID (ts));
    fail_unless (GST_BUFFER_DURATION_IS_VALID (buf));
    if (i > 0) {
      fail_unless (GST_CLOCK_TIME_IS_VALID (prev_ts));
      fail_unless (GST_CLOCK_TIME_IS_VALID (prev_duration));
      fail_unless ((prev_ts + prev_duration) == ts);
    }
    prev_ts = ts;
    prev_duration = GST_BUFFER_DURATION (buf);
    gst_buffer_unref (buf);
  }

  gst_buffer_straw_stop_pipeline (pipeline, sinkpad);

  gst_task_cleanup_all ();
  gst_object_unref (pipeline);
  gst_object_unref (sinkpad);
}

GST_END_TEST;

static Suite *
cddabasesrc_suite (void)
{
  Suite *s = suite_create ("cddabasesrc");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_discid_calculations);
  tcase_add_test (tc_chain, test_buffer_timestamps);

  return s;
}

GST_CHECK_MAIN (cddabasesrc)
