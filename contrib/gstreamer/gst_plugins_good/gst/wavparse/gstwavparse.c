/* -*- Mode: C; tab-width: 2; indent-tabs-mode: t; c-basic-offset: 2 -*- */
/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2006> Nokia Corporation, Stefan Kost <stefan.kost@nokia.com>.
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
 * SECTION:element-wavparse
 *
 * <refsect2>
 * <para>
 * Parse a .wav file into raw or compressed audio.
 * </para>
 * <para>
 * This element currently only supports pull based scheduling.
 * </para>
 * <title>Example launch line</title>
 * <para>
 * <programlisting>
 * gst-launch filesrc location=sine.wav ! wavparse ! audioconvert ! alsasink
 * </programlisting>
 * Read a wav file and output to the soundcard using the ALSA element. The
 * wav file is assumed to contain raw uncompressed samples.
 * </para>
 * <para>
 * <programlisting>
 * gst-launch gnomevfssrc location=http://www.example.org/sine.wav ! queue ! wavparse ! audioconvert ! alsasink
 * </programlisting>
 * Stream data from
 * </para>
 * </refsect2>
 *
 * Last reviewed on 2006-03-03 (0.10.3)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "string.h"
#include "gstwavparse.h"
#include <gst/riff/riff-ids.h>
#include <gst/riff/riff-media.h>
#include <gst/riff/riff-read.h>

#ifndef __SYMBIAN32__
#include <gst/gst-i18n-plugin.h>
#else
#include "gst/gst-i18n-plugin.h"
#endif

#ifdef __SYMBIAN32__
#include <gst/gstinfo.h>
#endif

#ifndef G_MAXUINT32
#define G_MAXUINT32 0xffffffff
#endif

GST_DEBUG_CATEGORY_STATIC (wavparse_debug);
#define GST_CAT_DEFAULT (wavparse_debug)

static void gst_wavparse_base_init (gpointer g_class);
static void gst_wavparse_class_init (GstWavParseClass * klass);
static void gst_wavparse_init (GstWavParse * wavparse);
static void gst_wavparse_dispose (GObject * object);

static gboolean gst_wavparse_sink_activate (GstPad * sinkpad);
static gboolean gst_wavparse_sink_activate_pull (GstPad * sinkpad,
    gboolean active);
static gboolean gst_wavparse_send_event (GstElement * element,
    GstEvent * event);
static GstFlowReturn gst_wavparse_chain (GstPad * pad, GstBuffer * buf);
static GstStateChangeReturn gst_wavparse_change_state (GstElement * element,
    GstStateChange transition);

static gboolean gst_wavparse_pad_query (GstPad * pad, GstQuery * query);
static const GstQueryType *gst_wavparse_get_query_types (GstPad * pad);
static gboolean gst_wavparse_pad_convert (GstPad * pad,
    GstFormat src_format,
    gint64 src_value, GstFormat * dest_format, gint64 * dest_value);

static void gst_wavparse_loop (GstPad * pad);
static gboolean gst_wavparse_srcpad_event (GstPad * pad, GstEvent * event);
static void gst_wavparse_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static const GstElementDetails gst_wavparse_details =
GST_ELEMENT_DETAILS ("WAV audio demuxer",
    "Codec/Demuxer/Audio",
    "Parse a .wav file into raw audio",
    "Erik Walthinsen <omega@cse.ogi.edu>");

static GstStaticPadTemplate sink_template_factory =
//GST_STATIC_PAD_TEMPLATE ("wavparse_sink",
GST_STATIC_PAD_TEMPLATE ("sink", 
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-wav")
    );

/* the pad is marked a sometimes and is added to the element when the
 * exact type is known. This makes it much easier for a static autoplugger
 * to connect the right decoder when needed.
 */
static GstStaticPadTemplate src_template_factory =
 //   GST_STATIC_PAD_TEMPLATE ("wavparse_src",
    GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_SOMETIMES,
    GST_STATIC_CAPS ("audio/x-raw-int, "
        "endianness = (int) little_endian, "
        "signed = (boolean) { true, false }, "
        "width = (int) { 8, 16, 24, 32 }, "
        "depth = (int) { 8, 16, 24, 32 }, "
        "rate = (int) [ 8000, 96000 ], "
        "channels = (int) [ 1, 8 ]; "
        "audio/mpeg, "
        "mpegversion = (int) 1, "
        "layer = (int) [ 1, 3 ], "
        "rate = (int) [ 8000, 48000 ], "
        "channels = (int) [ 1, 2 ]; "
        "audio/x-alaw, "
        "rate = (int) [ 8000, 48000 ], "
        "channels = (int) [ 1, 2 ]; "
        "audio/x-mulaw, "
        "rate = (int) [ 8000, 48000 ], " "channels = (int) [ 1, 2 ];"
        "audio/x-adpcm, "
        "layout = (string) microsoft, "
        "block_align = (int) [ 1, 8192 ], "
        "rate = (int) [ 8000, 48000 ], "
        "channels = (int) [ 1, 2 ]; "
        "audio/x-adpcm, "
        "layout = (string) dvi, "
        "block_align = (int) [ 1, 8192 ], "
        "rate = (int) [ 8000, 48000 ], " "channels = (int) [ 1, 2 ];"
        "audio/x-vnd.sony.atrac3;"
        "audio/x-dts;" "audio/x-wma, " "wmaversion = (int) [ 1, 2 ]")
    );


static GstElementClass *parent_class = NULL;

GType
gst_wavparse_get_type (void)
{
  static GType wavparse_type = 0;

  if (!wavparse_type) {
    static const GTypeInfo wavparse_info = {
      sizeof (GstWavParseClass),
      gst_wavparse_base_init,
      NULL,
      (GClassInitFunc) gst_wavparse_class_init,
      NULL,
      NULL,
      sizeof (GstWavParse),
      0,
      (GInstanceInitFunc) gst_wavparse_init,
    };

    wavparse_type =
        g_type_register_static (GST_TYPE_ELEMENT, "GstWavParse",
        &wavparse_info, 0);
  }
  return wavparse_type;
}


static void
gst_wavparse_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  /* register src pads */
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&sink_template_factory));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_template_factory));
  gst_element_class_set_details (element_class, &gst_wavparse_details);
}

static void
gst_wavparse_class_init (GstWavParseClass * klass)
{
  GstElementClass *gstelement_class;
  GObjectClass *object_class;

  gstelement_class = (GstElementClass *) klass;
  object_class = (GObjectClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  object_class->get_property = gst_wavparse_get_property;
  object_class->dispose = gst_wavparse_dispose;

  gstelement_class->change_state = gst_wavparse_change_state;
  gstelement_class->send_event = gst_wavparse_send_event;
}


static void
gst_wavparse_dispose (GObject * object)
{
	#ifndef __SYMBIAN32__
  GST_DEBUG("WAV: Dispose\n");
  #endif
  GstWavParse *wav = GST_WAVPARSE (object);
  #ifdef __SYMBIAN32__
  GST_DEBUG("WAV: Dispose\n");
	#endif

  if (wav->adapter) {
    g_object_unref (wav->adapter);
    wav->adapter = NULL;
  }

  G_OBJECT_CLASS (parent_class)->dispose (object);
}


static void
gst_wavparse_reset (GstWavParse * wavparse)
{
  wavparse->state = GST_WAVPARSE_START;

  /* These will all be set correctly in the fmt chunk */
  wavparse->depth = 0;
  wavparse->rate = 0;
  wavparse->width = 0;
  wavparse->channels = 0;
  wavparse->blockalign = 0;
  wavparse->bps = 0;
  wavparse->offset = 0;
  wavparse->end_offset = 0;
  wavparse->dataleft = 0;
  wavparse->datasize = 0;
  wavparse->datastart = 0;
  wavparse->got_fmt = FALSE;
  wavparse->first = TRUE;

  if (wavparse->seek_event)
    gst_event_unref (wavparse->seek_event);
  wavparse->seek_event = NULL;

  /* we keep the segment info in time */
  gst_segment_init (&wavparse->segment, GST_FORMAT_TIME);
}

static void
gst_wavparse_init (GstWavParse * wavparse)
{
  gst_wavparse_reset (wavparse);

  /* sink */
  wavparse->sinkpad =
      gst_pad_new_from_static_template (&sink_template_factory, "sink");
  gst_pad_set_activate_function (wavparse->sinkpad,
      GST_DEBUG_FUNCPTR (gst_wavparse_sink_activate));
  gst_pad_set_activatepull_function (wavparse->sinkpad,
      GST_DEBUG_FUNCPTR (gst_wavparse_sink_activate_pull));
  gst_pad_set_chain_function (wavparse->sinkpad,
      GST_DEBUG_FUNCPTR (gst_wavparse_chain));
  gst_element_add_pad (GST_ELEMENT (wavparse), wavparse->sinkpad);

  /* src, will be created later */
  wavparse->srcpad = NULL;
}

static void
gst_wavparse_destroy_sourcepad (GstWavParse * wavparse)
{
  if (wavparse->srcpad) {
    gst_element_remove_pad (GST_ELEMENT (wavparse), wavparse->srcpad);
    wavparse->srcpad = NULL;
  }
}

static void
gst_wavparse_create_sourcepad (GstWavParse * wavparse)
{
  /* destroy previous one */
  gst_wavparse_destroy_sourcepad (wavparse);

  /* source */
  wavparse->srcpad =
      gst_pad_new_from_static_template (&src_template_factory, "src");
  gst_pad_use_fixed_caps (wavparse->srcpad);
  gst_pad_set_query_type_function (wavparse->srcpad,
      GST_DEBUG_FUNCPTR (gst_wavparse_get_query_types));
  gst_pad_set_query_function (wavparse->srcpad,
      GST_DEBUG_FUNCPTR (gst_wavparse_pad_query));
  gst_pad_set_event_function (wavparse->srcpad,
      GST_DEBUG_FUNCPTR (gst_wavparse_srcpad_event));

  GST_DEBUG_OBJECT (wavparse, "srcpad created");
}

static void
gst_wavparse_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GstWavParse *wavparse;

  wavparse = GST_WAVPARSE (object);

  switch (prop_id) {
    default:
      break;
  }
}



#if 0
static void
gst_wavparse_parse_adtl (GstWavParse * wavparse, int len)
{
  guint32 got_bytes;
  GstByteStream *bs = wavparse->bs;
  gst_riff_chunk *temp_chunk, chunk;
  guint8 *tempdata;
  struct _gst_riff_labl labl, *temp_labl;
  struct _gst_riff_ltxt ltxt, *temp_ltxt;
  struct _gst_riff_note note, *temp_note;
  char *label_name;
  GstProps *props;
  GstPropsEntry *entry;
  GstCaps *new_caps;
  GList *caps = NULL;

  props = wavparse->metadata->properties;

  while (len > 0) {
    got_bytes =
        gst_bytestream_peek_bytes (bs, &tempdata, sizeof (gst_riff_chunk));
    if (got_bytes != sizeof (gst_riff_chunk)) {
      return;
    }
    temp_chunk = (gst_riff_chunk *) tempdata;

    chunk.id = GUINT32_FROM_LE (temp_chunk->id);
    chunk.size = GUINT32_FROM_LE (temp_chunk->size);

    if (chunk.size == 0) {
      gst_bytestream_flush (bs, sizeof (gst_riff_chunk));
      len -= sizeof (gst_riff_chunk);
      continue;
    }

    switch (chunk.id) {
      case GST_RIFF_adtl_labl:
        got_bytes =
            gst_bytestream_peek_bytes (bs, &tempdata,
            sizeof (struct _gst_riff_labl));
        if (got_bytes != sizeof (struct _gst_riff_labl)) {
          return;
        }

        temp_labl = (struct _gst_riff_labl *) tempdata;
        labl.id = GUINT32_FROM_LE (temp_labl->id);
        labl.size = GUINT32_FROM_LE (temp_labl->size);
        labl.identifier = GUINT32_FROM_LE (temp_labl->identifier);

        gst_bytestream_flush (bs, sizeof (struct _gst_riff_labl));
        len -= sizeof (struct _gst_riff_labl);

        got_bytes = gst_bytestream_peek_bytes (bs, &tempdata, labl.size - 4);
        if (got_bytes != labl.size - 4) {
          return;
        }

        label_name = (char *) tempdata;

        gst_bytestream_flush (bs, ((labl.size - 4) + 1) & ~1);
        len -= (((labl.size - 4) + 1) & ~1);

        new_caps = gst_caps_new ("label",
            "application/x-gst-metadata",
            gst_props_new ("identifier", G_TYPE_INT (labl.identifier),
                "name", G_TYPE_STRING (label_name), NULL));

        if (gst_props_get (props, "labels", &caps, NULL)) {
          caps = g_list_append (caps, new_caps);
        } else {
          caps = g_list_append (NULL, new_caps);

          entry = gst_props_entry_new ("labels", GST_PROPS_GLIST (caps));
          gst_props_add_entry (props, entry);
        }

        break;

      case GST_RIFF_adtl_ltxt:
        got_bytes =
            gst_bytestream_peek_bytes (bs, &tempdata,
            sizeof (struct _gst_riff_ltxt));
        if (got_bytes != sizeof (struct _gst_riff_ltxt)) {
          return;
        }

        temp_ltxt = (struct _gst_riff_ltxt *) tempdata;
        ltxt.id = GUINT32_FROM_LE (temp_ltxt->id);
        ltxt.size = GUINT32_FROM_LE (temp_ltxt->size);
        ltxt.identifier = GUINT32_FROM_LE (temp_ltxt->identifier);
        ltxt.length = GUINT32_FROM_LE (temp_ltxt->length);
        ltxt.purpose = GUINT32_FROM_LE (temp_ltxt->purpose);
        ltxt.country = GUINT16_FROM_LE (temp_ltxt->country);
        ltxt.language = GUINT16_FROM_LE (temp_ltxt->language);
        ltxt.dialect = GUINT16_FROM_LE (temp_ltxt->dialect);
        ltxt.codepage = GUINT16_FROM_LE (temp_ltxt->codepage);

        gst_bytestream_flush (bs, sizeof (struct _gst_riff_ltxt));
        len -= sizeof (struct _gst_riff_ltxt);

        if (ltxt.size - 20 > 0) {
          got_bytes = gst_bytestream_peek_bytes (bs, &tempdata, ltxt.size - 20);
          if (got_bytes != ltxt.size - 20) {
            return;
          }

          gst_bytestream_flush (bs, ((ltxt.size - 20) + 1) & ~1);
          len -= (((ltxt.size - 20) + 1) & ~1);

          label_name = (char *) tempdata;
        } else {
          label_name = "";
        }

        new_caps = gst_caps_new ("ltxt",
            "application/x-gst-metadata",
            gst_props_new ("identifier", G_TYPE_INT (ltxt.identifier),
                "name", G_TYPE_STRING (label_name),
                "length", G_TYPE_INT (ltxt.length), NULL));

        if (gst_props_get (props, "ltxts", &caps, NULL)) {
          caps = g_list_append (caps, new_caps);
        } else {
          caps = g_list_append (NULL, new_caps);

          entry = gst_props_entry_new ("ltxts", GST_PROPS_GLIST (caps));
          gst_props_add_entry (props, entry);
        }

        break;

      case GST_RIFF_adtl_note:
        got_bytes =
            gst_bytestream_peek_bytes (bs, &tempdata,
            sizeof (struct _gst_riff_note));
        if (got_bytes != sizeof (struct _gst_riff_note)) {
          return;
        }

        temp_note = (struct _gst_riff_note *) tempdata;
        note.id = GUINT32_FROM_LE (temp_note->id);
        note.size = GUINT32_FROM_LE (temp_note->size);
        note.identifier = GUINT32_FROM_LE (temp_note->identifier);

        gst_bytestream_flush (bs, sizeof (struct _gst_riff_note));
        len -= sizeof (struct _gst_riff_note);

        got_bytes = gst_bytestream_peek_bytes (bs, &tempdata, note.size - 4);
        if (got_bytes != note.size - 4) {
          return;
        }

        gst_bytestream_flush (bs, ((note.size - 4) + 1) & ~1);
        len -= (((note.size - 4) + 1) & ~1);

        label_name = (char *) tempdata;

        new_caps = gst_caps_new ("note",
            "application/x-gst-metadata",
            gst_props_new ("identifier", G_TYPE_INT (note.identifier),
                "name", G_TYPE_STRING (label_name), NULL));

        if (gst_props_get (props, "notes", &caps, NULL)) {
          caps = g_list_append (caps, new_caps);
        } else {
          caps = g_list_append (NULL, new_caps);

          entry = gst_props_entry_new ("notes", GST_PROPS_GLIST (caps));
          gst_props_add_entry (props, entry);
        }

        break;

      default:
        g_print ("Unknown chunk: %" GST_FOURCC_FORMAT "\n",
            GST_FOURCC_ARGS (chunk.id));
        return;
    }
  }

  g_object_notify (G_OBJECT (wavparse), "metadata");
}

static void
gst_wavparse_parse_cues (GstWavParse * wavparse, int len)
{
  guint32 got_bytes;
  GstByteStream *bs = wavparse->bs;
  struct _gst_riff_cue *temp_cue, cue;
  struct _gst_riff_cuepoints *points;
  guint8 *tempdata;
  int i;
  GList *cues = NULL;
  GstPropsEntry *entry;

  while (len > 0) {
    int required;

    got_bytes =
        gst_bytestream_peek_bytes (bs, &tempdata,
        sizeof (struct _gst_riff_cue));
    temp_cue = (struct _gst_riff_cue *) tempdata;

    /* fixup for our big endian friends */
    cue.id = GUINT32_FROM_LE (temp_cue->id);
    cue.size = GUINT32_FROM_LE (temp_cue->size);
    cue.cuepoints = GUINT32_FROM_LE (temp_cue->cuepoints);

    gst_bytestream_flush (bs, sizeof (struct _gst_riff_cue));
    if (got_bytes != sizeof (struct _gst_riff_cue)) {
      return;
    }

    len -= sizeof (struct _gst_riff_cue);

    /* -4 because cue.size contains the cuepoints size
       and we've already flushed that out of the system */
    required = cue.size - 4;
    got_bytes = gst_bytestream_peek_bytes (bs, &tempdata, required);
    gst_bytestream_flush (bs, ((required) + 1) & ~1);
    if (got_bytes != required) {
      return;
    }

    len -= (((cue.size - 4) + 1) & ~1);

    /* now we have an array of struct _gst_riff_cuepoints in tempdata */
    points = (struct _gst_riff_cuepoints *) tempdata;

    for (i = 0; i < cue.cuepoints; i++) {
      GstCaps *caps;

      caps = gst_caps_new ("cues",
          "application/x-gst-metadata",
          gst_props_new ("identifier", G_TYPE_INT (points[i].identifier),
              "position", G_TYPE_INT (points[i].offset), NULL));
      cues = g_list_append (cues, caps);
    }

    entry = gst_props_entry_new ("cues", GST_PROPS_GLIST (cues));
    gst_props_add_entry (wavparse->metadata->properties, entry);
  }

  g_object_notify (G_OBJECT (wavparse), "metadata");
}

/* Read 'fmt ' header */
static gboolean
gst_wavparse_fmt (GstWavParse * wav)
{
  gst_riff_strf_auds *header = NULL;
  GstCaps *caps;

  if (!gst_riff_read_strf_auds (wav, &header)) {
    g_warning ("Not fmt");
    return FALSE;
  }

  wav->format = header->format;
  wav->rate = header->rate;
  wav->channels = header->channels;
  if (wav->channels == 0) {
    GST_ELEMENT_ERROR (wav, STREAM, FAILED, (NULL),
        ("Stream claims to contain zero channels - invalid data"));
    g_free (header);
    return FALSE;
  }
  wav->blockalign = header->blockalign;
  wav->width = (header->blockalign * 8) / header->channels;
  wav->depth = header->size;
  wav->bps = header->av_bps;
  if (wav->bps <= 0) {
    GST_ELEMENT_ERROR (wav, STREAM, FAILED, (NULL),
        ("Stream claims to bitrate of <= zero - invalid data"));
    g_free (header);
    return FALSE;
  }

  /* Note: gst_riff_create_audio_caps might nedd to fix values in
   * the header header depending on the format, so call it first */
  caps = gst_riff_create_audio_caps (header->format, NULL, header, NULL);

  g_free (header);

  if (caps) {
    gst_wavparse_create_sourcepad (wav);
    gst_pad_use_fixed_caps (wav->srcpad);
    gst_pad_set_active (wav->srcpad, TRUE);
    gst_pad_set_caps (wav->srcpad, caps);
    gst_caps_free (caps);
    gst_element_add_pad (GST_ELEMENT (wav), wav->srcpad);
    gst_element_no_more_pads (GST_ELEMENT (wav));
    GST_DEBUG ("frequency %d, channels %d", wav->rate, wav->channels);
  } else {
    GST_ELEMENT_ERROR (wav, STREAM, TYPE_NOT_FOUND, (NULL), (NULL));
    return FALSE;
  }

  return TRUE;
}

static gboolean
gst_wavparse_other (GstWavParse * wav)
{
  guint32 tag, length;

  if (!gst_riff_peek_head (wav, &tag, &length, NULL)) {
    GST_WARNING_OBJECT (wav, "could not peek head");
    return FALSE;
  }
  GST_DEBUG_OBJECT (wav, "got tag (%08x) %4.4s, length %d", tag,
      (gchar *) & tag, length);

  switch (tag) {
    case GST_RIFF_TAG_LIST:
      if (!(tag = gst_riff_peek_list (wav))) {
        GST_WARNING_OBJECT (wav, "could not peek list");
        return FALSE;
      }

      switch (tag) {
        case GST_RIFF_LIST_INFO:
          if (!gst_riff_read_list (wav, &tag) || !gst_riff_read_info (wav)) {
            GST_WARNING_OBJECT (wav, "could not read list");
            return FALSE;
          }
          break;

        case GST_RIFF_LIST_adtl:
          if (!gst_riff_read_skip (wav)) {
            GST_WARNING_OBJECT (wav, "could not read skip");
            return FALSE;
          }
          break;

        default:
          GST_DEBUG_OBJECT (wav, "skipping tag (%08x) %4.4s", tag,
              (gchar *) & tag);
          if (!gst_riff_read_skip (wav)) {
            GST_WARNING_OBJECT (wav, "could not read skip");
            return FALSE;
          }
          break;
      }

      break;

    case GST_RIFF_TAG_data:
      if (!gst_bytestream_flush (wav->bs, 8)) {
        GST_WARNING_OBJECT (wav, "could not flush 8 bytes");
        return FALSE;
      }

      GST_DEBUG_OBJECT (wav, "switching to data mode");
      wav->state = GST_WAVPARSE_DATA;
      wav->datastart = gst_bytestream_tell (wav->bs);
      if (length == 0) {
        guint64 file_length;

        /* length is 0, data probably stretches to the end
         * of file */
        GST_DEBUG_OBJECT (wav, "length is 0 trying to find length");
        /* get length of file */
        file_length = gst_bytestream_length (wav->bs);
        if (file_length == -1) {
          GST_DEBUG_OBJECT (wav,
              "could not get file length, assuming data to eof");
          /* could not get length, assuming till eof */
          length = G_MAXUINT32;
        }
        if (file_length > G_MAXUINT32) {
          GST_DEBUG_OBJECT (wav, "file length %lld, clipping to 32 bits");
          /* could not get length, assuming till eof */
          length = G_MAXUINT32;
        } else {
          GST_DEBUG_OBJECT (wav, "file length %lld, datalength",
              file_length, length);
          /* substract offset of datastart from length */
          length = file_length - wav->datastart;
          GST_DEBUG_OBJECT (wav, "datalength %lld", length);
        }
      }
      wav->datasize = (guint64) length;
      break;

    case GST_RIFF_TAG_cue:
      if (!gst_riff_read_skip (wav)) {
        GST_WARNING_OBJECT (wav, "could not read skip");
        return FALSE;
      }
      break;

    default:
      GST_DEBUG_OBJECT (wav, "skipping tag (%08x) %4.4s", tag, (gchar *) & tag);
      if (!gst_riff_read_skip (wav))
        return FALSE;
      break;
  }

  return TRUE;
}
#endif



static gboolean
gst_wavparse_parse_file_header (GstElement * element, GstBuffer * buf)
{
  guint32 doctype;
  
  if (!gst_riff_parse_file_header (element, buf, &doctype))
   return FALSE;

  if (doctype != GST_RIFF_RIFF_WAVE)
    goto not_wav;

  return TRUE;

  /* ERRORS */
not_wav:

  {
    GST_ELEMENT_ERROR (element, STREAM, WRONG_TYPE, (NULL),
        ("File is not an WAVE file: %" GST_FOURCC_FORMAT,
            GST_FOURCC_ARGS (doctype)));
    return FALSE;
  }
}

static GstFlowReturn
gst_wavparse_stream_init (GstWavParse * wav)
{
  GstFlowReturn res;
  GstBuffer *buf = NULL;

  
  if ((res = gst_pad_pull_range (wav->sinkpad,
              wav->offset, 12, &buf)) != GST_FLOW_OK)
      {
 
    return res;
      }
  else if (!gst_wavparse_parse_file_header (GST_ELEMENT (wav), buf))
    return GST_FLOW_ERROR;

  wav->offset += 12;

  return GST_FLOW_OK;
}

/* This function is used to perform seeks on the element in 
 * pull mode.
 *
 * It also works when event is NULL, in which case it will just
 * start from the last configured segment. This technique is
 * used when activating the element and to perform the seek in
 * READY.
 */
static gboolean
gst_wavparse_perform_seek (GstWavParse * wav, GstEvent * event)
{
  gboolean res;
  gdouble rate;
  GstEvent *newsegment;
  GstFormat format;
  GstSeekFlags flags;
  GstSeekType cur_type, stop_type;
  gint64 cur, stop;
  gboolean flush;
  gboolean update;
  GstSegment seeksegment;

  if (event) {
    GST_DEBUG_OBJECT (wav, "doing seek with event");

    gst_event_parse_seek (event, &rate, &format, &flags,
        &cur_type, &cur, &stop_type, &stop);

    /* we have to have a format as the segment format. Try to convert
     * if not. */
    if (format != GST_FORMAT_TIME) {
      GstFormat fmt;

      fmt = GST_FORMAT_TIME;
      res = TRUE;
      if (cur_type != GST_SEEK_TYPE_NONE)
        res = gst_pad_query_convert (wav->srcpad, format, cur, &fmt, &cur);
      if (res && stop_type != GST_SEEK_TYPE_NONE)
        res = gst_pad_query_convert (wav->srcpad, format, stop, &fmt, &stop);
      if (!res)
        goto no_format;

      format = fmt;
    }
  } else {
    GST_DEBUG_OBJECT (wav, "doing seek without event");
    flags = 0;
  }

  flush = flags & GST_SEEK_FLAG_FLUSH;

  if (flush && wav->srcpad) {
    GST_DEBUG_OBJECT (wav, "sending flush start");
    gst_pad_push_event (wav->srcpad, gst_event_new_flush_start ());
  } else {
    gst_pad_pause_task (wav->sinkpad);
  }

  GST_PAD_STREAM_LOCK (wav->sinkpad);

  /* copy segment, we need this because we still need the old
   * segment when we close the current segment. */
  memcpy (&seeksegment, &wav->segment, sizeof (GstSegment));

  if (event) {
    GST_DEBUG_OBJECT (wav, "configuring seek");
    gst_segment_set_seek (&seeksegment, rate, format, flags,
        cur_type, cur, stop_type, stop, &update);
  }

  if ((stop = seeksegment.stop) == -1)
    stop = seeksegment.duration;

  if (cur_type != GST_SEEK_TYPE_NONE) {
    wav->offset =
        gst_util_uint64_scale_int (seeksegment.last_stop, wav->bps, GST_SECOND);
    wav->offset -= wav->offset % wav->bytes_per_sample;
    wav->offset += wav->datastart;
  }

  if (stop != -1) {
    wav->end_offset = gst_util_uint64_scale_int (stop, wav->bps, GST_SECOND);
    wav->end_offset +=
        wav->bytes_per_sample - (wav->end_offset % wav->bytes_per_sample);
    wav->end_offset += wav->datastart;
  } else {
    wav->end_offset = wav->datasize + wav->datastart;
  }
  wav->offset = MIN (wav->offset, wav->end_offset);
  wav->dataleft = wav->end_offset - wav->offset;

  GST_DEBUG_OBJECT (wav,
      "seek: offset %" G_GUINT64_FORMAT ", end %" G_GUINT64_FORMAT ", segment %"
      GST_TIME_FORMAT " -- %" GST_TIME_FORMAT, wav->offset, wav->end_offset,
      GST_TIME_ARGS (seeksegment.start), GST_TIME_ARGS (stop));

  /* prepare for streaming again */
  if (wav->srcpad) {
    if (flush) {
      GST_DEBUG_OBJECT (wav, "sending flush stop");
      gst_pad_push_event (wav->srcpad, gst_event_new_flush_stop ());
    } else if (wav->segment_running) {
      /* we are running the current segment and doing a non-flushing seek,
       * close the segment first based on the last_stop. */
      GST_DEBUG_OBJECT (wav, "closing running segment %" G_GINT64_FORMAT
          " to %" G_GINT64_FORMAT, wav->segment.start, wav->segment.last_stop);

      gst_pad_push_event (wav->srcpad,
          gst_event_new_new_segment (TRUE,
              wav->segment.rate, wav->segment.format,
              wav->segment.start, wav->segment.last_stop, wav->segment.time));
    }
  }

  memcpy (&wav->segment, &seeksegment, sizeof (GstSegment));

  if (wav->segment.flags & GST_SEEK_FLAG_SEGMENT) {
    gst_element_post_message (GST_ELEMENT (wav),
        gst_message_new_segment_start (GST_OBJECT (wav),
            wav->segment.format, wav->segment.last_stop));
  }

  /* now send the newsegment */
  GST_DEBUG_OBJECT (wav, "Sending newsegment from %" G_GINT64_FORMAT
      " to %" G_GINT64_FORMAT, wav->segment.start, stop);

  newsegment =
      gst_event_new_new_segment (FALSE, wav->segment.rate,
      wav->segment.format, wav->segment.last_stop, stop, wav->segment.time);

  if (wav->srcpad) {
    gst_pad_push_event (wav->srcpad, newsegment);
  } else {
    /* send later when we actually create the source pad */
    g_assert (wav->newsegment == NULL);
    wav->newsegment = newsegment;
  }

  wav->segment_running = TRUE;
  if (!wav->streaming) {
    gst_pad_start_task (wav->sinkpad, (GstTaskFunction) gst_wavparse_loop,
        wav->sinkpad);
  }

  GST_PAD_STREAM_UNLOCK (wav->sinkpad);

  return TRUE;

  /* ERRORS */
no_format:
  {
    GST_DEBUG_OBJECT (wav, "unsupported format given, seek aborted.");
    return FALSE;
  }
}

/*
 * gst_wavparse_peek_chunk_info:
 * @wav Wavparse object
 * @tag holder for tag
 * @size holder for tag size
 *                         
 * Peek next chunk info (tag and size)                         
 *
 * Returns: %TRUE when one chunk info has been got from the adapter
 */
static gboolean
gst_wavparse_peek_chunk_info (GstWavParse * wav, guint32 * tag, guint32 * size)
{
  const guint8 *data = NULL;

  if (gst_adapter_available (wav->adapter) < 8) {
    return FALSE;
  }

  GST_DEBUG ("Next chunk size is %d bytes", *size);
  data = gst_adapter_peek (wav->adapter, 8);
  *tag = GST_READ_UINT32_LE (data);
  *size = GST_READ_UINT32_LE (data + 4);

  return TRUE;
}

/*
 * gst_wavparse_peek_chunk:
 * @wav Wavparse object
 * @tag holder for tag
 * @size holder for tag size
 *
 * Peek enough data for one full chunk
 *
 * Returns: %TRUE when one chunk has been got
 */
static gboolean
gst_wavparse_peek_chunk (GstWavParse * wav, guint32 * tag, guint32 * size)
{
  guint32 peek_size = 0;

  gst_wavparse_peek_chunk_info (wav, tag, size);
  GST_DEBUG ("Need to peek chunk of %d bytes", *size);
  peek_size = (*size + 1) & ~1;

  if (gst_adapter_available (wav->adapter) >= (8 + peek_size)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

static gboolean
gst_wavparse_get_upstream_size (GstWavParse * wav, gint64 * len)
{
  gboolean res = FALSE;
  GstFormat fmt = GST_FORMAT_BYTES;
  GstPad *peer;

  if ((peer = gst_pad_get_peer (wav->sinkpad))) {
    res = gst_pad_query_duration (peer, &fmt, len);
    gst_object_unref (peer);
  }

  return res;
}

static GstFlowReturn
gst_wavparse_stream_headers (GstWavParse * wav)
{
  GstFlowReturn res;
  GstBuffer *buf;
  gst_riff_strf_auds *header = NULL;
  guint32 tag, size;
  gboolean gotdata = FALSE;
  GstCaps *caps;
  gint64 duration;
  gchar *codec_name = NULL;
  GstEvent **event_p;

  
  if (!wav->got_fmt) {
    GstBuffer *extra;

    /* The header start with a 'fmt ' tag */

    if (wav->streaming) {
      if (!gst_wavparse_peek_chunk (wav, &tag, &size))
        return GST_FLOW_OK;

      buf = gst_buffer_new ();
      gst_buffer_ref (buf);
      gst_adapter_flush (wav->adapter, 8);
      wav->offset += 8;
      GST_BUFFER_DATA (buf) = (guint8 *) gst_adapter_peek (wav->adapter, size);
      GST_BUFFER_SIZE (buf) = size;

    } else {
      if ((res = gst_riff_read_chunk (GST_ELEMENT (wav), wav->sinkpad,
                  &wav->offset, &tag, &buf)) != GST_FLOW_OK)
        return res;
    }

    if (tag != GST_RIFF_TAG_fmt)
      goto invalid_wav;

    if (!(gst_riff_parse_strf_auds (GST_ELEMENT (wav), buf, &header, &extra)))
      goto parse_header_error;

    if (wav->streaming) {
      gst_adapter_flush (wav->adapter, size);
      wav->offset += size;
      GST_BUFFER_DATA (buf) = NULL;
      gst_buffer_unref (buf);
    }

    /* Note: gst_riff_create_audio_caps might nedd to fix values in
     * the header header depending on the format, so call it first */
    caps =
        gst_riff_create_audio_caps (header->format, NULL, header, extra,
        NULL, &codec_name);

    if (extra)
      gst_buffer_unref (extra);

    wav->format = header->format;
    wav->rate = header->rate;
    wav->channels = header->channels;

    if (wav->channels == 0)
      goto no_channels;

    wav->blockalign = header->blockalign;
    wav->width = (header->blockalign * 8) / header->channels;
    wav->depth = header->size;
    wav->bps = header->av_bps;

    if (wav->bps <= 0)
      goto no_bitrate;

    wav->bytes_per_sample = wav->channels * wav->width / 8;
    if (wav->bytes_per_sample <= 0)
      goto no_bytes_per_sample;

    g_free (header);

    if (!caps)
      goto unknown_format;

    GST_DEBUG_OBJECT (wav, "blockalign = %u", (guint) wav->blockalign);
    GST_DEBUG_OBJECT (wav, "width      = %u", (guint) wav->width);
    GST_DEBUG_OBJECT (wav, "depth      = %u", (guint) wav->depth);
    GST_DEBUG_OBJECT (wav, "bps        = %u", (guint) wav->bps);

    /* create pad later so we can sniff the first few bytes
     * of the real data and correct our caps if necessary */
    gst_caps_replace (&wav->caps, caps);
    gst_caps_replace (&caps, NULL);

    wav->got_fmt = TRUE;

    if (codec_name) {
      wav->tags = gst_tag_list_new ();

      gst_tag_list_add (wav->tags, GST_TAG_MERGE_REPLACE,
          GST_TAG_AUDIO_CODEC, codec_name, NULL);

      g_free (codec_name);
      codec_name = NULL;
    }

    GST_DEBUG_OBJECT (wav, "frequency %d, channels %d", wav->rate,
        wav->channels);
  }

  /* loop headers until we get data */
  while (!gotdata) {
    if (wav->streaming) {
      if (!gst_wavparse_peek_chunk_info (wav, &tag, &size))
        return GST_FLOW_OK;
    } else {
      if ((res =
              gst_pad_pull_range (wav->sinkpad, wav->offset, 8,
                  &buf)) != GST_FLOW_OK)
        goto header_read_error;
      tag = GST_READ_UINT32_LE (GST_BUFFER_DATA (buf));
      size = GST_READ_UINT32_LE (GST_BUFFER_DATA (buf) + 4);
    }

    /*
       wav is a st00pid format, we don't know for sure where data starts.
       So we have to go bit by bit until we find the 'data' header
     */

    switch (tag) {
        /* TODO : Implement the various cases */
      case GST_RIFF_TAG_data:{
        gint64 upstream_size;

        GST_DEBUG_OBJECT (wav, "Got 'data' TAG, size : %d", size);
        gotdata = TRUE;
        if (wav->streaming) {
          gst_adapter_flush (wav->adapter, 8);
        } else {
          gst_buffer_unref (buf);
        }
        wav->offset += 8;
        wav->datastart = wav->offset;
        /* file might be truncated */
        if (gst_wavparse_get_upstream_size (wav, &upstream_size)) {
          size = MIN (size, (upstream_size - wav->datastart));
        }
        wav->datasize = size;
        wav->dataleft = size;
        wav->end_offset = size + wav->datastart;
        break;
      }
      default:
        if (wav->streaming) {
          if (!gst_wavparse_peek_chunk (wav, &tag, &size))
            return GST_FLOW_OK;
        }
        GST_DEBUG_OBJECT (wav, "Ignoring tag %" GST_FOURCC_FORMAT,
            GST_FOURCC_ARGS (tag));
        wav->offset += 8 + ((size + 1) & ~1);
        if (wav->streaming) {
          gst_adapter_flush (wav->adapter, 8 + ((size + 1) & ~1));
        } else {
          gst_buffer_unref (buf);
        }
    }
  }

  GST_DEBUG_OBJECT (wav, "Finished parsing headers");

  duration = gst_util_uint64_scale_int (wav->datasize, GST_SECOND, wav->bps);
  GST_DEBUG_OBJECT (wav, "Got duration %" GST_TIME_FORMAT,
      GST_TIME_ARGS (duration));
  gst_segment_set_duration (&wav->segment, GST_FORMAT_TIME, duration);

  /* now we have all the info to perform a pending seek if any, if no
   * event, this will still do the right thing and it will also send
   * the right newsegment event downstream. */
  gst_wavparse_perform_seek (wav, wav->seek_event);
  /* remove pending event */
  event_p = &wav->seek_event;
  gst_event_replace (event_p, NULL);

  wav->state = GST_WAVPARSE_DATA;

  return GST_FLOW_OK;

  /* ERROR */
invalid_wav:
  {
    GST_ELEMENT_ERROR (wav, STREAM, DEMUX, (NULL),
        ("Invalid WAV header (no fmt at start): %"
            GST_FOURCC_FORMAT, GST_FOURCC_ARGS (tag)));
    g_free (codec_name);

    return GST_FLOW_ERROR;
  }
parse_header_error:
  {
    GST_ELEMENT_ERROR (wav, STREAM, DEMUX, (NULL),
        ("Couldn't parse audio header"));
    gst_buffer_unref (buf);
    g_free (codec_name);
 
    return GST_FLOW_ERROR;
  }
no_channels:
  {
    GST_ELEMENT_ERROR (wav, STREAM, FAILED, (NULL),
        ("Stream claims to contain no channels - invalid data"));
    g_free (header);
    g_free (codec_name);
    return GST_FLOW_ERROR;
  }
no_bitrate:
  {
    GST_ELEMENT_ERROR (wav, STREAM, FAILED, (NULL),
        ("Stream claims to have a bitrate of <= zero - invalid data"));
    g_free (header);
    g_free (codec_name);
    return GST_FLOW_ERROR;
  }
no_bytes_per_sample:
  {
    GST_ELEMENT_ERROR (wav, STREAM, FAILED, (NULL),
        ("could not caluclate bytes per sample - invalid data"));
    g_free (header);
    g_free (codec_name);
    return GST_FLOW_ERROR;
  }
unknown_format:
  {
    GST_ELEMENT_ERROR (wav, STREAM, TYPE_NOT_FOUND, (NULL),
        ("No caps found for format 0x%x, %d channels, %d Hz",
            wav->format, wav->channels, wav->rate));
    g_free (codec_name);
    return GST_FLOW_ERROR;
  }
header_read_error:
  {
    GST_ELEMENT_ERROR (wav, STREAM, DEMUX, (NULL), ("Couldn't read in header"));
    g_free (codec_name);
    return GST_FLOW_ERROR;
  }
}


/*                       
 * Read WAV file tag when streaming
 */
static GstFlowReturn
gst_wavparse_parse_stream_init (GstWavParse * wav)
{
  if (gst_adapter_available (wav->adapter) >= 12) {
    GstBuffer *tmp = gst_buffer_new ();

    /* _take flushes the data */
    GST_BUFFER_DATA (tmp) = gst_adapter_take (wav->adapter, 12);
    GST_BUFFER_SIZE (tmp) = 12;

    GST_DEBUG ("Parsing wav header");
    if (!gst_wavparse_parse_file_header (GST_ELEMENT (wav), tmp)) {
      return GST_FLOW_ERROR;
    }

    wav->offset += 12;
    /* Go to next state */
    wav->state = GST_WAVPARSE_HEADER;
  }
  return GST_FLOW_OK;
}

/* handle an event sent directly to the element.
 *
 * This event can be sent either in the READY state or the
 * >READY state. The only event of interest really is the seek
 * event.
 *
 * In the READY state we can only store the event and try to
 * respect it when going to PAUSED. We assume we are in the
 * READY state when our parsing state != GST_WAVPARSE_DATA.
 *
 * When we are steaming, we can simply perform the seek right
 * away.
 */
static gboolean
gst_wavparse_send_event (GstElement * element, GstEvent * event)
{
  GstWavParse *wav = GST_WAVPARSE (element);
  gboolean res = FALSE;
  GstEvent **event_p;

  GST_DEBUG_OBJECT (wav, "received event %s", GST_EVENT_TYPE_NAME (event));

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEEK:
      if (wav->state == GST_WAVPARSE_DATA) {
        /* we can handle the seek directly when streaming data */
        res = gst_wavparse_perform_seek (wav, event);
      } else {
        GST_DEBUG_OBJECT (wav, "queuing seek for later");

        event_p = &wav->seek_event;
        gst_event_replace (event_p, event);

        /* we always return true */
        res = TRUE;
      }
      break;
    default:
      break;
  }
  gst_event_unref (event);
  return res;
}

static void
gst_wavparse_add_src_pad (GstWavParse * wav, GstBuffer * buf)
{
  GstStructure *s;
  const guint8 dts_marker[] = { 0xFF, 0x1F, 0x00, 0xE8, 0xF1, 0x07 };

  
  s = gst_caps_get_structure (wav->caps, 0);
  if (gst_structure_has_name (s, "audio/x-raw-int") &&
      GST_BUFFER_SIZE (buf) > 6 &&
      memcmp (GST_BUFFER_DATA (buf), dts_marker, 6) == 0) {

    GST_WARNING_OBJECT (wav, "Found DTS marker in file marked as raw PCM");
    gst_caps_unref (wav->caps);
    wav->caps = gst_caps_from_string ("audio/x-dts");

    gst_tag_list_add (wav->tags, GST_TAG_MERGE_REPLACE,
        GST_TAG_AUDIO_CODEC, "dts", NULL);
  
  }

  gst_wavparse_create_sourcepad (wav);
  gst_pad_set_active (wav->srcpad, TRUE);
  gst_pad_set_caps (wav->srcpad, wav->caps);
  gst_caps_replace (&wav->caps, NULL);

  gst_element_add_pad (GST_ELEMENT (wav), wav->srcpad);
   

  gst_element_no_more_pads (GST_ELEMENT (wav));

  GST_DEBUG_OBJECT (wav, "Send newsegment event on newpad");
  gst_pad_push_event (wav->srcpad, wav->newsegment);
  wav->newsegment = NULL;

  
  if (wav->tags) {
    gst_element_found_tags_for_pad (GST_ELEMENT (wav), wav->srcpad, wav->tags);
    wav->tags = NULL;
  }

}

#define MAX_BUFFER_SIZE 4096

static GstFlowReturn
gst_wavparse_stream_data (GstWavParse * wav)
{
  GstBuffer *buf = NULL;
  GstFlowReturn res = GST_FLOW_OK;
  guint64 desired, obtained;
  GstClockTime timestamp, next_timestamp;
  guint64 pos, nextpos;

 
iterate_adapter:
  GST_LOG_OBJECT (wav,
      "offset: %" G_GINT64_FORMAT " , end: %" G_GINT64_FORMAT " , dataleft: %"
      G_GINT64_FORMAT, wav->offset, wav->end_offset, wav->dataleft);

  /* Get the next n bytes and output them */
  if (wav->dataleft == 0 || wav->dataleft < wav->blockalign)
    goto found_eos;

  /* scale the amount of data by the segment rate so we get equal
   * amounts of data regardless of the playback rate */
  desired =
      MIN (gst_guint64_to_gdouble (wav->dataleft),
      MAX_BUFFER_SIZE * ABS (wav->segment.rate));
  if (desired >= wav->blockalign && wav->blockalign > 0)
    desired -= (desired % wav->blockalign);

  
  GST_LOG_OBJECT (wav, "Fetching %" G_GINT64_FORMAT " bytes of data "
      "from the sinkpad", desired);

  if (wav->streaming) {
    guint avail = gst_adapter_available (wav->adapter);
     
    if (avail < desired) {
   
      GST_LOG_OBJECT (wav, "Got only %d bytes of data from the sinkpad", avail);
      return GST_FLOW_OK;
    }

    buf = gst_buffer_new ();
    GST_BUFFER_DATA (buf) = gst_adapter_take (wav->adapter, desired);
    GST_BUFFER_SIZE (buf) = desired;

    
  } else {
    if ((res = gst_pad_pull_range (wav->sinkpad, wav->offset,
                desired, &buf)) != GST_FLOW_OK)
        {
   
      goto pull_error;
        }

  }

  /* first chunk of data? create the source pad. We do this only here so
   * we can detect broken .wav files with dts disguised as raw PCM (sigh) */
  if (G_UNLIKELY (wav->first)) {
    wav->first = FALSE;

    gst_wavparse_add_src_pad (wav, buf);
  }

  obtained = GST_BUFFER_SIZE (buf);

  /* our positions */
  pos = wav->offset - wav->datastart;
  nextpos = pos + obtained;

  /* update offsets, does not overflow. */
  GST_BUFFER_OFFSET (buf) = pos / wav->bytes_per_sample;
  GST_BUFFER_OFFSET_END (buf) = nextpos / wav->bytes_per_sample;

  /* and timestamps, be carefull for overflows */
  timestamp = gst_util_uint64_scale_int (pos, GST_SECOND, wav->bps);
  next_timestamp = gst_util_uint64_scale_int (nextpos, GST_SECOND, wav->bps);

  GST_BUFFER_TIMESTAMP (buf) = timestamp;
  GST_BUFFER_DURATION (buf) = next_timestamp - timestamp;

  /* update current running segment position */
  gst_segment_set_last_stop (&wav->segment, GST_FORMAT_TIME, next_timestamp);

  /* don't forget to set the caps on the buffer */
  gst_buffer_set_caps (buf, GST_PAD_CAPS (wav->srcpad));

  GST_LOG_OBJECT (wav,
      "Got buffer. timestamp:%" GST_TIME_FORMAT " , duration:%" GST_TIME_FORMAT
      ", size:%u", GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (buf)),
      GST_TIME_ARGS (GST_BUFFER_DURATION (buf)), GST_BUFFER_SIZE (buf));
  
     
  if ((res = gst_pad_push (wav->srcpad, buf)) != GST_FLOW_OK)
      {

    goto push_error;
      }
 
    
  if (obtained < wav->dataleft) {
    wav->dataleft -= obtained;
  } else {
    wav->dataleft = 0;
  }
  wav->offset += obtained;
  /* Iterate until need more data, so adapter size won't grow */
  if (wav->streaming) {
    GST_LOG_OBJECT (wav,
        "offset: %" G_GINT64_FORMAT " , end: %" G_GINT64_FORMAT, wav->offset,
        wav->end_offset);
    
    goto iterate_adapter;
  }

  return res;

  /* ERROR */
found_eos:
  {

    GST_DEBUG_OBJECT (wav, "found EOS");
    /* we completed the segment */
    wav->segment_running = FALSE;
    if (wav->segment.flags & GST_SEEK_FLAG_SEGMENT) {
      GstClockTime stop;

      if ((stop = wav->segment.stop) == -1)
        stop = wav->segment.duration;

      gst_element_post_message (GST_ELEMENT (wav),
          gst_message_new_segment_done (GST_OBJECT (wav), GST_FORMAT_TIME,
              stop));

    } else {
      gst_pad_push_event (wav->srcpad, gst_event_new_eos ());
 
    }
    return GST_FLOW_WRONG_STATE;
  }
pull_error:
  {
     
    GST_DEBUG_OBJECT (wav, "Error getting %" G_GINT64_FORMAT " bytes from the "
        "sinkpad (dataleft = %" G_GINT64_FORMAT ")", desired, wav->dataleft);
    return res;
  }
push_error:
  {
      
    GST_DEBUG_OBJECT (wav, "Error pushing on srcpad");
    return res;
  }
}

static void
gst_wavparse_loop (GstPad * pad)
{
  GstFlowReturn ret;
  GstWavParse *wav = GST_WAVPARSE (GST_PAD_PARENT (pad));

  GST_LOG_OBJECT (wav, "process data");

  switch (wav->state) {
    case GST_WAVPARSE_START:
      GST_DEBUG_OBJECT (wav, "GST_WAVPARSE_START");
      if ((ret = gst_wavparse_stream_init (wav)) != GST_FLOW_OK)
        goto pause;
      
      wav->state = GST_WAVPARSE_HEADER;
      /* fall-through */

    case GST_WAVPARSE_HEADER:
      GST_DEBUG_OBJECT (wav, "GST_WAVPARSE_HEADER");
      if ((ret = gst_wavparse_stream_headers (wav)) != GST_FLOW_OK)
        goto pause;

      wav->state = GST_WAVPARSE_DATA;
      /* fall-through */

    case GST_WAVPARSE_DATA:
      if ((ret = gst_wavparse_stream_data (wav)) != GST_FLOW_OK)
          {

        goto pause;
          }
 
        break;
    default:
      g_assert_not_reached ();
  }
 
  return;

  /* ERRORS */
pause:
  GST_LOG_OBJECT (wav, "pausing task %d", ret);
  gst_pad_pause_task (wav->sinkpad);
  if (GST_FLOW_IS_FATAL (ret)) {

     
    /* for fatal errors we post an error message */
    GST_ELEMENT_ERROR (wav, STREAM, FAILED,
        (_("Internal data stream error.")),
        ("streaming stopped, reason %s", gst_flow_get_name (ret)));
    if (wav->srcpad != NULL)
        {
      gst_pad_push_event (wav->srcpad, gst_event_new_eos ());
           }
  }
  
}

static GstFlowReturn
gst_wavparse_chain (GstPad * pad, GstBuffer * buf)
{
  GstFlowReturn ret;
  GstWavParse *wav = GST_WAVPARSE (GST_PAD_PARENT (pad));
   GST_LOG_OBJECT (wav, "adapter_push %" G_GINT64_FORMAT " bytes",
      GST_BUFFER_SIZE (buf));

  gst_adapter_push (wav->adapter, buf);
  switch (wav->state) {
    case GST_WAVPARSE_START:
      GST_DEBUG_OBJECT (wav, "GST_WAVPARSE_START");
      if ((ret = gst_wavparse_parse_stream_init (wav)) != GST_FLOW_OK) 
      {
   
        goto pause;
			}
			
      wav->state = GST_WAVPARSE_HEADER;
      /* fall-through */

    case GST_WAVPARSE_HEADER:
      GST_DEBUG_OBJECT (wav, "GST_WAVPARSE_HEADER");
      if ((ret = gst_wavparse_stream_headers (wav)) != GST_FLOW_OK)
        goto pause;
		
      wav->state = GST_WAVPARSE_DATA;
      /* fall-through */

    case GST_WAVPARSE_DATA:
      if ((ret = gst_wavparse_stream_data (wav)) != GST_FLOW_OK)
      {
 
        goto pause;
      }

      break;

    default:
      g_assert_not_reached ();
  }
	
  return ret;

pause:

  GST_LOG_OBJECT (wav, "pausing task %d", ret);
  gst_pad_pause_task (wav->sinkpad);
  if (GST_FLOW_IS_FATAL (ret)) {
    /* for fatal errors we post an error message */
 
    GST_ELEMENT_ERROR (wav, STREAM, FAILED,
        (_("Internal data stream error.")),
        ("streaming stopped, reason %s", gst_flow_get_name (ret)));
    if (wav->srcpad != NULL)
    {
      gst_pad_push_event (wav->srcpad, gst_event_new_eos ());
  	
  	}
  }

  return ret;
}

#if 0
/* convert and query stuff */
static const GstFormat *
gst_wavparse_get_formats (GstPad * pad)
{
  static GstFormat formats[] = {
    GST_FORMAT_TIME,
    GST_FORMAT_BYTES,
    GST_FORMAT_DEFAULT,         /* a "frame", ie a set of samples per Hz */
    0
  };

  return formats;
}
#endif

static gboolean
gst_wavparse_pad_convert (GstPad * pad,
    GstFormat src_format, gint64 src_value,
    GstFormat * dest_format, gint64 * dest_value)
{
  GstWavParse *wavparse;
  gboolean res = TRUE;

  wavparse = GST_WAVPARSE (gst_pad_get_parent (pad));

  if (wavparse->bytes_per_sample == 0)
    goto no_bytes_per_sample;

  if (wavparse->bps == 0)
    goto no_bps;

  switch (src_format) {
    case GST_FORMAT_BYTES:
      switch (*dest_format) {
        case GST_FORMAT_DEFAULT:
          *dest_value = src_value / wavparse->bytes_per_sample;
          break;
        case GST_FORMAT_TIME:
          *dest_value =
              gst_util_uint64_scale_int (src_value, GST_SECOND, wavparse->bps);
          break;
        default:
          res = FALSE;
          goto done;
      }
      *dest_value -= *dest_value % wavparse->bytes_per_sample;
      break;

    case GST_FORMAT_DEFAULT:
      switch (*dest_format) {
        case GST_FORMAT_BYTES:
          *dest_value = src_value * wavparse->bytes_per_sample;
          break;
        case GST_FORMAT_TIME:
          *dest_value =
              gst_util_uint64_scale_int (src_value, GST_SECOND, wavparse->rate);
          break;
        default:
          res = FALSE;
          goto done;
      }
      break;

    case GST_FORMAT_TIME:
      switch (*dest_format) {
        case GST_FORMAT_BYTES:
          /* make sure we end up on a sample boundary */
          *dest_value =
              gst_util_uint64_scale_int (src_value, wavparse->bps, GST_SECOND);
          *dest_value -= *dest_value % wavparse->blockalign;
          break;
        case GST_FORMAT_DEFAULT:
          *dest_value =
              gst_util_uint64_scale_int (src_value, wavparse->rate, GST_SECOND);
          break;
        default:
          res = FALSE;
          goto done;
      }
      break;

    default:
      res = FALSE;
      goto done;
  }

done:
  gst_object_unref (wavparse);

  return res;

  /* ERRORS */
no_bytes_per_sample:
  {
    GST_DEBUG_OBJECT (wavparse,
        "bytes_per_sample 0, probably an mp3 - channels %d, width %d",
        wavparse->channels, wavparse->width);
    res = FALSE;
    goto done;
  }
no_bps:
  {
    GST_DEBUG_OBJECT (wavparse, "bps 0, cannot convert");
    res = FALSE;
    goto done;
  }
}

static const GstQueryType *
gst_wavparse_get_query_types (GstPad * pad)
{
  static const GstQueryType types[] = {
    GST_QUERY_POSITION,
    GST_QUERY_DURATION,
    GST_QUERY_CONVERT,
    0
  };

  return types;
}

/* handle queries for location and length in requested format */
static gboolean
gst_wavparse_pad_query (GstPad * pad, GstQuery * query)
{
  gboolean res = TRUE;
  GstWavParse *wav = GST_WAVPARSE (GST_PAD_PARENT (pad));

  /* only if we know */
  if (wav->state != GST_WAVPARSE_DATA)
    return FALSE;

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_POSITION:
    {
      gint64 curb;
      gint64 cur;
      GstFormat format;
      gboolean res = TRUE;

      curb = wav->offset - wav->datastart;
      gst_query_parse_position (query, &format, NULL);

      switch (format) {
        case GST_FORMAT_TIME:
          res &=
              gst_wavparse_pad_convert (pad, GST_FORMAT_BYTES, curb,
              &format, &cur);
          break;
        default:
          format = GST_FORMAT_BYTES;
          cur = curb;
          break;
      }
      if (res)
        gst_query_set_position (query, format, cur);
      break;
    }
    case GST_QUERY_DURATION:
    {
      gint64 endb;
      gint64 end;
      GstFormat format;
      gboolean res = TRUE;

      endb = wav->datasize;
      gst_query_parse_duration (query, &format, NULL);

      switch (format) {
        case GST_FORMAT_TIME:
          res &=
              gst_wavparse_pad_convert (pad, GST_FORMAT_BYTES, endb,
              &format, &end);
          break;
        default:
          format = GST_FORMAT_BYTES;
          end = endb;
          break;
      }
      if (res)
        gst_query_set_duration (query, format, end);
      break;
    }
    case GST_QUERY_CONVERT:
    {
      gint64 srcvalue, dstvalue;
      GstFormat srcformat, dstformat;

      gst_query_parse_convert (query, &srcformat, &srcvalue,
          &dstformat, &dstvalue);
      res &=
          gst_wavparse_pad_convert (pad, srcformat, srcvalue,
          &dstformat, &dstvalue);
      if (res)
        gst_query_set_convert (query, srcformat, srcvalue, dstformat, dstvalue);
      break;
    }
    default:
      res = gst_pad_query_default (pad, query);
      break;
  }
  return res;
}

static gboolean
gst_wavparse_srcpad_event (GstPad * pad, GstEvent * event)
{
  GstWavParse *wavparse = GST_WAVPARSE (GST_PAD_PARENT (pad));
  gboolean res = TRUE;

  GST_DEBUG_OBJECT (wavparse, "event %d, %s", GST_EVENT_TYPE (event),
      GST_EVENT_TYPE_NAME (event));

  /* can only handle events when we are in the data state */
  if (wavparse->state != GST_WAVPARSE_DATA)
    return FALSE;

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEEK:
    {
      res = gst_wavparse_perform_seek (wavparse, event);
      break;
    }
    default:
      res = FALSE;
      break;
  }

  gst_event_unref (event);

  return res;
}

static gboolean
gst_wavparse_sink_activate (GstPad * sinkpad)
{
  GstWavParse *wav = GST_WAVPARSE (gst_pad_get_parent (sinkpad));
  gboolean res;

  if (gst_pad_check_pull_range (sinkpad)) {
    GST_DEBUG ("going to pull mode");
    wav->streaming = FALSE;
    wav->adapter = NULL;
    res = gst_pad_activate_pull (sinkpad, TRUE);
  } else {
    GST_DEBUG ("going to push (streaming) mode");
    wav->streaming = TRUE;
    wav->adapter = gst_adapter_new ();
    res = gst_pad_activate_push (sinkpad, TRUE);
  }
  gst_object_unref (wav);
  return res;
}


static gboolean
gst_wavparse_sink_activate_pull (GstPad * sinkpad, gboolean active)
{
  GstWavParse *wav = GST_WAVPARSE (gst_pad_get_parent (sinkpad));

  GST_DEBUG_OBJECT (wav, "activating pull");

  if (active) {
    /* if we have a scheduler we can start the task */
    wav->segment_running = TRUE;
    gst_pad_start_task (sinkpad, (GstTaskFunction) gst_wavparse_loop, sinkpad);
  } else {
    gst_pad_stop_task (sinkpad);
  }
  gst_object_unref (wav);

  return TRUE;
}

static GstStateChangeReturn
gst_wavparse_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret;
  GstWavParse *wav = GST_WAVPARSE (element);

  GST_DEBUG_OBJECT (wav, "changing state %s - %s",
      gst_element_state_get_name (GST_STATE_TRANSITION_CURRENT (transition)),
      gst_element_state_get_name (GST_STATE_TRANSITION_NEXT (transition)));

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      gst_wavparse_reset (wav);
      break;
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:{
      GstEvent **event_p = &wav->seek_event;

      gst_wavparse_destroy_sourcepad (wav);
      gst_event_replace (event_p, NULL);
      gst_wavparse_reset (wav);
      if (wav->adapter) {
        gst_adapter_clear (wav->adapter);
      }
      break;
    }
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }
  return ret;
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  gst_riff_init ();

  GST_DEBUG_CATEGORY_INIT (wavparse_debug, "wavparse", 0, "WAV parser");

  return gst_element_register (plugin, "wavparse", GST_RANK_PRIMARY,
      GST_TYPE_WAVPARSE);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "wavparse",
    "Parse a .wav file into raw audio",
    plugin_init, VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)


EXPORT_C GstPluginDesc* _GST_PLUGIN_DESC()
{
	return &gst_plugin_desc;
}

