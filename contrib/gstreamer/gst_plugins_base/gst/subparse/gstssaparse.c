/* GStreamer SSA subtitle parser
 * Copyright (c) 2006 Tim-Philipp Müller <tim centricular net>
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

/* Super-primitive SSA parser - we just want the text and ignore
 * everything else like styles and timing codes etc. for now */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>             /* atoi() */
#include <string.h>

#include "gstssaparse.h"

GST_DEBUG_CATEGORY_STATIC (ssa_parse_debug);
#define GST_CAT_DEFAULT ssa_parse_debug

static GstStaticPadTemplate sink_templ = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-ssa; application/x-ass")
    );

static GstStaticPadTemplate src_templ = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("text/x-pango-markup")
    );

GST_BOILERPLATE (GstSsaParse, gst_ssa_parse, GstElement, GST_TYPE_ELEMENT);

static GstStateChangeReturn gst_ssa_parse_change_state (GstElement *
    element, GstStateChange transition);
static gboolean gst_ssa_parse_setcaps (GstPad * sinkpad, GstCaps * caps);
static gboolean gst_ssa_parse_src_event (GstPad * pad, GstEvent * event);
static gboolean gst_ssa_parse_sink_event (GstPad * pad, GstEvent * event);
static GstFlowReturn gst_ssa_parse_chain (GstPad * sinkpad, GstBuffer * buf);


static void
gst_ssa_parse_base_init (gpointer klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  static GstElementDetails ssa_parse_details = {
    "SSA Subtitle Parser",
    "Codec/Parser/Subtitle",
    "Parses SSA subtitle streams",
    "Tim-Philipp Müller <tim centricular net>"
  };

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&sink_templ));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_templ));
  gst_element_class_set_details (element_class, &ssa_parse_details);

  GST_DEBUG_CATEGORY_INIT (ssa_parse_debug, "ssaparse", 0,
      "SSA subtitle parser");
}

static void
gst_ssa_parse_dispose (GObject * object)
{
  GstSsaParse *parse = GST_SSA_PARSE (object);

  g_free (parse->ini);
  parse->ini = NULL;

  GST_CALL_PARENT (G_OBJECT_CLASS, dispose, (object));
}

static void
gst_ssa_parse_init (GstSsaParse * parse, GstSsaParseClass * klass)
{
  parse->sinkpad = gst_pad_new_from_static_template (&sink_templ, "sink");
  gst_pad_set_setcaps_function (parse->sinkpad,
      GST_DEBUG_FUNCPTR (gst_ssa_parse_setcaps));
  gst_pad_set_chain_function (parse->sinkpad,
      GST_DEBUG_FUNCPTR (gst_ssa_parse_chain));
  gst_pad_set_event_function (parse->sinkpad,
      GST_DEBUG_FUNCPTR (gst_ssa_parse_sink_event));
  gst_element_add_pad (GST_ELEMENT (parse), parse->sinkpad);

  parse->srcpad = gst_pad_new_from_static_template (&src_templ, "src");
  gst_pad_set_event_function (parse->srcpad,
      GST_DEBUG_FUNCPTR (gst_ssa_parse_src_event));
  gst_element_add_pad (GST_ELEMENT (parse), parse->srcpad);
  gst_pad_use_fixed_caps (parse->srcpad);
  gst_pad_set_caps (parse->srcpad,
      gst_static_pad_template_get_caps (&src_templ));

  parse->ini = NULL;
  parse->framed = FALSE;
}

static void
gst_ssa_parse_class_init (GstSsaParseClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  object_class->dispose = GST_DEBUG_FUNCPTR (gst_ssa_parse_dispose);

  element_class->change_state = GST_DEBUG_FUNCPTR (gst_ssa_parse_change_state);
}

static gboolean
gst_ssa_parse_src_event (GstPad * pad, GstEvent * event)
{
  return gst_pad_event_default (pad, event);
}

static gboolean
gst_ssa_parse_sink_event (GstPad * pad, GstEvent * event)
{
  return gst_pad_event_default (pad, event);
}

static gboolean
gst_ssa_parse_setcaps (GstPad * sinkpad, GstCaps * caps)
{
  GstSsaParse *parse = GST_SSA_PARSE (GST_PAD_PARENT (sinkpad));
  const GValue *val;
  GstStructure *s;
  const guchar bom_utf8[] = { 0xEF, 0xBB, 0xBF };
  GstBuffer *priv;
  gchar *data;
  guint size;

  s = gst_caps_get_structure (caps, 0);
  val = gst_structure_get_value (s, "codec_data");
  if (val == NULL) {
    parse->framed = FALSE;
    GST_ERROR ("Only SSA subtitles embedded in containers are supported");
    return FALSE;
  }

  parse->framed = TRUE;

  priv = (GstBuffer *) gst_value_get_mini_object (val);
  g_return_val_if_fail (priv != NULL, FALSE);

  gst_buffer_ref (priv);

  data = (gchar *) GST_BUFFER_DATA (priv);
  size = GST_BUFFER_SIZE (priv);
  /* skip UTF-8 BOM */
  if (size >= 3 && memcmp (data, bom_utf8, 3) == 0) {
    data += 3;
    size -= 3;
  }

  if (!strstr (data, "[Script Info]")) {
    GST_WARNING_OBJECT (parse, "Invalid Init section - no Script Info header");
    gst_buffer_unref (priv);
    return FALSE;
  }

  if (!g_utf8_validate (data, size, NULL)) {
    GST_WARNING_OBJECT (parse, "Init section is not valid UTF-8");
    gst_buffer_unref (priv);
    return FALSE;
  }

  /* FIXME: parse initial section */
  parse->ini = g_strndup (data, size);
  GST_LOG_OBJECT (parse, "Init section:\n%s", parse->ini);

  gst_buffer_unref (priv);

  return TRUE;
}

static gboolean
gst_ssa_parse_remove_override_codes (GstSsaParse * parse, gchar * txt)
{
  gchar *t, *end;
  gboolean removed_any = FALSE;

  while ((t = strchr (txt, '{'))) {
    end = strchr (txt, '}');
    if (end == NULL) {
      GST_WARNING_OBJECT (parse, "Missing { for style override code");
      return removed_any;
    }
    /* move terminating NUL character forward as well */
    g_memmove (t, end + 1, strlen (end + 1) + 1);
    removed_any = TRUE;
  }

  /* these may occur outside of curly brackets. We don't handle the different
   * wrapping modes yet, so just remove these markers from the text for now */
  while ((t = strstr (txt, "\\n"))) {
    t[0] = ' ';
    t[1] = '\n';
  }
  while ((t = strstr (txt, "\\N"))) {
    t[0] = ' ';
    t[1] = '\n';
  }
  while ((t = strstr (txt, "\\h"))) {
    t[0] = ' ';
    t[1] = ' ';
  }

  return removed_any;
}

/**
 * gst_ssa_parse_push_line:
 * @parse: caller element
 * @txt: text to push
 * @start: timestamp for the buffer
 * @duration: duration for the buffer
 *
 * Parse the text in a buffer with the given properties and
 * push it to the srcpad of the @parse element
 *
 * Returns: result of the push of the created buffer
 */
static GstFlowReturn
gst_ssa_parse_push_line (GstSsaParse * parse, gchar * txt,
    GstClockTime start, GstClockTime duration)
{
  GstFlowReturn ret;
  GstBuffer *buf;
  gchar *t, *escaped;
  gint num, i, len;

  num = atoi (txt);
  GST_LOG_OBJECT (parse, "Parsing line #%d at %" GST_TIME_FORMAT,
      num, GST_TIME_ARGS (start));

  /* skip all non-text fields before the actual text */
  t = txt;
  for (i = 0; i < 8; ++i) {
    t = strchr (t, ',');
    if (t == NULL)
      return GST_FLOW_ERROR;
    ++t;
  }

  GST_LOG_OBJECT (parse, "Text : %s", t);

  if (gst_ssa_parse_remove_override_codes (parse, t)) {
    GST_LOG_OBJECT (parse, "Clean: %s", t);
  }

  /* we claim to output pango markup, so we must escape the
   * text even if we don't actually use any pango markup yet */
  escaped = g_markup_printf_escaped ("%s", t);

  len = strlen (escaped);

  /* allocate enough for a terminating NUL, but don't include it in buf size */
  buf = gst_buffer_new_and_alloc (len + 1);
  memcpy (GST_BUFFER_DATA (buf), escaped, len + 1);
  GST_BUFFER_SIZE (buf) = len;
  g_free (escaped);

  GST_BUFFER_TIMESTAMP (buf) = start;
  GST_BUFFER_DURATION (buf) = duration;

  gst_buffer_set_caps (buf, GST_PAD_CAPS (parse->srcpad));

  GST_LOG_OBJECT (parse, "Pushing buffer with timestamp %" GST_TIME_FORMAT
      " and duration %" GST_TIME_FORMAT, GST_TIME_ARGS (start),
      GST_TIME_ARGS (duration));

  ret = gst_pad_push (parse->srcpad, buf);

  if (ret != GST_FLOW_OK) {
    GST_DEBUG_OBJECT (parse, "Push of text '%s' returned flow %s", txt,
        gst_flow_get_name (ret));
  }

  return ret;
}

static GstFlowReturn
gst_ssa_parse_chain (GstPad * sinkpad, GstBuffer * buf)
{
  GstFlowReturn ret;
  GstSsaParse *parse = GST_SSA_PARSE (GST_PAD_PARENT (sinkpad));
  GstClockTime ts;
  gchar *txt;

  if (G_UNLIKELY (!parse->framed))
    goto not_framed;

  /* make double-sure it's 0-terminated and all */
  txt = g_strndup ((gchar *) GST_BUFFER_DATA (buf), GST_BUFFER_SIZE (buf));

  if (txt == NULL)
    return GST_FLOW_UNEXPECTED;

  ts = GST_BUFFER_TIMESTAMP (buf);
  ret = gst_ssa_parse_push_line (parse, txt, ts, GST_BUFFER_DURATION (buf));

  if (ret != GST_FLOW_OK && GST_CLOCK_TIME_IS_VALID (ts)) {
    /* just advance time without sending anything */
    gst_pad_push_event (parse->srcpad,
        gst_event_new_new_segment (TRUE, 1.0, GST_FORMAT_TIME, ts, -1, ts));
    ret = GST_FLOW_OK;
  }

  g_free (txt);

  return ret;

/* ERRORS */
not_framed:
  {
    GST_ELEMENT_ERROR (parse, STREAM, FORMAT, (NULL),
        ("Only SSA subtitles embedded in containers are supported"));
    return GST_FLOW_NOT_NEGOTIATED;
  }
}

static GstStateChangeReturn
gst_ssa_parse_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  GstSsaParse *parse = GST_SSA_PARSE (element);

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      break;
    default:
      break;
  }

  ret = parent_class->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      g_free (parse->ini);
      parse->ini = NULL;
      parse->framed = FALSE;
      break;
    default:
      break;
  }

  return ret;
}
