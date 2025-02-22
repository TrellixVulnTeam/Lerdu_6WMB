/* GStreamer
 *
 * unit test for audioconvert
 *
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
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
#include <unistd.h>

//#include <glib_global.h>

#define LOG_FILE "c:\\logs\\audioconvert_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include <gst/floatcast/floatcast.h>
#include <gst/check/gstcheck.h>

#include <gst/audio/multichannel.h>


#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers;
#endif

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
gboolean _gst_check_expecting_log = FALSE;
#endif

#include <libgstreamer_wsd_solution.h>

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}


/* For ease of programming we use globals to keep refs for our floating
 * src and sink pads we create; otherwise we always have to do get_pad,
 * get_peer, and then remove references in every test function */
static GstPad *mysrcpad, *mysinkpad;

#define CONVERT_CAPS_TEMPLATE_STRING    \
  "audio/x-raw-float, " \
    "rate = (int) [ 1, MAX ], " \
    "channels = (int) [ 1, 8 ], " \
    "endianness = (int) { LITTLE_ENDIAN, BIG_ENDIAN }, " \
    "width = (int) { 32, 64 };" \
  "audio/x-raw-int, " \
    "rate = (int) [ 1, MAX ], " \
    "channels = (int) [ 1, 8 ], " \
    "endianness = (int) { LITTLE_ENDIAN, BIG_ENDIAN }, " \
    "width = (int) 32, " \
    "depth = (int) [ 1, 32 ], " \
    "signed = (boolean) { true, false }; " \
  "audio/x-raw-int, " \
    "rate = (int) [ 1, MAX ], " \
    "channels = (int) [ 1, 8 ], " \
    "endianness = (int) { LITTLE_ENDIAN, BIG_ENDIAN }, " \
    "width = (int) 24, " \
    "depth = (int) [ 1, 24 ], " \
    "signed = (boolean) { true, false }; " \
  "audio/x-raw-int, " \
    "rate = (int) [ 1, MAX ], " \
    "channels = (int) [ 1, 8 ], " \
    "endianness = (int) { LITTLE_ENDIAN, BIG_ENDIAN }, " \
    "width = (int) 16, " \
    "depth = (int) [ 1, 16 ], " \
    "signed = (boolean) { true, false }; " \
  "audio/x-raw-int, " \
    "rate = (int) [ 1, MAX ], " \
    "channels = (int) [ 1, 8 ], " \
    "endianness = (int) { LITTLE_ENDIAN, BIG_ENDIAN }, " \
    "width = (int) 8, " \
    "depth = (int) [ 1, 8 ], " \
    "signed = (boolean) { true, false } "

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (CONVERT_CAPS_TEMPLATE_STRING)
    );
static GstStaticPadTemplate srctemplate = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (CONVERT_CAPS_TEMPLATE_STRING)
    );

/* takes over reference for outcaps */
static GstElement *
setup_audioconvert (GstCaps * outcaps)
{
  GstElement *audioconvert;

  GST_DEBUG ("setup_audioconvert with caps %" GST_PTR_FORMAT, outcaps);
  audioconvert = gst_check_setup_element ("audioconvert");
  g_object_set (G_OBJECT (audioconvert), "dithering", 0, NULL);
  g_object_set (G_OBJECT (audioconvert), "noise-shaping", 0, NULL);
  mysrcpad = gst_check_setup_src_pad (audioconvert, &srctemplate, NULL);
  mysinkpad = gst_check_setup_sink_pad (audioconvert, &sinktemplate, NULL);
  /* this installs a getcaps func that will always return the caps we set
   * later */
  gst_pad_use_fixed_caps (mysinkpad);
  gst_pad_set_caps (mysinkpad, outcaps);
  gst_caps_unref (outcaps);
  outcaps = gst_pad_get_negotiated_caps (mysinkpad);
  fail_unless (gst_caps_is_fixed (outcaps));
  gst_caps_unref (outcaps);

  gst_pad_set_active (mysrcpad, TRUE);
  gst_pad_set_active (mysinkpad, TRUE);

  return audioconvert;
}

static void
cleanup_audioconvert (GstElement * audioconvert)
{
  GST_DEBUG ("cleanup_audioconvert");

  gst_pad_set_active (mysrcpad, FALSE);
  gst_pad_set_active (mysinkpad, FALSE);
  gst_check_teardown_src_pad (audioconvert);
  gst_check_teardown_sink_pad (audioconvert);
  gst_check_teardown_element (audioconvert);
}

/* returns a newly allocated caps */
static GstCaps *
get_int_caps (guint channels, gchar * endianness, guint width,
    guint depth, gboolean signedness)
{
  GstCaps *caps;
  gchar *string;

  string = g_strdup_printf ("audio/x-raw-int, "
      "rate = (int) 44100, "
      "channels = (int) %d, "
      "endianness = (int) %s, "
      "width = (int) %d, "
      "depth = (int) %d, "
      "signed = (boolean) %s ",
      channels, endianness, width, depth, signedness ? "true" : "false");
  GST_DEBUG ("creating caps from %s", string);
  caps = gst_caps_from_string (string);
  g_free (string);
  fail_unless (caps != NULL);
  GST_DEBUG ("returning caps %p", caps);
  return caps;
}

/* returns a newly allocated caps */
static GstCaps *
get_float_caps (guint channels, gchar * endianness, guint width)
{
  GstCaps *caps;
  gchar *string;

  string = g_strdup_printf ("audio/x-raw-float, "
      "rate = (int) 44100, "
      "channels = (int) %d, "
      "endianness = (int) %s, "
      "width = (int) %d ", channels, endianness, width);
  GST_DEBUG ("creating caps from %s", string);
  caps = gst_caps_from_string (string);
  g_free (string);
  fail_unless (caps != NULL);
  GST_DEBUG ("returning caps %p", caps);
  return caps;
}

/* Copied from vorbis; the particular values used don't matter */
static GstAudioChannelPosition channelpositions[][6] = {
  {                             /* Mono */
      GST_AUDIO_CHANNEL_POSITION_FRONT_MONO},
  {                             /* Stereo */
        GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
      GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT},
  {                             /* Stereo + Centre */
        GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
        GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
      GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT},
  {                             /* Quadraphonic */
        GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
        GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
        GST_AUDIO_CHANNEL_POSITION_REAR_LEFT,
        GST_AUDIO_CHANNEL_POSITION_REAR_RIGHT,
      },
  {                             /* Stereo + Centre + rear stereo */
        GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
        GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
        GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
        GST_AUDIO_CHANNEL_POSITION_REAR_LEFT,
        GST_AUDIO_CHANNEL_POSITION_REAR_RIGHT,
      },
  {                             /* Full 5.1 Surround */
        GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
        GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
        GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
        GST_AUDIO_CHANNEL_POSITION_REAR_LEFT,
        GST_AUDIO_CHANNEL_POSITION_REAR_RIGHT,
        GST_AUDIO_CHANNEL_POSITION_LFE,
      }
};

/* these are a bunch of random positions, they are mostly just
 * different from the ones above, don't use elsewhere */
static GstAudioChannelPosition mixed_up_positions[][6] = {
  {
      GST_AUDIO_CHANNEL_POSITION_FRONT_MONO},
  {
        GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
      GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT},
  {
        GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
        GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
      GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT},
  {
        GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
        GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
        GST_AUDIO_CHANNEL_POSITION_REAR_RIGHT,
        GST_AUDIO_CHANNEL_POSITION_REAR_LEFT,
      },
  {
        GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
        GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
        GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
        GST_AUDIO_CHANNEL_POSITION_REAR_LEFT,
        GST_AUDIO_CHANNEL_POSITION_REAR_RIGHT,
      },
  {
        GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
        GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
        GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
        GST_AUDIO_CHANNEL_POSITION_REAR_RIGHT,
        GST_AUDIO_CHANNEL_POSITION_REAR_LEFT,
        GST_AUDIO_CHANNEL_POSITION_LFE,
      }
};

static void
set_channel_positions (GstCaps * caps, int channels,
    GstAudioChannelPosition * channelpositions)
{
  GValue chanpos = { 0 };
  GValue pos = { 0 };
  GstStructure *structure = gst_caps_get_structure (caps, 0);
  int c;

  g_value_init (&chanpos, GST_TYPE_ARRAY);
  g_value_init (&pos, GST_TYPE_AUDIO_CHANNEL_POSITION);

  for (c = 0; c < channels; c++) {
    g_value_set_enum (&pos, channelpositions[c]);
    gst_value_array_append_value (&chanpos, &pos);
  }
  g_value_unset (&pos);

  gst_structure_set_value (structure, "channel-positions", &chanpos);
  g_value_unset (&chanpos);
}

/* For channels > 2, caps have to have channel positions. This adds some simple
 * ones. Only implemented for channels between 1 and 6.
 */
static GstCaps *
get_float_mc_caps (guint channels, gchar * endianness, guint width,
    gboolean mixed_up_layout)
{
  GstCaps *caps = get_float_caps (channels, endianness, width);

  if (channels <= 6) {
    if (mixed_up_layout)
      set_channel_positions (caps, channels, mixed_up_positions[channels - 1]);
    else
      set_channel_positions (caps, channels, channelpositions[channels - 1]);
  }

  return caps;
}

static GstCaps *
get_int_mc_caps (guint channels, gchar * endianness, guint width,
    guint depth, gboolean signedness, gboolean mixed_up_layout)
{
  GstCaps *caps = get_int_caps (channels, endianness, width, depth, signedness);

  if (channels <= 6) {
    if (mixed_up_layout)
      set_channel_positions (caps, channels, mixed_up_positions[channels - 1]);
    else
      set_channel_positions (caps, channels, channelpositions[channels - 1]);
  }

  return caps;
}

/* eats the refs to the caps */
static void
verify_convert (const gchar * which, void *in, int inlength,
    GstCaps * incaps, void *out, int outlength, GstCaps * outcaps)
{
  GstBuffer *inbuffer, *outbuffer;
  GstElement *audioconvert;

  GST_DEBUG ("verifying conversion %s", which);
  GST_DEBUG ("incaps: %" GST_PTR_FORMAT, incaps);
  GST_DEBUG ("outcaps: %" GST_PTR_FORMAT, outcaps);
  ASSERT_CAPS_REFCOUNT (incaps, "incaps", 1);
  ASSERT_CAPS_REFCOUNT (outcaps, "outcaps", 1);
  audioconvert = setup_audioconvert (outcaps);
  ASSERT_CAPS_REFCOUNT (outcaps, "outcaps", 1);

  fail_unless (gst_element_set_state (audioconvert,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_SUCCESS,
      "could not set to playing");

  GST_DEBUG ("Creating buffer of %d bytes", inlength);
  inbuffer = gst_buffer_new_and_alloc (inlength);
  memcpy (GST_BUFFER_DATA (inbuffer), in, inlength);
  gst_buffer_set_caps (inbuffer, incaps);
  ASSERT_CAPS_REFCOUNT (incaps, "incaps", 2);
  ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

  /* pushing gives away my reference ... */
  GST_DEBUG ("push it");
  fail_unless (gst_pad_push (mysrcpad, inbuffer) == GST_FLOW_OK);
  GST_DEBUG ("pushed it");
  /* ... and puts a new buffer on the global list */
  fail_unless (g_list_length (buffers) == 1);
  fail_if ((outbuffer = (GstBuffer *) buffers->data) == NULL);

  ASSERT_BUFFER_REFCOUNT (outbuffer, "outbuffer", 1);
  fail_unless_equals_int (GST_BUFFER_SIZE (outbuffer), outlength);

  if (memcmp (GST_BUFFER_DATA (outbuffer), out, outlength) != 0) {
    g_print ("\nInput data:\n");
    gst_util_dump_mem (in, inlength);
    g_print ("\nConverted data:\n");
    gst_util_dump_mem (GST_BUFFER_DATA (outbuffer), outlength);
    g_print ("\nExpected data:\n");
    gst_util_dump_mem (out, outlength);
  }
  fail_unless (memcmp (GST_BUFFER_DATA (outbuffer), out, outlength) == 0,
      "failed converting %s", which);

  /* make sure that the channel positions are not lost */
  {
    GstStructure *in_s, *out_s;
    gint out_chans;

    in_s = gst_caps_get_structure (incaps, 0);
    out_s = gst_caps_get_structure (GST_BUFFER_CAPS (outbuffer), 0);
    fail_unless (gst_structure_get_int (out_s, "channels", &out_chans));

    /* positions for 1 and 2 channels are implicit if not provided */
    if (out_chans > 2 && gst_structure_has_field (in_s, "channel-positions")) {
      if (!gst_structure_has_field (out_s, "channel-positions")) {
        g_error ("Channel layout got lost somewhere:\n\nIns : %s\nOuts: %s\n",
            gst_structure_to_string (in_s), gst_structure_to_string (out_s));
      }
    }
  }

  buffers = g_list_remove (buffers, outbuffer);
  gst_buffer_unref (outbuffer);
  fail_unless (gst_element_set_state (audioconvert,
          GST_STATE_NULL) == GST_STATE_CHANGE_SUCCESS, "could not set to null");
  /* cleanup */
  GST_DEBUG ("cleanup audioconvert");
  cleanup_audioconvert (audioconvert);
  GST_DEBUG ("cleanup, unref incaps");
  ASSERT_CAPS_REFCOUNT (incaps, "incaps", 1);
  gst_caps_unref (incaps);
}


#define RUN_CONVERSION(which, inarray, in_get_caps, outarray, out_get_caps)    \
  verify_convert (which, inarray, sizeof (inarray),                            \
        in_get_caps, outarray, sizeof (outarray), out_get_caps)


void test_int16()
{
	xmlfile = "test_int16";
    std_log(LOG_FILENAME_LINE, "Test Started test_int16");
  /* stereo to mono */
  {
    gint16 in[] = { 16384, -256, 1024, 1024 };
    gint16 out[] = { 8064, 1024 };

    RUN_CONVERSION ("int16 stereo to mono",
        in, get_int_caps (2, "BYTE_ORDER", 16, 16, TRUE),
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE));
  }
  /* mono to stereo */
  {
    gint16 in[] = { 512, 1024 };
    gint16 out[] = { 512, 512, 1024, 1024 };

    RUN_CONVERSION ("int16 mono to stereo",
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE),
        out, get_int_caps (2, "BYTE_ORDER", 16, 16, TRUE));
  }
  /* signed -> unsigned */
  {
    gint16 in[] = { 0, -32767, 32767, -32768 };
    guint16 out[] = { 32768, 1, 65535, 0 };

    RUN_CONVERSION ("int16 signed to unsigned",
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE),
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, FALSE));
    RUN_CONVERSION ("int16 unsigned to signed",
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, FALSE),
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE));
  }
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}




void test_float32()
{
	xmlfile = "test_float32";
    std_log(LOG_FILENAME_LINE, "Test Started test_float32");
  /* stereo to mono */
  {
    gfloat in[] = { 0.6, -0.0078125, 0.03125, 0.03125 };
    gfloat out[] = { 0.29609375, 0.03125 };

    RUN_CONVERSION ("float32 stereo to mono",
        in, get_float_caps (2, "BYTE_ORDER", 32),
        out, get_float_caps (1, "BYTE_ORDER", 32));
  }
  /* mono to stereo */
  {
    gfloat in[] = { 0.015625, 0.03125 };
    gfloat out[] = { 0.015625, 0.015625, 0.03125, 0.03125 };

    RUN_CONVERSION ("float32 mono to stereo",
        in, get_float_caps (1, "BYTE_ORDER", 32),
        out, get_float_caps (2, "BYTE_ORDER", 32));
  }
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}




void test_int_conversion()
{
	xmlfile = "test_int_conversion";
    std_log(LOG_FILENAME_LINE, "Test Started test_int_conversion");
  /* 8 <-> 16 signed */
  /* NOTE: if audioconvert was doing dithering we'd have a problem */
  {
    gint8 in[] = { 0, 1, 2, 127, -127 };
    gint16 out[] = { 0, 256, 512, 32512, -32512 };

    RUN_CONVERSION ("int 8bit to 16bit signed",
        in, get_int_caps (1, "BYTE_ORDER", 8, 8, TRUE),
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE)
        );
    RUN_CONVERSION ("int 16bit signed to 8bit",
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE),
        in, get_int_caps (1, "BYTE_ORDER", 8, 8, TRUE)
        );
  }
  /* 16 -> 8 signed */
  {
    gint16 in[] = { 0, 127, 128, 256, 256 + 127, 256 + 128 };
    gint8 out[] = { 0, 0, 1, 1, 1, 2 };

    RUN_CONVERSION ("16 bit to 8 signed",
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE),
        out, get_int_caps (1, "BYTE_ORDER", 8, 8, TRUE)
        );
  }
  /* 8 unsigned <-> 16 signed */
  /* NOTE: if audioconvert was doing dithering we'd have a problem */
  {
    guint8 in[] = { 128, 129, 130, 255, 1 };
    gint16 out[] = { 0, 256, 512, 32512, -32512 };
    GstCaps *incaps, *outcaps;

    /* exploded for easier valgrinding */
    incaps = get_int_caps (1, "BYTE_ORDER", 8, 8, FALSE);
    outcaps = get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE);
    GST_DEBUG ("incaps: %" GST_PTR_FORMAT, incaps);
    GST_DEBUG ("outcaps: %" GST_PTR_FORMAT, outcaps);
    RUN_CONVERSION ("8 unsigned to 16 signed", in, incaps, out, outcaps);
    RUN_CONVERSION ("16 signed to 8 unsigned", out, get_int_caps (1,
            "BYTE_ORDER", 16, 16, TRUE), in, get_int_caps (1, "BYTE_ORDER", 8,
            8, FALSE)
        );
  }
  /* 8 <-> 24 signed */
  /* NOTE: if audioconvert was doing dithering we'd have a problem */
  {
    gint8 in[] = { 0, 1, 127 };
    guint8 out[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x7f };
    /* out has the bytes in little-endian, so that's how they should be
     * interpreted during conversion */

    RUN_CONVERSION ("8 to 24 signed", in, get_int_caps (1, "BYTE_ORDER", 8, 8,
            TRUE), out, get_int_caps (1, "LITTLE_ENDIAN", 24, 24, TRUE)
        );
    RUN_CONVERSION ("24 signed to 8", out, get_int_caps (1, "LITTLE_ENDIAN", 24,
            24, TRUE), in, get_int_caps (1, "BYTE_ORDER", 8, 8, TRUE)
        );
  }

  /* 16 bit signed <-> unsigned */
  {
    gint16 in[] = { 0, 128, -128 };
    guint16 out[] = { 32768, 32896, 32640 };
    RUN_CONVERSION ("16 signed to 16 unsigned",
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE),
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, FALSE)
        );
    RUN_CONVERSION ("16 unsigned to 16 signed",
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, FALSE),
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE)
        );
  }

  /* 16 bit signed <-> 8 in 16 bit signed */
  /* NOTE: if audioconvert was doing dithering we'd have a problem */
  {
    gint16 in[] = { 0, 64 << 8, -64 << 8 };
    gint16 out[] = { 0, 64, -64 };
    RUN_CONVERSION ("16 signed to 8 in 16 signed",
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE),
        out, get_int_caps (1, "BYTE_ORDER", 16, 8, TRUE)
        );
    RUN_CONVERSION ("8 in 16 signed to 16 signed",
        out, get_int_caps (1, "BYTE_ORDER", 16, 8, TRUE),
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE)
        );
  }

  /* 16 bit unsigned <-> 8 in 16 bit unsigned */
  /* NOTE: if audioconvert was doing dithering we'd have a problem */
  {
    guint16 in[] = { 1 << 15, (1 << 15) - (64 << 8), (1 << 15) + (64 << 8) };
    guint16 out[] = { 1 << 7, (1 << 7) - 64, (1 << 7) + 64 };
    RUN_CONVERSION ("16 unsigned to 8 in 16 unsigned",
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, FALSE),
        out, get_int_caps (1, "BYTE_ORDER", 16, 8, FALSE)
        );
    RUN_CONVERSION ("8 in 16 unsigned to 16 unsigned",
        out, get_int_caps (1, "BYTE_ORDER", 16, 8, FALSE),
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, FALSE)
        );
  }

  /* 32 bit signed -> 16 bit signed for rounding check */
  /* NOTE: if audioconvert was doing dithering we'd have a problem */
  {
    gint32 in[] = { 0, G_MININT32, G_MAXINT32,
      (32 << 16), (32 << 16) + (1 << 15), (32 << 16) - (1 << 15),
      (32 << 16) + (2 << 15), (32 << 16) - (2 << 15),
      (-32 << 16) + (1 << 15), (-32 << 16) - (1 << 15),
      (-32 << 16) + (2 << 15), (-32 << 16) - (2 << 15),
      (-32 << 16)
    };
    gint16 out[] = { 0, G_MININT16, G_MAXINT16,
      32, 33, 32,
      33, 31,
      -31, -32,
      -31, -33,
      -32
    };
    RUN_CONVERSION ("32 signed to 16 signed for rounding",
        in, get_int_caps (1, "BYTE_ORDER", 32, 32, TRUE),
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE)
        );
  }

  /* 32 bit signed -> 16 bit unsigned for rounding check */
  /* NOTE: if audioconvert was doing dithering we'd have a problem */
  {
    gint32 in[] = { 0, G_MININT32, G_MAXINT32,
      (32 << 16), (32 << 16) + (1 << 15), (32 << 16) - (1 << 15),
      (32 << 16) + (2 << 15), (32 << 16) - (2 << 15),
      (-32 << 16) + (1 << 15), (-32 << 16) - (1 << 15),
      (-32 << 16) + (2 << 15), (-32 << 16) - (2 << 15),
      (-32 << 16)
    };
    guint16 out[] = { (1 << 15), 0, G_MAXUINT16,
      (1 << 15) + 32, (1 << 15) + 33, (1 << 15) + 32,
      (1 << 15) + 33, (1 << 15) + 31,
      (1 << 15) - 31, (1 << 15) - 32,
      (1 << 15) - 31, (1 << 15) - 33,
      (1 << 15) - 32
    };
    RUN_CONVERSION ("32 signed to 16 unsigned for rounding",
        in, get_int_caps (1, "BYTE_ORDER", 32, 32, TRUE),
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, FALSE)
        );
  }
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}

void create_array_float(gfloat in_be[], float temp[], int len)
    {
        int i;
        for(i=0; i<len; i++)
                in_be[i] = GFLOAT_TO_BE (temp[i]);
    }
void create_array_double(gdouble in_be[], float temp[], int len)
    {
        int i;
        for(i=0; i<len; i++)
                in_be[i] = GDOUBLE_TO_BE (temp[i]);
    }


void test_float_conversion()
{
    std_log(LOG_FILENAME_LINE, "Test Started test_float_conversion");
  /* 32 float <-> 16 signed */
  /* NOTE: if audioconvert was doing dithering we'd have a problem */
  {
    gfloat in_le[] =
        { GFLOAT_TO_LE (0.0), GFLOAT_TO_LE (1.0), GFLOAT_TO_LE (-1.0),
      GFLOAT_TO_LE (0.5), GFLOAT_TO_LE (-0.5), GFLOAT_TO_LE (1.1),
      GFLOAT_TO_LE (-1.1)
    };
    

    gfloat in_be[7];
    float temp[] = {0.0, 1.0, -1.0, 0.5, -0.5, 1.1, -1.1};  
    
    gint16 out[] = { 0, 32767, -32768, 16384, -16384, 32767, -32768 };
    
    create_array_float(in_be, temp, 7);

    /* only one direction conversion, the other direction does
     * not produce exactly the same as the input due to floating
     * point rounding errors etc. */
    RUN_CONVERSION ("32 float le to 16 signed",
        in_le, get_float_caps (1, "LITTLE_ENDIAN", 32),
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE));
    RUN_CONVERSION ("32 float be to 16 signed",
        in_be, get_float_caps (1, "BIG_ENDIAN", 32),
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE));
  }


  {
    gint16 in[] = { 0, -32768, 16384, -16384 };
    gfloat out[] = { 0.0, -1.0, 0.5, -0.5 };

    RUN_CONVERSION ("16 signed to 32 float",
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE),
        out, get_float_caps (1, "BYTE_ORDER", 32));
  }

  /* 64 float <-> 16 signed */
  /* NOTE: if audioconvert was doing dithering we'd have a problem */
  {
    gdouble in_le[] =
        { GDOUBLE_TO_LE (0.0), GDOUBLE_TO_LE (1.0), GDOUBLE_TO_LE (-1.0),
      GDOUBLE_TO_LE (0.5), GDOUBLE_TO_LE (-0.5), GDOUBLE_TO_LE (1.1),
      GDOUBLE_TO_LE (-1.1)
    };
   
    gdouble in_be[7];
    float temp[] = {0.0, 1.0, -1.0, 0.5, -0.5, 1.1, -1.1}; 
        
    gint16 out[] = { 0, 32767, -32768, 16384, -16384, 32767, -32768 };
    create_array_double(in_be, temp, 7);

    /* only one direction conversion, the other direction does
     * not produce exactly the same as the input due to floating
     * point rounding errors etc. */
    RUN_CONVERSION ("64 float LE to 16 signed",
        in_le, get_float_caps (1, "LITTLE_ENDIAN", 64),
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE));
    RUN_CONVERSION ("64 float BE to 16 signed",
        in_be, get_float_caps (1, "BIG_ENDIAN", 64),
        out, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE));
  }
  {
    gint16 in[] = { 0, -32768, 16384, -16384 };
    gdouble out[] = { 0.0,
      (gdouble) (-32768L << 16) / 2147483647.0, /* ~ -1.0 */
      (gdouble) (16384L << 16) / 2147483647.0,  /* ~  0.5 */
      (gdouble) (-16384L << 16) / 2147483647.0, /* ~ -0.5 */
    };

    RUN_CONVERSION ("16 signed to 64 float",
        in, get_int_caps (1, "BYTE_ORDER", 16, 16, TRUE),
        out, get_float_caps (1, "BYTE_ORDER", 64));
  }
  {
    gint32 in[] = { 0, (-1L << 31), (1L << 30), (-1L << 30) };
    gdouble out[] = { 0.0,
      (gdouble) (-1L << 31) / 2147483647.0,     /* ~ -1.0 */
      (gdouble) (1L << 30) / 2147483647.0,      /* ~  0.5 */
      (gdouble) (-1L << 30) / 2147483647.0,     /* ~ -0.5 */
    };

    RUN_CONVERSION ("32 signed to 64 float",
        in, get_int_caps (1, "BYTE_ORDER", 32, 32, TRUE),
        out, get_float_caps (1, "BYTE_ORDER", 64));
  }

  /* 64-bit float <-> 32-bit float */
  {
    gdouble in[] = { 0.0, 1.0, -1.0, 0.5, -0.5 };
    gfloat out[] = { 0.0, 1.0, -1.0, 0.5, -0.5 };
    
    RUN_CONVERSION ("64 float to 32 float",
        in, get_float_caps (1, "BYTE_ORDER", 64),
        out, get_float_caps (1, "BYTE_ORDER", 32));

    RUN_CONVERSION ("32 float to 64 float",
        out, get_float_caps (1, "BYTE_ORDER", 32),
        in, get_float_caps (1, "BYTE_ORDER", 64));
  }

  /* 32-bit float little endian <-> big endian */
  {
    gfloat le[] = { GFLOAT_TO_LE (0.0), GFLOAT_TO_LE (1.0), GFLOAT_TO_LE (-1.0),
      GFLOAT_TO_LE (0.5), GFLOAT_TO_LE (-0.5)
    };
  
    gfloat be[5];
    float temp[] = {0.0, 1.0, -1.0, 0.5, -0.5};
           
    create_array_float(be, temp, 5);

    RUN_CONVERSION ("32 float LE to BE",
        le, get_float_caps (1, "LITTLE_ENDIAN", 32),
        be, get_float_caps (1, "BIG_ENDIAN", 32));

    RUN_CONVERSION ("32 float BE to LE",
        be, get_float_caps (1, "BIG_ENDIAN", 32),
        le, get_float_caps (1, "LITTLE_ENDIAN", 32));
  }

  /* 64-bit float little endian <-> big endian */
  {
    gdouble le[] =
        { GDOUBLE_TO_LE (0.0), GDOUBLE_TO_LE (1.0), GDOUBLE_TO_LE (-1.0),
      GDOUBLE_TO_LE (0.5), GDOUBLE_TO_LE (-0.5)
    };

    gdouble be[5];
    float temp[] = {0.0, 1.0, -1.0, 0.5, -0.5}; 
        
    create_array_double(be, temp, 5);

    RUN_CONVERSION ("64 float LE to BE",
        le, get_float_caps (1, "LITTLE_ENDIAN", 64),
        be, get_float_caps (1, "BIG_ENDIAN", 64));

    RUN_CONVERSION ("64 float BE to LE",
        be, get_float_caps (1, "BIG_ENDIAN", 64),
        le, get_float_caps (1, "LITTLE_ENDIAN", 64));
  }

  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}


void test_multichannel_conversion()
{
	xmlfile = "test_multichannel_conversion";
    std_log(LOG_FILENAME_LINE, "Test Started test_multichannel_conversion");
  {
    /* Ensure that audioconvert prefers to convert to integer, rather than mix
     * to mono
     */
    gfloat in[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    gfloat out[] = { 0.0, 0.0 };

    /* only one direction conversion, the other direction does
     * not produce exactly the same as the input due to floating
     * point rounding errors etc. */
    RUN_CONVERSION ("3 channels to 1", in, get_float_mc_caps (3,
            "BYTE_ORDER", 32, FALSE), out, get_float_caps (1, "BYTE_ORDER",
            32));
  }
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}




void test_channel_remapping()
{
	xmlfile = "test_channel_remapping";
    std_log(LOG_FILENAME_LINE, "Test Started test_channel_remapping");
  /* float */
  {
    gfloat in[] = { 0.0, 1.0, -0.5 };
    gfloat out[] = { -0.5, 1.0, 0.0 };
    GstCaps *in_caps = get_float_mc_caps (3, "BYTE_ORDER", 32, FALSE);
    GstCaps *out_caps = get_float_mc_caps (3, "BYTE_ORDER", 32, TRUE);

    RUN_CONVERSION ("3 channels layout remapping float", in, in_caps,
        out, out_caps);
  }

  /* int */
  {
    guint16 in[] = { 0, 65535, 0x9999 };
    guint16 out[] = { 0x9999, 65535, 0 };
    GstCaps *in_caps = get_int_mc_caps (3, "BYTE_ORDER", 16, 16, FALSE, FALSE);
    GstCaps *out_caps = get_int_mc_caps (3, "BYTE_ORDER", 16, 16, FALSE, TRUE);

    RUN_CONVERSION ("3 channels layout remapping int", in, in_caps,
        out, out_caps);
  }

  /* TODO: float => int conversion with remapping and vice versa,
   *       int   => int conversion with remapping */
   std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}



void test_caps_negotiation()
{
  GstElement *src, *ac1, *ac2, *ac3, *sink;
  GstElement *pipeline;
  GstPad *ac3_src;
  GstCaps *caps1, *caps2;
xmlfile = "test_caps_negotiation";
    std_log(LOG_FILENAME_LINE, "Test Started test_caps_negotiation");
  pipeline = gst_pipeline_new ("test");

  /* create elements */
  src = gst_element_factory_make ("audiotestsrc", "src");
  ac1 = gst_element_factory_make ("audioconvert", "ac1");
  ac2 = gst_element_factory_make ("audioconvert", "ac2");
  ac3 = gst_element_factory_make ("audioconvert", "ac3");
  sink = gst_element_factory_make ("fakesink", "sink");
  ac3_src = gst_element_get_pad (ac3, "src");

  /* test with 2 audioconvert elements */
  gst_bin_add_many (GST_BIN (pipeline), src, ac1, ac3, sink, NULL);
  gst_element_link_many (src, ac1, ac3, sink, NULL);

  /* Set to PAUSED and wait for PREROLL */
  fail_if (gst_element_set_state (pipeline, GST_STATE_PAUSED) ==
      GST_STATE_CHANGE_FAILURE, "Failed to set test pipeline to PAUSED");
  fail_if (gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE) !=
      GST_STATE_CHANGE_SUCCESS, "Failed to set test pipeline to PAUSED");

  caps1 = gst_pad_get_caps (ac3_src);
  fail_if (caps1 == NULL, "gst_pad_get_caps returned NULL");
  GST_DEBUG ("Caps size 1 : %d", gst_caps_get_size (caps1));

  fail_if (gst_element_set_state (pipeline, GST_STATE_READY) ==
      GST_STATE_CHANGE_FAILURE, "Failed to set test pipeline back to READY");
  fail_if (gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE) !=
      GST_STATE_CHANGE_SUCCESS, "Failed to set test pipeline back to READY");

  /* test with 3 audioconvert elements */
  gst_element_unlink (ac1, ac3);
  gst_bin_add (GST_BIN (pipeline), ac2);
  gst_element_link_many (ac1, ac2, ac3, NULL);

  fail_if (gst_element_set_state (pipeline, GST_STATE_PAUSED) ==
      GST_STATE_CHANGE_FAILURE, "Failed to set test pipeline back to PAUSED");
  fail_if (gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE) !=
      GST_STATE_CHANGE_SUCCESS, "Failed to set test pipeline back to PAUSED");

  caps2 = gst_pad_get_caps (ac3_src);

  fail_if (caps2 == NULL, "gst_pad_get_caps returned NULL");
  GST_DEBUG ("Caps size 2 : %d", gst_caps_get_size (caps2));
  fail_unless (gst_caps_get_size (caps1) == gst_caps_get_size (caps2));

  gst_caps_unref (caps1);
  gst_caps_unref (caps2);

  fail_if (gst_element_set_state (pipeline, GST_STATE_NULL) ==
      GST_STATE_CHANGE_FAILURE, "Failed to set test pipeline back to NULL");
  fail_if (gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE) !=
      GST_STATE_CHANGE_SUCCESS, "Failed to set test pipeline back to NULL");

  gst_object_unref (ac3_src);
  gst_object_unref (pipeline);
  std_log(LOG_FILENAME_LINE, "Test Successful");
    create_xml(0);
}




void (*fn[]) (void) = {
test_int16,
test_float32,
test_int_conversion,
test_float_conversion,
test_multichannel_conversion,
test_channel_remapping,
test_caps_negotiation
};

char *args[] = {
"test_int16",
"test_float32",
"test_int_conversion",
"test_float_conversion",
"test_multichannel_conversion",
"test_channel_remapping",
"test_caps_negotiation"
};

GST_CHECK_MAIN(audioconvert)


