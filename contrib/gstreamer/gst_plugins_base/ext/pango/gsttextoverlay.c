/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2003> David Schleef <ds@schleef.org>
 * Copyright (C) <2006> Julien Moutte <julien@moutte.net>
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
 * SECTION:element-textoverlay
 * @see_also: #GstTextRender, #GstClockOverlay, #GstTimeOverlay, #GstSubParse
 *
 * <refsect2>
 * <para>
 * This plugin renders text on top of a video stream. This can be either
 * static text or text from buffers received on the text sink pad, e.g.
 * as produced by the subparse element. If the text sink pad is not linked,
 * the text set via the "text" property will be rendered. If the text sink
 * pad is linked, text will be rendered as it is received on that pad,
 * honouring and matching the buffer timestamps of both input streams.
 * </para>
 * <para>
 * The text can contain newline characters and text wrapping is enabled by
 * default.
 * </para>
 * <para>
 * Here is a simple pipeline that displays a static text in the top left
 * corner of the video picture:
 * <programlisting>
 * gst-launch -v videotestsrc ! textoverlay text="Room A" valign=top halign=left ! xvimagesink
 * </programlisting>
 * </para>
 * <para>
 * Here is another pipeline that displays subtitles from an .srt subtitle
 * file, centered at the bottom of the picture and with a rectangular shading
 * around the text in the background:
 * <programlisting>
 * gst-launch -v filesrc location=subtitles.srt ! subparse ! txt.   videotestsrc ! timeoverlay ! textoverlay name=txt shaded-background=yes ! xvimagesink
 * </programlisting>
 * If you do not have such a subtitle file, create one looking like this
 * in a text editor:
 * <programlisting>
 * 1
 * 00:00:03,000 --> 00:00:05,000
 * Hello? (3-5s)
 *  
 * 2
 * 00:00:08,000 --> 00:00:13,000
 * Yes, this is a subtitle. Don&apos;t
 * you like it? (8-13s)
 *  
 * 3
 * 00:00:18,826 --> 00:01:02,886
 * Uh? What are you talking about?
 * I don&apos;t understand  (18-62s)
 * </programlisting>
 * </para>
 * </refsect2>
 */

/* FIXME: alloc segment as part of instance struct */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/video/video.h>

#include "gsttextoverlay.h"
#include "gsttimeoverlay.h"
#include "gstclockoverlay.h"
#include "gsttextrender.h"

/* FIXME:
 *  - use proper strides and offset for I420
 *  - if text is wider than the video picture, it does not get
 *    clipped properly during blitting (if wrapping is disabled)
 *  - make 'shading_value' a property (or enum:  light/normal/dark/verydark)?
 */

GST_DEBUG_CATEGORY (pango_debug);
#define GST_CAT_DEFAULT pango_debug

static const GstElementDetails text_overlay_details =
GST_ELEMENT_DETAILS ("Text overlay",
    "Filter/Editor/Video",
    "Adds text strings on top of a video buffer",
    "David Schleef <ds@schleef.org>");


#define DEFAULT_PROP_TEXT 	""
#define DEFAULT_PROP_SHADING	FALSE
#define DEFAULT_PROP_VALIGNMENT	GST_TEXT_OVERLAY_VALIGN_BASELINE
#define DEFAULT_PROP_HALIGNMENT	GST_TEXT_OVERLAY_HALIGN_CENTER
#define DEFAULT_PROP_VALIGN	"baseline"
#define DEFAULT_PROP_HALIGN	"center"
#define DEFAULT_PROP_XPAD	25
#define DEFAULT_PROP_YPAD	25
#define DEFAULT_PROP_DELTAX	0
#define DEFAULT_PROP_DELTAY	0
#define DEFAULT_PROP_WRAP_MODE  GST_TEXT_OVERLAY_WRAP_MODE_WORD_CHAR
#define DEFAULT_PROP_FONT_DESC	""
#define DEFAULT_PROP_SILENT	FALSE
#define DEFAULT_PROP_LINE_ALIGNMENT GST_TEXT_OVERLAY_LINE_ALIGN_CENTER

/* make a property of me */
#define DEFAULT_SHADING_VALUE    -80

enum
{
  PROP_0,
  PROP_TEXT,
  PROP_SHADING,
  PROP_VALIGN,                  /* deprecated */
  PROP_HALIGN,                  /* deprecated */
  PROP_HALIGNMENT,
  PROP_VALIGNMENT,
  PROP_XPAD,
  PROP_YPAD,
  PROP_DELTAX,
  PROP_DELTAY,
  PROP_WRAP_MODE,
  PROP_FONT_DESC,
  PROP_SILENT,
  PROP_LINE_ALIGNMENT
};


static GstStaticPadTemplate src_template_factory =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_YUV ("I420"))
    );

static GstStaticPadTemplate video_sink_template_factory =
GST_STATIC_PAD_TEMPLATE ("video_sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_YUV ("I420"))
    );

static GstStaticPadTemplate text_sink_template_factory =
    GST_STATIC_PAD_TEMPLATE ("text_sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("text/x-pango-markup; text/plain")
    );


#define GST_TYPE_TEXT_OVERLAY_VALIGN (gst_text_overlay_valign_get_type())
static GType
gst_text_overlay_valign_get_type (void)
{
  static GType text_overlay_valign_type = 0;
  static const GEnumValue text_overlay_valign[] = {
    {GST_TEXT_OVERLAY_VALIGN_BASELINE, "baseline", "baseline"},
    {GST_TEXT_OVERLAY_VALIGN_BOTTOM, "bottom", "bottom"},
    {GST_TEXT_OVERLAY_VALIGN_TOP, "top", "top"},
    {0, NULL, NULL},
  };

  if (!text_overlay_valign_type) {
    text_overlay_valign_type =
        g_enum_register_static ("GstTextOverlayVAlign", text_overlay_valign);
  }
  return text_overlay_valign_type;
}

#define GST_TYPE_TEXT_OVERLAY_HALIGN (gst_text_overlay_halign_get_type())
static GType
gst_text_overlay_halign_get_type (void)
{
  static GType text_overlay_halign_type = 0;
  static const GEnumValue text_overlay_halign[] = {
    {GST_TEXT_OVERLAY_HALIGN_LEFT, "left", "left"},
    {GST_TEXT_OVERLAY_HALIGN_CENTER, "center", "center"},
    {GST_TEXT_OVERLAY_HALIGN_RIGHT, "right", "right"},
    {0, NULL, NULL},
  };

  if (!text_overlay_halign_type) {
    text_overlay_halign_type =
        g_enum_register_static ("GstTextOverlayHAlign", text_overlay_halign);
  }
  return text_overlay_halign_type;
}


#define GST_TYPE_TEXT_OVERLAY_WRAP_MODE (gst_text_overlay_wrap_mode_get_type())
static GType
gst_text_overlay_wrap_mode_get_type (void)
{
  static GType text_overlay_wrap_mode_type = 0;
  static const GEnumValue text_overlay_wrap_mode[] = {
    {GST_TEXT_OVERLAY_WRAP_MODE_NONE, "none", "none"},
    {GST_TEXT_OVERLAY_WRAP_MODE_WORD, "word", "word"},
    {GST_TEXT_OVERLAY_WRAP_MODE_CHAR, "char", "char"},
    {GST_TEXT_OVERLAY_WRAP_MODE_WORD_CHAR, "wordchar", "wordchar"},
    {0, NULL, NULL},
  };

  if (!text_overlay_wrap_mode_type) {
    text_overlay_wrap_mode_type =
        g_enum_register_static ("GstTextOverlayWrapMode",
        text_overlay_wrap_mode);
  }
  return text_overlay_wrap_mode_type;
}

#define GST_TYPE_TEXT_OVERLAY_LINE_ALIGN (gst_text_overlay_line_align_get_type())
static GType
gst_text_overlay_line_align_get_type (void)
{
  static GType text_overlay_line_align_type = 0;
  static const GEnumValue text_overlay_line_align[] = {
    {GST_TEXT_OVERLAY_LINE_ALIGN_LEFT, "left", "left"},
    {GST_TEXT_OVERLAY_LINE_ALIGN_CENTER, "center", "center"},
    {GST_TEXT_OVERLAY_LINE_ALIGN_RIGHT, "right", "right"},
    {0, NULL, NULL}
  };

  if (!text_overlay_line_align_type) {
    text_overlay_line_align_type =
        g_enum_register_static ("GstTextOverlayLineAlign",
        text_overlay_line_align);
  }
  return text_overlay_line_align_type;
}

/* These macros are adapted from videotestsrc.c */
#define I420_Y_ROWSTRIDE(width) (GST_ROUND_UP_4(width))
#define I420_U_ROWSTRIDE(width) (GST_ROUND_UP_8(width)/2)
#define I420_V_ROWSTRIDE(width) ((GST_ROUND_UP_8(I420_Y_ROWSTRIDE(width)))/2)

#define I420_Y_OFFSET(w,h) (0)
#define I420_U_OFFSET(w,h) (I420_Y_OFFSET(w,h)+(I420_Y_ROWSTRIDE(w)*GST_ROUND_UP_2(h)))
#define I420_V_OFFSET(w,h) (I420_U_OFFSET(w,h)+(I420_U_ROWSTRIDE(w)*GST_ROUND_UP_2(h)/2))

#define I420_SIZE(w,h)     (I420_V_OFFSET(w,h)+(I420_V_ROWSTRIDE(w)*GST_ROUND_UP_2(h)/2))

#define GST_TEXT_OVERLAY_GET_COND(ov) (((GstTextOverlay *)ov)->cond)
#define GST_TEXT_OVERLAY_WAIT(ov)     (g_cond_wait (GST_TEXT_OVERLAY_GET_COND (ov), GST_OBJECT_GET_LOCK (ov)))
#define GST_TEXT_OVERLAY_SIGNAL(ov)   (g_cond_signal (GST_TEXT_OVERLAY_GET_COND (ov)))
#define GST_TEXT_OVERLAY_BROADCAST(ov)(g_cond_broadcast (GST_TEXT_OVERLAY_GET_COND (ov)))

static GstStateChangeReturn gst_text_overlay_change_state (GstElement * element,
    GstStateChange transition);

static GstCaps *gst_text_overlay_getcaps (GstPad * pad);
static gboolean gst_text_overlay_setcaps (GstPad * pad, GstCaps * caps);
static gboolean gst_text_overlay_setcaps_txt (GstPad * pad, GstCaps * caps);
static gboolean gst_text_overlay_src_event (GstPad * pad, GstEvent * event);

static gboolean gst_text_overlay_video_event (GstPad * pad, GstEvent * event);
static GstFlowReturn gst_text_overlay_video_chain (GstPad * pad,
    GstBuffer * buffer);

static gboolean gst_text_overlay_text_event (GstPad * pad, GstEvent * event);
static GstFlowReturn gst_text_overlay_text_chain (GstPad * pad,
    GstBuffer * buffer);
static GstPadLinkReturn gst_text_overlay_text_pad_link (GstPad * pad,
    GstPad * peer);
static void gst_text_overlay_text_pad_unlink (GstPad * pad);
static void gst_text_overlay_pop_text (GstTextOverlay * overlay);

static void gst_text_overlay_finalize (GObject * object);
static void gst_text_overlay_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_text_overlay_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

GST_BOILERPLATE (GstTextOverlay, gst_text_overlay, GstElement, GST_TYPE_ELEMENT)

     static void gst_text_overlay_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_template_factory));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&video_sink_template_factory));

  /* ugh */
  if (!GST_IS_TIME_OVERLAY_CLASS (g_class) &&
      !GST_IS_CLOCK_OVERLAY_CLASS (g_class)) {
    gst_element_class_add_pad_template (element_class,
        gst_static_pad_template_get (&text_sink_template_factory));
  }

  gst_element_class_set_details (element_class, &text_overlay_details);
}

static gchar *
gst_text_overlay_get_text (GstTextOverlay * overlay, GstBuffer * video_frame)
{
  return g_strdup (overlay->default_text);
}

static void
gst_text_overlay_class_init (GstTextOverlayClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->finalize = gst_text_overlay_finalize;
  gobject_class->set_property = gst_text_overlay_set_property;
  gobject_class->get_property = gst_text_overlay_get_property;

  gstelement_class->change_state =
      GST_DEBUG_FUNCPTR (gst_text_overlay_change_state);

  klass->get_text = gst_text_overlay_get_text;
  klass->pango_context = pango_ft2_get_context (72, 72);

  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_TEXT,
      g_param_spec_string ("text", "text",
          "Text to be display.", DEFAULT_PROP_TEXT, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_SHADING,
      g_param_spec_boolean ("shaded-background", "shaded background",
          "Whether to shade the background under the text area",
          DEFAULT_PROP_SHADING, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_VALIGNMENT,
      g_param_spec_enum ("valignment", "vertical alignment",
          "Vertical alignment of the text",
          GST_TYPE_TEXT_OVERLAY_VALIGN, DEFAULT_PROP_VALIGNMENT,
          G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_HALIGNMENT,
      g_param_spec_enum ("halignment", "horizontal alignment",
          "Horizontal alignment of the text", GST_TYPE_TEXT_OVERLAY_HALIGN,
          DEFAULT_PROP_HALIGNMENT, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_VALIGN,
      g_param_spec_string ("valign", "vertical alignment",
          "Vertical alignment of the text (deprecated; use valignment)",
          DEFAULT_PROP_VALIGN, G_PARAM_WRITABLE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_HALIGN,
      g_param_spec_string ("halign", "horizontal alignment",
          "Horizontal alignment of the text (deprecated; use halignment)",
          DEFAULT_PROP_HALIGN, G_PARAM_WRITABLE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_XPAD,
      g_param_spec_int ("xpad", "horizontal paddding",
          "Horizontal paddding when using left/right alignment", 0, G_MAXINT,
          DEFAULT_PROP_XPAD, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_YPAD,
      g_param_spec_int ("ypad", "vertical padding",
          "Vertical padding when using top/bottom alignment", 0, G_MAXINT,
          DEFAULT_PROP_YPAD, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_DELTAX,
      g_param_spec_int ("deltax", "X position modifier",
          "Shift X position to the left or to the right. Unit is pixels.",
          G_MININT, G_MAXINT, DEFAULT_PROP_DELTAX, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_DELTAY,
      g_param_spec_int ("deltay", "Y position modifier",
          "Shift Y position up or down. Unit is pixels.", G_MININT, G_MAXINT,
          DEFAULT_PROP_DELTAY, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_WRAP_MODE,
      g_param_spec_enum ("wrap-mode", "wrap mode",
          "Whether to wrap the text and if so how.",
          GST_TYPE_TEXT_OVERLAY_WRAP_MODE, DEFAULT_PROP_WRAP_MODE,
          G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_FONT_DESC,
      g_param_spec_string ("font-desc", "font description",
          "Pango font description of font to be used for rendering. "
          "See documentation of pango_font_description_from_string "
          "for syntax.", DEFAULT_PROP_FONT_DESC, G_PARAM_WRITABLE));
  /**
   * GstTextOverlay:line-alignment
   *
   * Alignment of text lines relative to each other (for multi-line text)
   *
   * Since: 0.10.15
   **/
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_LINE_ALIGNMENT,
      g_param_spec_enum ("line-alignment", "line alignment",
          "Alignment of text lines relative to each other.",
          GST_TYPE_TEXT_OVERLAY_LINE_ALIGN, DEFAULT_PROP_LINE_ALIGNMENT,
          G_PARAM_READWRITE));
  /**
   * GstTextOverlay:silent
   *
   * If set, no text is rendered. Useful to switch off text rendering
   * temporarily without removing the textoverlay element from the pipeline.
   *
   * Since: 0.10.15
   **/
  /* FIXME 0.11: rename to "visible" or "text-visible" or "render-text" */
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_SILENT,
      g_param_spec_boolean ("silent", "silent",
          "Whether to render the text string",
          DEFAULT_PROP_SILENT, G_PARAM_READWRITE));
}

static void
gst_text_overlay_finalize (GObject * object)
{
  GstTextOverlay *overlay = GST_TEXT_OVERLAY (object);

  g_free (overlay->default_text);
  g_free (overlay->bitmap.buffer);

  if (overlay->layout) {
    g_object_unref (overlay->layout);
    overlay->layout = NULL;
  }

  if (overlay->segment) {
    gst_segment_free (overlay->segment);
    overlay->segment = NULL;
  }

  if (overlay->text_buffer) {
    gst_buffer_unref (overlay->text_buffer);
    overlay->text_buffer = NULL;
  }

  if (overlay->cond) {
    g_cond_free (overlay->cond);
    overlay->cond = NULL;
  }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_text_overlay_init (GstTextOverlay * overlay, GstTextOverlayClass * klass)
{
  GstPadTemplate *template;

  /* video sink */
  template = gst_static_pad_template_get (&video_sink_template_factory);
  overlay->video_sinkpad = gst_pad_new_from_template (template, "video_sink");
  gst_object_unref (template);
  gst_pad_set_getcaps_function (overlay->video_sinkpad,
      GST_DEBUG_FUNCPTR (gst_text_overlay_getcaps));
  gst_pad_set_setcaps_function (overlay->video_sinkpad,
      GST_DEBUG_FUNCPTR (gst_text_overlay_setcaps));
  gst_pad_set_event_function (overlay->video_sinkpad,
      GST_DEBUG_FUNCPTR (gst_text_overlay_video_event));
  gst_pad_set_chain_function (overlay->video_sinkpad,
      GST_DEBUG_FUNCPTR (gst_text_overlay_video_chain));
  gst_element_add_pad (GST_ELEMENT (overlay), overlay->video_sinkpad);

  if (!GST_IS_TIME_OVERLAY_CLASS (klass) && !GST_IS_CLOCK_OVERLAY_CLASS (klass)) {
    /* text sink */
    template = gst_static_pad_template_get (&text_sink_template_factory);
    overlay->text_sinkpad = gst_pad_new_from_template (template, "text_sink");
    gst_object_unref (template);
    gst_pad_set_setcaps_function (overlay->text_sinkpad,
        GST_DEBUG_FUNCPTR (gst_text_overlay_setcaps_txt));
    gst_pad_set_event_function (overlay->text_sinkpad,
        GST_DEBUG_FUNCPTR (gst_text_overlay_text_event));
    gst_pad_set_chain_function (overlay->text_sinkpad,
        GST_DEBUG_FUNCPTR (gst_text_overlay_text_chain));
    gst_pad_set_link_function (overlay->text_sinkpad,
        GST_DEBUG_FUNCPTR (gst_text_overlay_text_pad_link));
    gst_pad_set_unlink_function (overlay->text_sinkpad,
        GST_DEBUG_FUNCPTR (gst_text_overlay_text_pad_unlink));
    gst_element_add_pad (GST_ELEMENT (overlay), overlay->text_sinkpad);
  }

  /* (video) source */
  template = gst_static_pad_template_get (&src_template_factory);
  overlay->srcpad = gst_pad_new_from_template (template, "src");
  gst_object_unref (template);
  gst_pad_set_getcaps_function (overlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_text_overlay_getcaps));
  gst_pad_set_event_function (overlay->srcpad,
      GST_DEBUG_FUNCPTR (gst_text_overlay_src_event));
  gst_element_add_pad (GST_ELEMENT (overlay), overlay->srcpad);

  overlay->line_align = DEFAULT_PROP_LINE_ALIGNMENT;
  overlay->layout =
      pango_layout_new (GST_TEXT_OVERLAY_GET_CLASS (overlay)->pango_context);
  pango_layout_set_alignment (overlay->layout,
      (PangoAlignment) overlay->line_align);
  memset (&overlay->bitmap, 0, sizeof (overlay->bitmap));

  overlay->halign = DEFAULT_PROP_HALIGNMENT;
  overlay->valign = DEFAULT_PROP_VALIGNMENT;
  overlay->xpad = DEFAULT_PROP_XPAD;
  overlay->ypad = DEFAULT_PROP_YPAD;
  overlay->deltax = DEFAULT_PROP_DELTAX;
  overlay->deltay = DEFAULT_PROP_DELTAY;

  overlay->wrap_mode = DEFAULT_PROP_WRAP_MODE;

  overlay->want_shading = DEFAULT_PROP_SHADING;
  overlay->shading_value = DEFAULT_SHADING_VALUE;
  overlay->silent = DEFAULT_PROP_SILENT;

  overlay->default_text = g_strdup (DEFAULT_PROP_TEXT);
  overlay->need_render = TRUE;

  overlay->fps_n = 0;
  overlay->fps_d = 1;

  overlay->text_buffer = NULL;
  overlay->text_linked = FALSE;
  overlay->video_flushing = FALSE;
  overlay->text_flushing = FALSE;
  overlay->text_eos = FALSE;
  overlay->cond = g_cond_new ();
  overlay->segment = gst_segment_new ();
  if (overlay->segment) {
    gst_segment_init (overlay->segment, GST_FORMAT_TIME);
  } else {
    GST_WARNING_OBJECT (overlay, "segment creation failed");
    g_assert_not_reached ();
  }
}

static void
gst_text_overlay_update_wrap_mode (GstTextOverlay * overlay)
{
  if (overlay->wrap_mode == GST_TEXT_OVERLAY_WRAP_MODE_NONE) {
    GST_DEBUG_OBJECT (overlay, "Set wrap mode NONE");
    pango_layout_set_width (overlay->layout, -1);
  } else {
    GST_DEBUG_OBJECT (overlay, "Set layout width %d", overlay->width);
    GST_DEBUG_OBJECT (overlay, "Set wrap mode    %d", overlay->wrap_mode);
    pango_layout_set_width (overlay->layout, overlay->width * PANGO_SCALE);
    pango_layout_set_wrap (overlay->layout, (PangoWrapMode) overlay->wrap_mode);
  }
}

static gboolean
gst_text_overlay_setcaps_txt (GstPad * pad, GstCaps * caps)
{
  GstTextOverlay *overlay;
  GstStructure *structure;

  overlay = GST_TEXT_OVERLAY (gst_pad_get_parent (pad));

  structure = gst_caps_get_structure (caps, 0);
  overlay->have_pango_markup =
      gst_structure_has_name (structure, "text/x-pango-markup");

  gst_object_unref (overlay);

  return TRUE;
}

/* FIXME: upstream nego (e.g. when the video window is resized) */

static gboolean
gst_text_overlay_setcaps (GstPad * pad, GstCaps * caps)
{
  GstTextOverlay *overlay;
  GstStructure *structure;
  gboolean ret = FALSE;
  const GValue *fps;

  if (!GST_PAD_IS_SINK (pad))
    return TRUE;

  g_return_val_if_fail (gst_caps_is_fixed (caps), FALSE);

  overlay = GST_TEXT_OVERLAY (gst_pad_get_parent (pad));

  overlay->width = 0;
  overlay->height = 0;
  structure = gst_caps_get_structure (caps, 0);
  fps = gst_structure_get_value (structure, "framerate");

  if (gst_structure_get_int (structure, "width", &overlay->width) &&
      gst_structure_get_int (structure, "height", &overlay->height) &&
      fps != NULL) {
    ret = gst_pad_set_caps (overlay->srcpad, caps);
  }

  overlay->fps_n = gst_value_get_fraction_numerator (fps);
  overlay->fps_d = gst_value_get_fraction_denominator (fps);

  if (ret) {
    GST_OBJECT_LOCK (overlay);
    gst_text_overlay_update_wrap_mode (overlay);
    GST_OBJECT_UNLOCK (overlay);
  }

  gst_object_unref (overlay);

  return ret;
}

static void
gst_text_overlay_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstTextOverlay *overlay = GST_TEXT_OVERLAY (object);

  GST_OBJECT_LOCK (overlay);
  switch (prop_id) {
    case PROP_TEXT:
      g_free (overlay->default_text);
      overlay->default_text = g_value_dup_string (value);
      overlay->need_render = TRUE;
      break;
    case PROP_SHADING:
      overlay->want_shading = g_value_get_boolean (value);
      break;
    case PROP_XPAD:
      overlay->xpad = g_value_get_int (value);
      break;
    case PROP_YPAD:
      overlay->ypad = g_value_get_int (value);
      break;
    case PROP_DELTAX:
      overlay->deltax = g_value_get_int (value);
      break;
    case PROP_DELTAY:
      overlay->deltay = g_value_get_int (value);
      break;
    case PROP_HALIGN:{
      const gchar *s = g_value_get_string (value);

      if (s && g_ascii_strcasecmp (s, "left") == 0)
        overlay->halign = GST_TEXT_OVERLAY_HALIGN_LEFT;
      else if (s && g_ascii_strcasecmp (s, "center") == 0)
        overlay->halign = GST_TEXT_OVERLAY_HALIGN_CENTER;
      else if (s && g_ascii_strcasecmp (s, "right") == 0)
        overlay->halign = GST_TEXT_OVERLAY_HALIGN_RIGHT;
      else
        g_warning ("Invalid value '%s' for textoverlay property 'halign'",
            GST_STR_NULL (s));
      break;
    }
    case PROP_VALIGN:{
      const gchar *s = g_value_get_string (value);

      if (s && g_ascii_strcasecmp (s, "baseline") == 0)
        overlay->valign = GST_TEXT_OVERLAY_VALIGN_BASELINE;
      else if (s && g_ascii_strcasecmp (s, "bottom") == 0)
        overlay->valign = GST_TEXT_OVERLAY_VALIGN_BOTTOM;
      else if (s && g_ascii_strcasecmp (s, "top") == 0)
        overlay->valign = GST_TEXT_OVERLAY_VALIGN_TOP;
      else
        g_warning ("Invalid value '%s' for textoverlay property 'valign'",
            GST_STR_NULL (s));
      break;
    }
    case PROP_VALIGNMENT:
      overlay->valign = g_value_get_enum (value);
      break;
    case PROP_HALIGNMENT:
      overlay->halign = g_value_get_enum (value);
      break;
    case PROP_WRAP_MODE:
      overlay->wrap_mode = g_value_get_enum (value);
      gst_text_overlay_update_wrap_mode (overlay);
      break;
    case PROP_FONT_DESC:
    {
      PangoFontDescription *desc;
      const gchar *fontdesc_str;

      fontdesc_str = g_value_get_string (value);
      desc = pango_font_description_from_string (fontdesc_str);
      if (desc) {
        GST_LOG_OBJECT (overlay, "font description set: %s", fontdesc_str);
        pango_layout_set_font_description (overlay->layout, desc);
        pango_font_description_free (desc);
      } else {
        GST_WARNING_OBJECT (overlay, "font description parse failed: %s",
            fontdesc_str);
      }
      break;
    }
    case PROP_SILENT:
      overlay->silent = g_value_get_boolean (value);
      break;
    case PROP_LINE_ALIGNMENT:
      overlay->line_align = g_value_get_enum (value);
      pango_layout_set_alignment (overlay->layout,
          (PangoAlignment) overlay->line_align);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  overlay->need_render = TRUE;
  GST_OBJECT_UNLOCK (overlay);
}

static void
gst_text_overlay_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstTextOverlay *overlay = GST_TEXT_OVERLAY (object);

  GST_OBJECT_LOCK (overlay);
  switch (prop_id) {
    case PROP_TEXT:
      g_value_set_string (value, overlay->default_text);
      break;
    case PROP_SHADING:
      g_value_set_boolean (value, overlay->want_shading);
      break;
    case PROP_XPAD:
      g_value_set_int (value, overlay->xpad);
      break;
    case PROP_YPAD:
      g_value_set_int (value, overlay->ypad);
      break;
    case PROP_DELTAX:
      g_value_set_int (value, overlay->deltax);
      break;
    case PROP_DELTAY:
      g_value_set_int (value, overlay->deltay);
      break;
    case PROP_VALIGNMENT:
      g_value_set_enum (value, overlay->valign);
      break;
    case PROP_HALIGNMENT:
      g_value_set_enum (value, overlay->halign);
      break;
    case PROP_WRAP_MODE:
      g_value_set_enum (value, overlay->wrap_mode);
      break;
    case PROP_SILENT:
      g_value_set_boolean (value, overlay->silent);
      break;
    case PROP_LINE_ALIGNMENT:
      g_value_set_enum (value, overlay->line_align);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  overlay->need_render = TRUE;
  GST_OBJECT_UNLOCK (overlay);
}

static gboolean
gst_text_overlay_src_event (GstPad * pad, GstEvent * event)
{
  gboolean ret = FALSE;
  GstTextOverlay *overlay = NULL;

  overlay = GST_TEXT_OVERLAY (gst_pad_get_parent (pad));

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEEK:
      /* We don't handle seek if we have not text pad */
      if (!overlay->text_linked) {
        ret = gst_pad_push_event (overlay->video_sinkpad, event);
        goto beach;
      }

      GST_DEBUG_OBJECT (overlay, "seek received, driving from here");

      /* Flush downstream, FIXME, only for flushing seek */
      gst_pad_push_event (overlay->srcpad, gst_event_new_flush_start ());

      /* Mark ourself as flushing, unblock chains */
      GST_OBJECT_LOCK (overlay);
      overlay->video_flushing = TRUE;
      overlay->text_flushing = TRUE;
      gst_text_overlay_pop_text (overlay);
      GST_OBJECT_UNLOCK (overlay);

      /* Seek on each sink pad */
      gst_event_ref (event);
      ret = gst_pad_push_event (overlay->video_sinkpad, event);
      if (ret) {
        ret = gst_pad_push_event (overlay->text_sinkpad, event);
      } else {
        gst_event_unref (event);
      }
      break;
    default:
      if (overlay->text_linked) {
        gst_event_ref (event);
        ret = gst_pad_push_event (overlay->video_sinkpad, event);
        gst_pad_push_event (overlay->text_sinkpad, event);
      } else {
        ret = gst_pad_push_event (overlay->video_sinkpad, event);
      }
      break;
  }

beach:
  gst_object_unref (overlay);

  return ret;
}

static GstCaps *
gst_text_overlay_getcaps (GstPad * pad)
{
  GstTextOverlay *overlay;
  GstPad *otherpad;
  GstCaps *caps;

  overlay = GST_TEXT_OVERLAY (gst_pad_get_parent (pad));

  if (pad == overlay->srcpad)
    otherpad = overlay->video_sinkpad;
  else
    otherpad = overlay->srcpad;

  /* we can do what the peer can */
  caps = gst_pad_peer_get_caps (otherpad);
  if (caps) {
    GstCaps *temp;
    const GstCaps *templ;

    GST_DEBUG_OBJECT (pad, "peer caps  %" GST_PTR_FORMAT, caps);

    /* filtered against our padtemplate */
    templ = gst_pad_get_pad_template_caps (otherpad);
    GST_DEBUG_OBJECT (pad, "our template  %" GST_PTR_FORMAT, templ);
    temp = gst_caps_intersect (caps, templ);
    GST_DEBUG_OBJECT (pad, "intersected %" GST_PTR_FORMAT, temp);
    gst_caps_unref (caps);
    /* this is what we can do */
    caps = temp;
  } else {
    /* no peer, our padtemplate is enough then */
    caps = gst_caps_copy (gst_pad_get_pad_template_caps (pad));
  }

  GST_DEBUG_OBJECT (overlay, "returning  %" GST_PTR_FORMAT, caps);

  gst_object_unref (overlay);

  return caps;
}

#define BOX_XPAD         6
#define BOX_YPAD         6

static inline void
gst_text_overlay_shade_y (GstTextOverlay * overlay, guchar * dest,
    guint dest_stride, gint x0, gint x1, gint y0, gint y1)
{
  gint i, j;

  x0 = CLAMP (x0 - BOX_XPAD, 0, overlay->width);
  x1 = CLAMP (x1 + BOX_XPAD, 0, overlay->width);

  y0 = CLAMP (y0 - BOX_YPAD, 0, overlay->height);
  y1 = CLAMP (y1 + BOX_YPAD, 0, overlay->height);

  for (i = y0; i < y1; ++i) {
    for (j = x0; j < x1; ++j) {
      gint y = dest[(i * dest_stride) + j] + overlay->shading_value;

      dest[(i * dest_stride) + j] = CLAMP (y, 0, 255);
    }
  }
}

/* FIXME:
 *  - use proper strides and offset for I420
 *  - don't draw over the edge of the picture (try a longer
 *    text with a huge font size)
 */

static inline void
gst_text_overlay_blit_yuv420 (GstTextOverlay * overlay, FT_Bitmap * bitmap,
    guint8 * yuv_pixels, gint x0, gint y0)
{
  int y;                        /* text bitmap coordinates */
  int x1, y1;                   /* video buffer coordinates */
  int bit_rowinc, uv_rowinc;
  guint8 *p, *bitp, *u_p;
  int video_width, video_height;
  int bitmap_x0 = 0;            //x0 < 1 ? -(x0 - 1) : 1;       /* 1 pixel border */
  int bitmap_y0 = y0 < 1 ? -(y0 - 1) : 1;       /* 1 pixel border */
  int bitmap_width = bitmap->width - bitmap_x0;
  int bitmap_height = bitmap->rows - bitmap_y0;
  int u_plane_size;
  int skip_y, skip_x;
  guint8 v;

  video_width = I420_Y_ROWSTRIDE (overlay->width);
  video_height = overlay->height;

/*
  if (x0 < 0 && abs (x0) < bitmap_width) {
    bitmap_x0 = abs (x0);
    x0 = 0;
  }
*/

  if (x0 + bitmap_x0 + bitmap_width > overlay->width - 1)       /* 1 pixel border */
    bitmap_width -= x0 + bitmap_x0 + bitmap_width - overlay->width + 1;
  if (y0 + bitmap_y0 + bitmap_height > video_height - 1)        /* 1 pixel border */
    bitmap_height -= y0 + bitmap_y0 + bitmap_height - video_height + 1;

  uv_rowinc = video_width / 2 - bitmap_width / 2;
  bit_rowinc = bitmap->pitch - bitmap_width;
  u_plane_size = (video_width / 2) * (video_height / 2);

  y1 = y0 + bitmap_y0;
  x1 = x0 + bitmap_x0;
  bitp = bitmap->buffer + bitmap->pitch * bitmap_y0 + bitmap_x0;
  for (y = bitmap_y0; y < bitmap_y0 + bitmap_height; y++) {
    int n;

    p = yuv_pixels + (y + y0) * I420_Y_ROWSTRIDE (overlay->width) + x1;
    for (n = bitmap_width; n > 0; --n) {
      v = *bitp;
      if (v) {
        p[-1] = CLAMP (p[-1] - v, 0, 255);
        p[1] = CLAMP (p[1] - v, 0, 255);
        p[-video_width] = CLAMP (p[-video_width] - v, 0, 255);
        p[video_width] = CLAMP (p[video_width] - v, 0, 255);
      }
      p++;
      bitp++;
    }
    bitp += bit_rowinc;
  }

  y = bitmap_y0;
  y1 = y0 + bitmap_y0;
  x1 = x0 + bitmap_x0;
  bitp = bitmap->buffer + bitmap->pitch * bitmap_y0 + bitmap_x0;
  p = yuv_pixels + video_width * y1 + x1;
  u_p =
      yuv_pixels + video_width * video_height + (video_width >> 1) * (y1 >> 1) +
      (x1 >> 1);
  skip_y = 0;
  skip_x = 0;

  for (; y < bitmap_y0 + bitmap_height; y++) {
    int n;

    x1 = x0 + bitmap_x0;
    skip_x = 0;
    for (n = bitmap_width; n > 0; --n) {
      v = *bitp;
      if (v) {
        *p = v;
        if (!skip_y) {
          u_p[0] = u_p[u_plane_size] = 0x80;
        }
      }
      if (!skip_y) {
        skip_x = !skip_x;
        if (!skip_x)
          u_p++;
      }
      p++;
      bitp++;
    }
    /*if (!skip_x && !skip_y) u_p--; */
    p += I420_Y_ROWSTRIDE (overlay->width) - bitmap_width;
    bitp += bit_rowinc;
    skip_y = !skip_y;
    u_p += skip_y ? uv_rowinc : 0;
  }
}

static void
gst_text_overlay_resize_bitmap (GstTextOverlay * overlay, gint width,
    gint height)
{
  FT_Bitmap *bitmap = &overlay->bitmap;
  int pitch = (width | 3) + 1;
  int size = pitch * height;

  /* no need to keep reallocating; just keep the maximum size so far */
  if (size <= overlay->bitmap_buffer_size) {
    bitmap->rows = height;
    bitmap->width = width;
    bitmap->pitch = pitch;
    memset (bitmap->buffer, 0, overlay->bitmap_buffer_size);
    return;
  }
  if (!bitmap->buffer) {
    /* initialize */
    bitmap->pixel_mode = ft_pixel_mode_grays;
    bitmap->num_grays = 256;
  }
  overlay->bitmap_buffer_size = size;
  bitmap->buffer = g_realloc (bitmap->buffer, size);
  memset (bitmap->buffer, 0, size);
  bitmap->rows = height;
  bitmap->width = width;
  bitmap->pitch = pitch;
}

static void
gst_text_overlay_render_text (GstTextOverlay * overlay,
    const gchar * text, gint textlen)
{
  PangoRectangle ink_rect, logical_rect;
  gchar *string;

  if (!overlay->need_render) {
    GST_DEBUG ("Using previously rendered text.");
    return;
  }

  /* -1 is the whole string */
  if (text != NULL && textlen < 0) {
    textlen = strlen (text);
  }

  if (text != NULL) {
    string = g_strndup (text, textlen);
  } else {                      /* empty string */
    string = g_strdup (" ");
  }
  g_strdelimit (string, "\r\t", ' ');
  textlen = strlen (string);

  /* FIXME: should we check for UTF-8 here? */

  GST_DEBUG ("Rendering '%s'", string);
  pango_layout_set_markup (overlay->layout, string, textlen);

  pango_layout_get_pixel_extents (overlay->layout, &ink_rect, &logical_rect);
  gst_text_overlay_resize_bitmap (overlay, ink_rect.width,
      ink_rect.height + ink_rect.y);
  pango_ft2_render_layout (&overlay->bitmap, overlay->layout, -ink_rect.x, 0);
  overlay->baseline_y = ink_rect.y;

  g_free (string);

  overlay->need_render = FALSE;
}

static GstFlowReturn
gst_text_overlay_push_frame (GstTextOverlay * overlay, GstBuffer * video_frame)
{
  gint xpos, ypos;

  video_frame = gst_buffer_make_writable (video_frame);

  switch (overlay->halign) {
    case GST_TEXT_OVERLAY_HALIGN_LEFT:
      xpos = overlay->xpad;
      break;
    case GST_TEXT_OVERLAY_HALIGN_CENTER:
      xpos = (overlay->width - overlay->bitmap.width) / 2;
      break;
    case GST_TEXT_OVERLAY_HALIGN_RIGHT:
      xpos = overlay->width - overlay->bitmap.width - overlay->xpad;
      break;
    default:
      xpos = 0;
  }
  xpos += overlay->deltax;


  switch (overlay->valign) {
    case GST_TEXT_OVERLAY_VALIGN_BOTTOM:
      ypos = overlay->height - overlay->bitmap.rows - overlay->ypad;
      break;
    case GST_TEXT_OVERLAY_VALIGN_BASELINE:
      ypos = overlay->height - (overlay->bitmap.rows + overlay->ypad);
      break;
    case GST_TEXT_OVERLAY_VALIGN_TOP:
      ypos = overlay->ypad;
      break;
    default:
      ypos = overlay->ypad;
      break;
  }
  ypos += overlay->deltay;

  /* shaded background box */
  if (overlay->want_shading) {
    gst_text_overlay_shade_y (overlay,
        GST_BUFFER_DATA (video_frame),
        I420_Y_ROWSTRIDE (overlay->width),
        xpos, xpos + overlay->bitmap.width, ypos, ypos + overlay->bitmap.rows);
  }


  if (overlay->bitmap.buffer) {
    gst_text_overlay_blit_yuv420 (overlay, &overlay->bitmap,
        GST_BUFFER_DATA (video_frame), xpos, ypos);
  }

  return gst_pad_push (overlay->srcpad, video_frame);
}

static GstPadLinkReturn
gst_text_overlay_text_pad_link (GstPad * pad, GstPad * peer)
{
  GstTextOverlay *overlay;

  overlay = GST_TEXT_OVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (overlay, "Text pad linked");

  overlay->text_linked = TRUE;

  gst_object_unref (overlay);

  return GST_PAD_LINK_OK;
}

static void
gst_text_overlay_text_pad_unlink (GstPad * pad)
{
  GstTextOverlay *overlay;

  /* don't use gst_pad_get_parent() here, will deadlock */
  overlay = GST_TEXT_OVERLAY (GST_PAD_PARENT (pad));

  GST_DEBUG_OBJECT (overlay, "Text pad unlinked");

  overlay->text_linked = FALSE;

  gst_segment_init (&overlay->text_segment, GST_FORMAT_UNDEFINED);
}

static gboolean
gst_text_overlay_text_event (GstPad * pad, GstEvent * event)
{
  gboolean ret = FALSE;
  GstTextOverlay *overlay = NULL;

  overlay = GST_TEXT_OVERLAY (gst_pad_get_parent (pad));

  GST_LOG_OBJECT (pad, "received event %s", GST_EVENT_TYPE_NAME (event));

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_NEWSEGMENT:{
      GstFormat fmt;
      gboolean update;
      gdouble rate, applied_rate;
      gint64 cur, stop, time;

      overlay->text_eos = FALSE;

      gst_event_parse_new_segment_full (event, &update, &rate, &applied_rate,
          &fmt, &cur, &stop, &time);

      if (fmt == GST_FORMAT_TIME) {
        GST_OBJECT_LOCK (overlay);
        gst_segment_set_newsegment_full (&overlay->text_segment, update, rate,
            applied_rate, GST_FORMAT_TIME, cur, stop, time);
        GST_DEBUG_OBJECT (overlay, "TEXT SEGMENT now: %" GST_SEGMENT_FORMAT,
            &overlay->text_segment);
        GST_OBJECT_UNLOCK (overlay);
      } else {
        GST_ELEMENT_WARNING (overlay, STREAM, MUX, (NULL),
            ("received non-TIME newsegment event on text input"));
      }

      gst_event_unref (event);
      ret = TRUE;

      /* wake up the video chain, it might be waiting for a text buffer or
       * a text segment update */
      GST_OBJECT_LOCK (overlay);
      GST_TEXT_OVERLAY_BROADCAST (overlay);
      GST_OBJECT_UNLOCK (overlay);
      break;
    }
    case GST_EVENT_FLUSH_STOP:
      GST_OBJECT_LOCK (overlay);
      overlay->text_flushing = FALSE;
      gst_text_overlay_pop_text (overlay);
      GST_OBJECT_UNLOCK (overlay);
      gst_event_unref (event);
      ret = TRUE;
      break;
    case GST_EVENT_FLUSH_START:
      GST_OBJECT_LOCK (overlay);
      overlay->text_flushing = TRUE;
      GST_TEXT_OVERLAY_BROADCAST (overlay);
      GST_OBJECT_UNLOCK (overlay);
      gst_event_unref (event);
      ret = TRUE;
      break;
    case GST_EVENT_EOS:
      GST_OBJECT_LOCK (overlay);
      overlay->text_flushing = TRUE;
      overlay->text_eos = TRUE;
      GST_INFO_OBJECT (overlay, "EOS");
      /* wake up the video chain, it might be waiting for a text buffer or
       * a text segment update */
      GST_TEXT_OVERLAY_BROADCAST (overlay);
      GST_OBJECT_UNLOCK (overlay);
      gst_event_unref (event);
      ret = TRUE;
      break;
    default:
      ret = gst_pad_event_default (pad, event);
      break;
  }

  gst_object_unref (overlay);

  return ret;
}

static gboolean
gst_text_overlay_video_event (GstPad * pad, GstEvent * event)
{
  gboolean ret = FALSE;
  GstTextOverlay *overlay = NULL;

  overlay = GST_TEXT_OVERLAY (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (pad, "received event %s", GST_EVENT_TYPE_NAME (event));

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_NEWSEGMENT:
    {
      GstFormat format;
      gdouble rate;
      gint64 start, stop, time;
      gboolean update;

      GST_DEBUG_OBJECT (overlay, "received new segment");

      gst_event_parse_new_segment (event, &update, &rate, &format, &start,
          &stop, &time);

      if (format == GST_FORMAT_TIME) {
        GST_DEBUG_OBJECT (overlay, "VIDEO SEGMENT now: %" GST_SEGMENT_FORMAT,
            overlay->segment);

        gst_segment_set_newsegment (overlay->segment, update, rate, format,
            start, stop, time);
      } else {
        GST_ELEMENT_WARNING (overlay, STREAM, MUX, (NULL),
            ("received non-TIME newsegment event on video input"));
      }

      ret = gst_pad_event_default (pad, event);
      break;
    }
    case GST_EVENT_EOS:
      GST_OBJECT_LOCK (overlay);
      overlay->video_flushing = TRUE;
      overlay->text_flushing = TRUE;
      gst_text_overlay_pop_text (overlay);
      GST_OBJECT_UNLOCK (overlay);
      ret = gst_pad_event_default (pad, event);
      break;
    case GST_EVENT_FLUSH_START:
      GST_OBJECT_LOCK (overlay);
      overlay->video_flushing = TRUE;
      GST_TEXT_OVERLAY_BROADCAST (overlay);
      GST_OBJECT_UNLOCK (overlay);
      ret = gst_pad_event_default (pad, event);
      break;
    case GST_EVENT_FLUSH_STOP:
      GST_OBJECT_LOCK (overlay);
      overlay->video_flushing = FALSE;
      GST_OBJECT_UNLOCK (overlay);
      ret = gst_pad_event_default (pad, event);
      break;
    default:
      ret = gst_pad_event_default (pad, event);
      break;
  }

  gst_object_unref (overlay);

  return ret;
}

/* Called with lock held */
static void
gst_text_overlay_pop_text (GstTextOverlay * overlay)
{
  g_return_if_fail (GST_IS_TEXT_OVERLAY (overlay));

  if (overlay->text_buffer) {
    /* update text_segment's last stop */
    if (overlay->text_segment.format == GST_FORMAT_TIME &&
        GST_BUFFER_TIMESTAMP_IS_VALID (overlay->text_buffer)) {
      overlay->text_segment.last_stop =
          GST_BUFFER_TIMESTAMP (overlay->text_buffer);
      if (GST_BUFFER_DURATION_IS_VALID (overlay->text_buffer)) {
        overlay->text_segment.last_stop +=
            GST_BUFFER_DURATION (overlay->text_buffer);
      }
    }
    GST_DEBUG_OBJECT (overlay, "releasing text buffer %p",
        overlay->text_buffer);
    gst_buffer_unref (overlay->text_buffer);
    overlay->text_buffer = NULL;
  }

  /* Let the text task know we used that buffer */
  GST_TEXT_OVERLAY_BROADCAST (overlay);
}

/* We receive text buffers here. If they are out of segment we just ignore them.
   If the buffer is in our segment we keep it internally except if another one
   is already waiting here, in that case we wait that it gets kicked out */
static GstFlowReturn
gst_text_overlay_text_chain (GstPad * pad, GstBuffer * buffer)
{
  GstFlowReturn ret = GST_FLOW_OK;
  GstTextOverlay *overlay = NULL;
  gboolean in_seg = FALSE;
  gint64 clip_start = 0, clip_stop = 0;

  overlay = GST_TEXT_OVERLAY (GST_PAD_PARENT (pad));

  GST_OBJECT_LOCK (overlay);

  if (overlay->text_eos) {
    GST_OBJECT_UNLOCK (overlay);
    ret = GST_FLOW_UNEXPECTED;
    GST_LOG_OBJECT (overlay, "text EOS");
    goto beach;
  }

  if (overlay->text_flushing) {
    GST_OBJECT_UNLOCK (overlay);
    ret = GST_FLOW_WRONG_STATE;
    GST_LOG_OBJECT (overlay, "text flushing");
    goto beach;
  }

  GST_LOG_OBJECT (overlay, "%" GST_SEGMENT_FORMAT "  BUFFER: ts=%"
      GST_TIME_FORMAT ", end=%" GST_TIME_FORMAT, overlay->segment,
      GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (buffer)),
      GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (buffer) +
          GST_BUFFER_DURATION (buffer)));

  in_seg = gst_segment_clip (overlay->segment, GST_FORMAT_TIME,
      GST_BUFFER_TIMESTAMP (buffer),
      GST_BUFFER_TIMESTAMP (buffer) + GST_BUFFER_DURATION (buffer),
      &clip_start, &clip_stop);

  if (in_seg) {
    GST_BUFFER_TIMESTAMP (buffer) = clip_start;
    GST_BUFFER_DURATION (buffer) = clip_stop - clip_start;

    /* Wait for the previous buffer to go away */
    while (overlay->text_buffer != NULL) {
      GST_DEBUG ("Pad %s:%s has a buffer queued, waiting",
          GST_DEBUG_PAD_NAME (pad));
      GST_TEXT_OVERLAY_WAIT (overlay);
      GST_DEBUG ("Pad %s:%s resuming", GST_DEBUG_PAD_NAME (pad));
      if (overlay->text_flushing) {
        GST_OBJECT_UNLOCK (overlay);
        ret = GST_FLOW_WRONG_STATE;
        goto beach;
      }
    }

    overlay->text_buffer = buffer;
    /* That's a new text buffer we need to render */
    overlay->need_render = TRUE;

    /* in case the video chain is waiting for a text buffer, wake it up */
    GST_TEXT_OVERLAY_BROADCAST (overlay);
  }

  GST_OBJECT_UNLOCK (overlay);

beach:

  return ret;
}

static GstFlowReturn
gst_text_overlay_video_chain (GstPad * pad, GstBuffer * buffer)
{
  GstTextOverlayClass *klass;
  GstTextOverlay *overlay;
  GstFlowReturn ret = GST_FLOW_OK;
  gboolean in_seg = FALSE;
  gint64 start, stop, clip_start = 0, clip_stop = 0;
  gchar *text = NULL;

  overlay = GST_TEXT_OVERLAY (GST_PAD_PARENT (pad));
  klass = GST_TEXT_OVERLAY_GET_CLASS (overlay);

  if (!GST_BUFFER_TIMESTAMP_IS_VALID (buffer))
    goto missing_timestamp;

  /* ignore buffers that are outside of the current segment */
  start = GST_BUFFER_TIMESTAMP (buffer);

  if (!GST_BUFFER_DURATION_IS_VALID (buffer)) {
    stop = GST_CLOCK_TIME_NONE;
  } else {
    stop = start + GST_BUFFER_DURATION (buffer);
  }

  GST_LOG_OBJECT (overlay, "%" GST_SEGMENT_FORMAT "  BUFFER: ts=%"
      GST_TIME_FORMAT ", end=%" GST_TIME_FORMAT, overlay->segment,
      GST_TIME_ARGS (start), GST_TIME_ARGS (stop));

  /* segment_clip() will adjust start unconditionally to segment_start if
   * no stop time is provided, so handle this ourselves */
  if (stop == GST_CLOCK_TIME_NONE && start < overlay->segment->start)
    goto out_of_segment;

  in_seg = gst_segment_clip (overlay->segment, GST_FORMAT_TIME, start, stop,
      &clip_start, &clip_stop);

  if (!in_seg)
    goto out_of_segment;

  /* if the buffer is only partially in the segment, fix up stamps */
  if (clip_start != start || (stop != -1 && clip_stop != stop)) {
    GST_DEBUG_OBJECT (overlay, "clipping buffer timestamp/duration to segment");
    buffer = gst_buffer_make_metadata_writable (buffer);
    GST_BUFFER_TIMESTAMP (buffer) = clip_start;
    if (stop != -1)
      GST_BUFFER_DURATION (buffer) = clip_stop - clip_start;
  }

  /* now, after we've done the clipping, fix up end time if there's no
   * duration (we only use those estimated values internally though, we
   * don't want to set bogus values on the buffer itself) */
  if (stop == -1) {
    GstStructure *s;
    gint fps_num, fps_denom;

    s = gst_caps_get_structure (GST_PAD_CAPS (pad), 0);
    if (gst_structure_get_fraction (s, "framerate", &fps_num, &fps_denom)) {
      GST_DEBUG_OBJECT (overlay, "estimating duration based on framerate");
      stop = start + gst_util_uint64_scale_int (GST_SECOND, fps_denom, fps_num);
    } else {
      GST_WARNING_OBJECT (overlay, "no duration, assuming minimal duration");
      stop = start + 1;         /* we need to assume some interval */
    }
  }

wait_for_text_buf:

  GST_OBJECT_LOCK (overlay);

  if (overlay->video_flushing)
    goto flushing;

  if (overlay->silent) {
    GST_OBJECT_UNLOCK (overlay);
    ret = gst_pad_push (overlay->srcpad, buffer);

    /* Update last_stop */
    gst_segment_set_last_stop (overlay->segment, GST_FORMAT_TIME, clip_start);

    return ret;
  }

  /* Text pad not linked, rendering internal text */
  if (!overlay->text_linked) {
    if (klass->get_text) {
      text = klass->get_text (overlay, buffer);
    } else {
      text = g_strdup (overlay->default_text);
    }

    GST_LOG_OBJECT (overlay, "Text pad not linked, rendering default "
        "text: '%s'", GST_STR_NULL (text));

    GST_OBJECT_UNLOCK (overlay);

    if (text != NULL && *text != '\0') {
      /* Render and push */
      gst_text_overlay_render_text (overlay, text, -1);
      ret = gst_text_overlay_push_frame (overlay, buffer);
    } else {
      /* Invalid or empty string */
      ret = gst_pad_push (overlay->srcpad, buffer);
    }
  } else {
    /* Text pad linked, check if we have a text buffer queued */
    if (overlay->text_buffer) {
      gboolean pop_text = FALSE;
      gint64 text_start, text_end;

      /* if the text buffer isn't stamped right, pop it off the
       * queue and display it for the current video frame only */
      if (!GST_BUFFER_TIMESTAMP_IS_VALID (overlay->text_buffer) ||
          !GST_BUFFER_DURATION_IS_VALID (overlay->text_buffer)) {
        GST_WARNING_OBJECT (overlay,
            "Got text buffer with invalid timestamp or duration");
        text_start = start;
        text_end = stop;
        pop_text = TRUE;
      } else {
        text_start = GST_BUFFER_TIMESTAMP (overlay->text_buffer);
        text_end = text_start + GST_BUFFER_DURATION (overlay->text_buffer);
      }

      GST_LOG_OBJECT (overlay, "T: %" GST_TIME_FORMAT " - %" GST_TIME_FORMAT,
          GST_TIME_ARGS (text_start), GST_TIME_ARGS (text_end));
      GST_LOG_OBJECT (overlay, "V: %" GST_TIME_FORMAT " - %" GST_TIME_FORMAT,
          GST_TIME_ARGS (start), GST_TIME_ARGS (stop));

      /* Text too old or in the future */
      if (text_end <= start) {
        /* text buffer too old, get rid of it and do nothing  */
        GST_LOG_OBJECT (overlay, "text buffer too old, popping");
        pop_text = FALSE;
        gst_text_overlay_pop_text (overlay);
        GST_OBJECT_UNLOCK (overlay);
        goto wait_for_text_buf;
      } else if (stop <= text_start) {
        GST_LOG_OBJECT (overlay, "text in future, pushing video buf");
        GST_OBJECT_UNLOCK (overlay);
        /* Push the video frame */
        ret = gst_pad_push (overlay->srcpad, buffer);
      } else {
        gchar *in_text;
        gsize in_size;

        in_text = (gchar *) GST_BUFFER_DATA (overlay->text_buffer);
        in_size = GST_BUFFER_SIZE (overlay->text_buffer);

        /* g_markup_escape_text() absolutely requires valid UTF8 input, it
         * might crash otherwise. We don't fall back on GST_SUBTITLE_ENCODING
         * here on purpose, this is something that needs fixing upstream */
        if (!g_utf8_validate (in_text, in_size, NULL)) {
          const gchar *end = NULL;

          GST_WARNING_OBJECT (overlay, "received invalid UTF-8");
          in_text = g_strndup (in_text, in_size);
          while (!g_utf8_validate (in_text, in_size, &end) && end)
            *((gchar *) end) = '*';
        }

        /* Get the string */
        if (overlay->have_pango_markup) {
          text = g_strndup (in_text, in_size);
        } else {
          text = g_markup_escape_text (in_text, in_size);
        }

        if (text != NULL && *text != '\0') {
          gint text_len = strlen (text);

          while (text_len > 0 && (text[text_len - 1] == '\n' ||
                  text[text_len - 1] == '\r')) {
            --text_len;
          }
          GST_DEBUG_OBJECT (overlay, "Rendering text '%*s'", text_len, text);
          gst_text_overlay_render_text (overlay, text, text_len);
        } else {
          GST_DEBUG_OBJECT (overlay, "No text to render (empty buffer)");
          gst_text_overlay_render_text (overlay, " ", 1);
        }

        if (in_text != (gchar *) GST_BUFFER_DATA (overlay->text_buffer))
          g_free (in_text);

        GST_OBJECT_UNLOCK (overlay);
        ret = gst_text_overlay_push_frame (overlay, buffer);

        if (text_end <= stop) {
          GST_LOG_OBJECT (overlay, "text buffer not needed any longer");
          pop_text = TRUE;
        }
      }
      if (pop_text) {
        GST_OBJECT_LOCK (overlay);
        gst_text_overlay_pop_text (overlay);
        GST_OBJECT_UNLOCK (overlay);
      }
    } else {
      gboolean wait_for_text_buf = TRUE;

      if (overlay->text_eos)
        wait_for_text_buf = FALSE;

      /* Text pad linked, but no text buffer available - what now? */
      if (overlay->text_segment.format == GST_FORMAT_TIME) {
        if (GST_BUFFER_TIMESTAMP (buffer) < overlay->text_segment.start ||
            GST_BUFFER_TIMESTAMP (buffer) < overlay->text_segment.last_stop) {
          wait_for_text_buf = FALSE;
        }
      }

      if (wait_for_text_buf) {
        GST_DEBUG_OBJECT (overlay, "no text buffer, need to wait for one");
        GST_TEXT_OVERLAY_WAIT (overlay);
        GST_DEBUG_OBJECT (overlay, "resuming");
        GST_OBJECT_UNLOCK (overlay);
        goto wait_for_text_buf;
      } else {
        GST_OBJECT_UNLOCK (overlay);
        GST_LOG_OBJECT (overlay, "no need to wait for a text buffer");
        ret = gst_pad_push (overlay->srcpad, buffer);
      }
    }
  }

  g_free (text);

  /* Update last_stop */
  gst_segment_set_last_stop (overlay->segment, GST_FORMAT_TIME, clip_start);

  return ret;

missing_timestamp:
  {
    GST_WARNING_OBJECT (overlay, "buffer without timestamp, discarding");
    gst_buffer_unref (buffer);
    return GST_FLOW_OK;
  }

flushing:
  {
    GST_OBJECT_UNLOCK (overlay);
    GST_DEBUG_OBJECT (overlay, "flushing, discarding buffer");
    gst_buffer_unref (buffer);
    return GST_FLOW_WRONG_STATE;
  }

out_of_segment:
  {
    GST_DEBUG_OBJECT (overlay, "buffer out of segment, discarding");
    gst_buffer_unref (buffer);
    return GST_FLOW_OK;
  }
}

static GstStateChangeReturn
gst_text_overlay_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  GstTextOverlay *overlay = GST_TEXT_OVERLAY (element);

  switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      GST_OBJECT_LOCK (overlay);
      overlay->text_flushing = TRUE;
      overlay->video_flushing = TRUE;
      /* pop_text will broadcast on the GCond and thus also make the video
       * chain exit if it's waiting for a text buffer */
      gst_text_overlay_pop_text (overlay);
      GST_OBJECT_UNLOCK (overlay);
      break;
    default:
      break;
  }

  ret = parent_class->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      GST_OBJECT_LOCK (overlay);
      overlay->text_flushing = FALSE;
      overlay->video_flushing = FALSE;
      GST_OBJECT_UNLOCK (overlay);
      break;
    default:
      break;
  }

  return ret;
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_element_register (plugin, "textoverlay", GST_RANK_NONE,
          GST_TYPE_TEXT_OVERLAY) ||
      !gst_element_register (plugin, "timeoverlay", GST_RANK_NONE,
          GST_TYPE_TIME_OVERLAY) ||
      !gst_element_register (plugin, "clockoverlay", GST_RANK_NONE,
          GST_TYPE_CLOCK_OVERLAY) ||
      !gst_element_register (plugin, "textrender", GST_RANK_NONE,
          GST_TYPE_TEXT_RENDER)) {
    return FALSE;
  }

  /*texttestsrc_plugin_init(module, plugin); */

  GST_DEBUG_CATEGORY_INIT (pango_debug, "pango", 0, "Pango elements");

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, GST_VERSION_MINOR,
    "pango", "Pango-based text rendering and overlay", plugin_init,
    VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
