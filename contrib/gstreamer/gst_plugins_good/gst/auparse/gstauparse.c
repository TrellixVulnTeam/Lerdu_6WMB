/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2006> Tim-Philipp Müller <tim centricular net>
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

/**
 * SECTION:element-auparse
 * @short_description: .au file parser
 *
 * <refsect2>
 * <para>
 * Parses .au files.
 * </para>
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "gstauparse.h"
#include <gst/audio/audio.h>

GST_DEBUG_CATEGORY_STATIC (auparse_debug);
#define GST_CAT_DEFAULT (auparse_debug)

static const GstElementDetails gst_au_parse_details =
GST_ELEMENT_DETAILS ("AU audio demuxer",
    "Codec/Demuxer/Audio",
    "Parse an .au file into raw audio",
    "Erik Walthinsen <omega@cse.ogi.edu>");
static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-au")
    );

#define GST_AU_PARSE_ALAW_PAD_TEMPLATE_CAPS \
    "audio/x-alaw, "                        \
    "rate = (int) [ 8000, 192000 ], "       \
    "channels = (int) [ 1, 2 ]"

#define GST_AU_PARSE_MULAW_PAD_TEMPLATE_CAPS \
    "audio/x-mulaw, "                        \
    "rate = (int) [ 8000, 192000 ], "        \
    "channels = (int) [ 1, 2 ]"

/* Nothing to decode those ADPCM streams for now */
#define GST_AU_PARSE_ADPCM_PAD_TEMPLATE_CAPS \
    "audio/x-adpcm, "                        \
    "layout = (string) { g721, g722, g723_3, g723_5 }"

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_SOMETIMES,
    GST_STATIC_CAPS (GST_AUDIO_INT_PAD_TEMPLATE_CAPS "; "
        GST_AUDIO_FLOAT_PAD_TEMPLATE_CAPS ";"
        GST_AU_PARSE_ALAW_PAD_TEMPLATE_CAPS ";"
        GST_AU_PARSE_MULAW_PAD_TEMPLATE_CAPS ";"
        GST_AU_PARSE_ADPCM_PAD_TEMPLATE_CAPS));


static void gst_au_parse_dispose (GObject * object);
static GstFlowReturn gst_au_parse_chain (GstPad * pad, GstBuffer * buf);
static GstStateChangeReturn gst_au_parse_change_state (GstElement * element,
    GstStateChange transition);
static void gst_au_parse_reset (GstAuParse * auparse);
static gboolean gst_au_parse_remove_srcpad (GstAuParse * auparse);
static gboolean gst_au_parse_add_srcpad (GstAuParse * auparse, GstCaps * caps);
static gboolean gst_au_parse_src_query (GstPad * pad, GstQuery * query);
static gboolean gst_au_parse_src_event (GstPad * pad, GstEvent * event);
static gboolean gst_au_parse_sink_event (GstPad * pad, GstEvent * event);

GST_BOILERPLATE (GstAuParse, gst_au_parse, GstElement, GST_TYPE_ELEMENT);

static void
gst_au_parse_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&sink_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_template));
  gst_element_class_set_details (element_class, &gst_au_parse_details);

  GST_DEBUG_CATEGORY_INIT (auparse_debug, "auparse", 0, ".au parser");
}

static void
gst_au_parse_class_init (GstAuParseClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->dispose = gst_au_parse_dispose;

  gstelement_class->change_state =
      GST_DEBUG_FUNCPTR (gst_au_parse_change_state);
}

static void
gst_au_parse_init (GstAuParse * auparse, GstAuParseClass * klass)
{
  auparse->sinkpad = gst_pad_new_from_static_template (&sink_template, "sink");
  gst_pad_set_chain_function (auparse->sinkpad,
      GST_DEBUG_FUNCPTR (gst_au_parse_chain));
  gst_pad_set_event_function (auparse->sinkpad,
      GST_DEBUG_FUNCPTR (gst_au_parse_sink_event));
  gst_element_add_pad (GST_ELEMENT (auparse), auparse->sinkpad);

  auparse->srcpad = NULL;
  auparse->adapter = gst_adapter_new ();
  gst_au_parse_reset (auparse);
}

static void
gst_au_parse_dispose (GObject * object)
{
  GstAuParse *au = GST_AU_PARSE (object);

  if (au->adapter != NULL) {
    g_object_unref (au->adapter);
    au->adapter = NULL;
  }
  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
gst_au_parse_reset (GstAuParse * auparse)
{
  gst_au_parse_remove_srcpad (auparse);

  auparse->offset = 0;
  auparse->buffer_offset = 0;
  auparse->encoding = 0;
  auparse->samplerate = 0;
  auparse->channels = 0;

  gst_adapter_clear (auparse->adapter);

  /* gst_segment_init (&auparse->segment, GST_FORMAT_TIME); */
}

static gboolean
gst_au_parse_add_srcpad (GstAuParse * auparse, GstCaps * new_caps)
{
  GstPad *srcpad = NULL;

  if (auparse->src_caps && gst_caps_is_equal (new_caps, auparse->src_caps)) {
    GST_LOG_OBJECT (auparse, "same caps, nothing to do");
    return TRUE;
  }

  gst_caps_replace (&auparse->src_caps, new_caps);
  if (auparse->srcpad != NULL) {
    GST_DEBUG_OBJECT (auparse, "Changing src pad caps to %" GST_PTR_FORMAT,
        auparse->src_caps);
    gst_pad_set_caps (auparse->srcpad, auparse->src_caps);
  }

  if (auparse->srcpad == NULL) {
    srcpad = auparse->srcpad =
        gst_pad_new_from_static_template (&src_template, "src");
    g_return_val_if_fail (auparse->srcpad != NULL, FALSE);

#if 0
    gst_pad_set_query_type_function (auparse->srcpad,
        GST_DEBUG_FUNCPTR (gst_au_parse_src_get_query_types));
#endif
    gst_pad_set_query_function (auparse->srcpad,
        GST_DEBUG_FUNCPTR (gst_au_parse_src_query));
    gst_pad_set_event_function (auparse->srcpad,
        GST_DEBUG_FUNCPTR (gst_au_parse_src_event));

    gst_pad_use_fixed_caps (auparse->srcpad);
    gst_pad_set_active (auparse->srcpad, TRUE);

    if (auparse->src_caps)
      gst_pad_set_caps (auparse->srcpad, auparse->src_caps);

    GST_DEBUG_OBJECT (auparse, "Adding src pad with caps %" GST_PTR_FORMAT,
        auparse->src_caps);

    gst_object_ref (auparse->srcpad);
    if (!gst_element_add_pad (GST_ELEMENT (auparse), auparse->srcpad))
      return FALSE;
    gst_element_no_more_pads (GST_ELEMENT (auparse));
  }

  return TRUE;
}

static gboolean
gst_au_parse_remove_srcpad (GstAuParse * auparse)
{
  gboolean res = TRUE;

  if (auparse->srcpad != NULL) {
    GST_DEBUG_OBJECT (auparse, "Removing src pad");
    res = gst_element_remove_pad (GST_ELEMENT (auparse), auparse->srcpad);
    g_return_val_if_fail (res != FALSE, FALSE);
    gst_object_unref (auparse->srcpad);
    auparse->srcpad = NULL;
  }

  return res;
}

static GstFlowReturn
gst_au_parse_parse_header (GstAuParse * auparse)
{
  GstCaps *tempcaps;
  guint32 size;
  guint8 *head;
  gchar layout[7] = { 0, };
  gint law = 0, depth = 0, ieee = 0;

  head = (guint8 *) gst_adapter_peek (auparse->adapter, 24);
  g_assert (head != NULL);

  GST_DEBUG_OBJECT (auparse, "[%c%c%c%c]", head[0], head[1], head[2], head[3]);

  switch (GST_READ_UINT32_BE (head)) {
      /* normal format is big endian (au is a Sparc format) */
    case 0x2e736e64:{          /* ".snd" */
      auparse->endianness = G_BIG_ENDIAN;
      break;
    }
      /* and of course, someone had to invent a little endian
       * version.  Used by DEC systems. */
    case 0x646e732e:           /* dns.                          */
    case 0x0064732e:{          /* other source say it is "dns." */
      auparse->endianness = G_LITTLE_ENDIAN;
      break;
    }
    default:{
      goto unknown_header;
    }
  }

  auparse->offset = GST_READ_UINT32_BE (head + 4);
  /* Do not trust size, could be set to -1 : unknown */
  size = GST_READ_UINT32_BE (head + 8);
  auparse->encoding = GST_READ_UINT32_BE (head + 12);
  auparse->samplerate = GST_READ_UINT32_BE (head + 16);
  auparse->channels = GST_READ_UINT32_BE (head + 20);

  if (auparse->samplerate < 8000 || auparse->samplerate > 192000)
    goto unsupported_sample_rate;

  if (auparse->channels < 1 || auparse->channels > 2)
    goto unsupported_number_of_channels;

  GST_DEBUG_OBJECT (auparse, "offset %" G_GINT64_FORMAT ", size %u, "
      "encoding %u, frequency %u, channels %u", auparse->offset, size,
      auparse->encoding, auparse->samplerate, auparse->channels);

  /* Docs:
   * http://www.opengroup.org/public/pubs/external/auformat.html
   * http://astronomy.swin.edu.au/~pbourke/dataformats/au/
   * Solaris headers : /usr/include/audio/au.h
   * libsndfile : src/au.c
   *
   * Samples :
   * http://www.tsp.ece.mcgill.ca/MMSP/Documents/AudioFormats/AU/Samples.html
   */

  switch (auparse->encoding) {
    case 1:                    /* 8-bit ISDN mu-law G.711 */
      law = 1;
      depth = 8;
      break;
    case 27:                   /* 8-bit ISDN  A-law G.711 */
      law = 2;
      depth = 8;
      break;

    case 2:                    /*  8-bit linear PCM */
      depth = 8;
      break;
    case 3:                    /* 16-bit linear PCM */
      depth = 16;
      break;
    case 4:                    /* 24-bit linear PCM */
      depth = 24;
      break;
    case 5:                    /* 32-bit linear PCM */
      depth = 32;
      break;

    case 6:                    /* 32-bit IEEE floating point */
      ieee = 1;
      depth = 32;
      break;
    case 7:                    /* 64-bit IEEE floating point */
      ieee = 1;
      depth = 64;
      break;

    case 23:                   /* 4-bit CCITT G.721   ADPCM 32kbps -> modplug/libsndfile (compressed 8-bit mu-law) */
      strcpy (layout, "g721");
      break;
    case 24:                   /* 8-bit CCITT G.722   ADPCM        -> rtp */
      strcpy (layout, "g722");
      break;
    case 25:                   /* 3-bit CCITT G.723.3 ADPCM 24kbps -> rtp/xine/modplug/libsndfile */
      strcpy (layout, "g723_3");
      break;
    case 26:                   /* 5-bit CCITT G.723.5 ADPCM 40kbps -> rtp/xine/modplug/libsndfile */
      strcpy (layout, "g723_5");
      break;

    case 8:                    /* Fragmented sample data */
    case 9:                    /* AU_ENCODING_NESTED */

    case 10:                   /* DSP program */
    case 11:                   /* DSP  8-bit fixed point */
    case 12:                   /* DSP 16-bit fixed point */
    case 13:                   /* DSP 24-bit fixed point */
    case 14:                   /* DSP 32-bit fixed point */

    case 16:                   /* AU_ENCODING_DISPLAY : non-audio display data */
    case 17:                   /* AU_ENCODING_MULAW_SQUELCH */

    case 18:                   /* 16-bit linear with emphasis */
    case 19:                   /* 16-bit linear compressed (NeXT) */
    case 20:                   /* 16-bit linear with emphasis and compression */

    case 21:                   /* Music kit DSP commands */
    case 22:                   /* Music kit DSP commands samples */

    default:
      goto unknown_format;
  }

  if (law) {
    tempcaps =
        gst_caps_new_simple ((law == 1) ? "audio/x-mulaw" : "audio/x-alaw",
        "rate", G_TYPE_INT, auparse->samplerate,
        "channels", G_TYPE_INT, auparse->channels, NULL);
    auparse->sample_size = auparse->channels;
  } else if (ieee) {
    tempcaps = gst_caps_new_simple ("audio/x-raw-float",
        "rate", G_TYPE_INT, auparse->samplerate,
        "channels", G_TYPE_INT, auparse->channels,
        "endianness", G_TYPE_INT, auparse->endianness,
        "width", G_TYPE_INT, depth, NULL);
    auparse->sample_size = auparse->channels * depth / 8;
  } else if (layout[0]) {
    tempcaps = gst_caps_new_simple ("audio/x-adpcm",
        "layout", G_TYPE_STRING, layout, NULL);
    auparse->sample_size = 0;
  } else {
    tempcaps = gst_caps_new_simple ("audio/x-raw-int",
        "rate", G_TYPE_INT, auparse->samplerate,
        "channels", G_TYPE_INT, auparse->channels,
        "endianness", G_TYPE_INT, auparse->endianness,
        "depth", G_TYPE_INT, depth, "width", G_TYPE_INT, depth,
        /* FIXME: signed TRUE even for 8-bit PCM? */
        "signed", G_TYPE_BOOLEAN, TRUE, NULL);
    auparse->sample_size = auparse->channels * depth / 8;
  }

  GST_DEBUG_OBJECT (auparse, "sample_size=%d", auparse->sample_size);

  if (!gst_au_parse_add_srcpad (auparse, tempcaps))
    goto add_pad_failed;

  GST_DEBUG_OBJECT (auparse, "offset=%" G_GINT64_FORMAT, auparse->offset);
  gst_adapter_flush (auparse->adapter, auparse->offset);

  gst_caps_unref (tempcaps);
  return GST_FLOW_OK;

  /* ERRORS */
unknown_header:
  {
    GST_ELEMENT_ERROR (auparse, STREAM, WRONG_TYPE, (NULL), (NULL));
    return GST_FLOW_ERROR;
  }
unsupported_sample_rate:
  {
    GST_ELEMENT_ERROR (auparse, STREAM, FORMAT, (NULL),
        ("Unsupported samplerate: %u", auparse->samplerate));
    return GST_FLOW_ERROR;
  }
unsupported_number_of_channels:
  {
    GST_ELEMENT_ERROR (auparse, STREAM, FORMAT, (NULL),
        ("Unsupported number of channels: %u", auparse->channels));
    return GST_FLOW_ERROR;
  }
unknown_format:
  {
    GST_ELEMENT_ERROR (auparse, STREAM, FORMAT, (NULL),
        ("Unsupported encoding: %u", auparse->encoding));
    return GST_FLOW_ERROR;
  }
add_pad_failed:
  {
    GST_ELEMENT_ERROR (auparse, STREAM, FAILED, (NULL),
        ("Failed to add srcpad"));
    gst_caps_unref (tempcaps);
    return GST_FLOW_ERROR;
  }
}

#define AU_HEADER_SIZE 24

static GstFlowReturn
gst_au_parse_chain (GstPad * pad, GstBuffer * buf)
{
  GstFlowReturn ret = GST_FLOW_OK;
  GstAuParse *auparse;
  gint avail, sendnow = 0;

  auparse = GST_AU_PARSE (gst_pad_get_parent (pad));

  GST_LOG_OBJECT (auparse, "got buffer of size %u", GST_BUFFER_SIZE (buf));

  gst_adapter_push (auparse->adapter, buf);
  buf = NULL;

  /* if we haven't seen any data yet... */
  if (auparse->srcpad == NULL) {
    if (gst_adapter_available (auparse->adapter) < AU_HEADER_SIZE) {
      GST_DEBUG_OBJECT (auparse, "need more data to parse header");
      ret = GST_FLOW_OK;
      goto out;
    }

    ret = gst_au_parse_parse_header (auparse);
    if (ret != GST_FLOW_OK)
      goto out;

    gst_pad_push_event (auparse->srcpad,
        gst_event_new_new_segment (FALSE, 1.0, GST_FORMAT_DEFAULT,
            0, GST_CLOCK_TIME_NONE, 0));
  }

  avail = gst_adapter_available (auparse->adapter);

  if (auparse->sample_size > 0) {
    /* Ensure we push a buffer that's a multiple of the frame size downstream */
    sendnow = avail - (avail % auparse->sample_size);
  } else {
    /* It's something non-trivial (such as ADPCM), we don't understand it, so
     * just push downstream and assume it will know what to do with it */
    sendnow = avail;
  }

  if (sendnow > 0) {
    GstBuffer *outbuf;
    const guint8 *data;

    ret = gst_pad_alloc_buffer_and_set_caps (auparse->srcpad,
        auparse->buffer_offset, sendnow, GST_PAD_CAPS (auparse->srcpad),
        &outbuf);

    if (ret != GST_FLOW_OK) {
      GST_DEBUG_OBJECT (auparse, "pad alloc flow: %s", gst_flow_get_name (ret));
      goto out;
    }

    data = gst_adapter_peek (auparse->adapter, sendnow);
    memcpy (GST_BUFFER_DATA (outbuf), data, sendnow);
    gst_adapter_flush (auparse->adapter, sendnow);

    auparse->buffer_offset += sendnow;

    ret = gst_pad_push (auparse->srcpad, outbuf);
  }

out:

  gst_object_unref (auparse);
  return ret;
}

static gboolean
gst_au_parse_src_convert (GstAuParse * auparse, GstFormat src_format,
    gint64 srcval, GstFormat dest_format, gint64 * destval)
{
  gboolean ret = TRUE;
  gint64 offset;
  guint samplesize, rate;

  if (dest_format == src_format) {
    *destval = srcval;
    return TRUE;
  }

  GST_OBJECT_LOCK (auparse);
  samplesize = auparse->sample_size;
  offset = auparse->offset;
  rate = auparse->samplerate;
  GST_OBJECT_UNLOCK (auparse);

  if (samplesize == 0 || rate == 0) {
    GST_LOG_OBJECT (auparse, "cannot convert, sample_size or rate unknown");
    return FALSE;
  }

  switch (src_format) {
    case GST_FORMAT_BYTES:
      srcval /= samplesize;
      /* fallthrough */
    case GST_FORMAT_DEFAULT:{
      switch (dest_format) {
        case GST_FORMAT_BYTES:
          *destval = srcval * samplesize;
          break;
        case GST_FORMAT_TIME:
          *destval = gst_util_uint64_scale_int (srcval, GST_SECOND, rate);
          break;
        default:
          ret = FALSE;
          break;
      }
      break;
    }
    case GST_FORMAT_TIME:{
      switch (dest_format) {
        case GST_FORMAT_BYTES:
          *destval =
              gst_util_uint64_scale_int (srcval, rate * samplesize, GST_SECOND);
          break;
        case GST_FORMAT_DEFAULT:
          *destval = gst_util_uint64_scale_int (srcval, rate, GST_SECOND);
          break;
        default:
          ret = FALSE;
          break;
      }
      break;
    }
    default:{
      ret = FALSE;
      break;
    }
  }

  if (!ret) {
    GST_DEBUG_OBJECT (auparse, "could not convert from %s to %s format",
        gst_format_get_name (src_format), gst_format_get_name (dest_format));
  }

  return ret;
}

static gboolean
gst_au_parse_src_query (GstPad * pad, GstQuery * query)
{
  GstAuParse *auparse;
  gboolean ret = FALSE;

  auparse = GST_AU_PARSE (gst_pad_get_parent (pad));

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_DURATION:{
      GstFormat bformat = GST_FORMAT_BYTES;
      GstFormat format;
      gint64 len, val;

      gst_query_parse_duration (query, &format, NULL);
      if (!gst_pad_query_peer_duration (auparse->sinkpad, &bformat, &len)) {
        GST_DEBUG_OBJECT (auparse, "failed to query upstream length");
        break;
      }
      GST_OBJECT_LOCK (auparse);
      len -= auparse->offset;
      GST_OBJECT_UNLOCK (auparse);

      ret = gst_au_parse_src_convert (auparse, GST_FORMAT_BYTES, len,
          format, &val);

      if (ret) {
        gst_query_set_duration (query, format, val);
      }
      break;
    }
    case GST_QUERY_POSITION:{
      GstFormat bformat = GST_FORMAT_BYTES;
      GstFormat format;
      gint64 pos, val;

      gst_query_parse_position (query, &format, NULL);
      if (!gst_pad_query_peer_position (auparse->sinkpad, &bformat, &pos)) {
        GST_DEBUG_OBJECT (auparse, "failed to query upstream position");
        break;
      }
      GST_OBJECT_LOCK (auparse);
      pos -= auparse->offset;
      GST_OBJECT_UNLOCK (auparse);

      ret = gst_au_parse_src_convert (auparse, GST_FORMAT_BYTES, pos,
          format, &val);

      if (ret) {
        gst_query_set_position (query, format, val);
      }
      break;
    }
    default:
      ret = gst_pad_query_default (pad, query);
      break;
  }

  gst_object_unref (auparse);
  return ret;
}

static gboolean
gst_au_parse_handle_seek (GstAuParse * auparse, GstEvent * event)
{
  GstSeekType start_type, stop_type;
  GstSeekFlags flags;
  GstFormat format;
  gdouble rate;
  gint64 start, stop;

  gst_event_parse_seek (event, &rate, &format, &flags, &start_type, &start,
      &stop_type, &stop);

  if (format != GST_FORMAT_TIME) {
    GST_DEBUG_OBJECT (auparse, "only support seeks in TIME format");
    return FALSE;
  }

  /* FIXME: implement seeking */
  return FALSE;
}

static gboolean
gst_au_parse_sink_event (GstPad * pad, GstEvent * event)
{
  GstAuParse *auparse;
  gboolean ret;

  auparse = GST_AU_PARSE (gst_pad_get_parent (pad));

  switch (GST_EVENT_TYPE (event)) {
    default:
      ret = gst_pad_event_default (pad, event);
      break;
  }

  gst_object_unref (auparse);
  return ret;
}

static gboolean
gst_au_parse_src_event (GstPad * pad, GstEvent * event)
{
  GstAuParse *auparse;
  gboolean ret;

  auparse = GST_AU_PARSE (gst_pad_get_parent (pad));

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEEK:
      ret = gst_au_parse_handle_seek (auparse, event);
      break;
    default:
      ret = gst_pad_event_default (pad, event);
      break;
  }

  gst_object_unref (auparse);
  return ret;
}

static GstStateChangeReturn
gst_au_parse_change_state (GstElement * element, GstStateChange transition)
{
  GstAuParse *auparse = GST_AU_PARSE (element);
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

  ret = parent_class->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      gst_au_parse_reset (auparse);
    default:
      break;
  }

  return ret;
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_element_register (plugin, "auparse", GST_RANK_SECONDARY,
          GST_TYPE_AU_PARSE)) {
    return FALSE;
  }

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "auparse",
    "parses au streams", plugin_init, VERSION, "LGPL", GST_PACKAGE_NAME,
    GST_PACKAGE_ORIGIN)

EXPORT_C GstPluginDesc* _GST_PLUGIN_DESC()
{
	return &gst_plugin_desc;
}
