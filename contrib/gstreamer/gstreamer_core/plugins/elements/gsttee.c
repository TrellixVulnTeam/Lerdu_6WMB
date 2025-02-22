/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *               2000,2001,2002,2003,2004,2005 Wim Taymans <wim@fluendo.com>
 *               2007 Wim Taymans <wim.taymans@gmail.com>
 *
 * gsttee.c: Tee element, one in N out
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
 * SECTION:element-tee
 * @short_description: 1-to-N pipe fitting
 * @see_also: #GstIdentity
 *
 * Split data to multiple pads.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef __SYMBIAN32__
#include <gst_global.h>
#endif
#include "gsttee.h"

#include <string.h>

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include <gobject_global.h>

#endif
static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

GST_DEBUG_CATEGORY_STATIC (gst_tee_debug);
#define GST_CAT_DEFAULT gst_tee_debug

#define GST_TYPE_TEE_PULL_MODE (gst_tee_pull_mode_get_type())
static GType
gst_tee_pull_mode_get_type (void)
{
  static GType type = 0;
  static const GEnumValue data[] = {
    {GST_TEE_PULL_MODE_NEVER, "Never activate in pull mode", "never"},
    {GST_TEE_PULL_MODE_SINGLE, "Only one src pad can be active in pull mode",
        "single"},
    {0, NULL, NULL},
  };

  if (!type) {
    type = g_enum_register_static ("GstTeePullMode", data);
  }
  return type;
}

#define DEFAULT_PROP_NUM_SRC_PADS	0
#define DEFAULT_PROP_HAS_SINK_LOOP	FALSE
#define DEFAULT_PROP_HAS_CHAIN		TRUE
#define DEFAULT_PROP_SILENT		TRUE
#define DEFAULT_PROP_LAST_MESSAGE	NULL
#define DEFAULT_PULL_MODE		GST_TEE_PULL_MODE_NEVER

enum
{
  PROP_0,
  PROP_NUM_SRC_PADS,
  PROP_HAS_SINK_LOOP,
  PROP_HAS_CHAIN,
  PROP_SILENT,
  PROP_LAST_MESSAGE,
  PROP_PULL_MODE,
};

static GstStaticPadTemplate tee_src_template = GST_STATIC_PAD_TEMPLATE ("src%d",
    GST_PAD_SRC,
    GST_PAD_REQUEST,
    GST_STATIC_CAPS_ANY);

#define _do_init(bla) \
    GST_DEBUG_CATEGORY_INIT (gst_tee_debug, "tee", 0, "tee element");

GST_BOILERPLATE_FULL (GstTee, gst_tee, GstElement, GST_TYPE_ELEMENT, _do_init);

/* structure and quark to keep track of which pads have been pushed */
static GQuark push_data;

typedef struct
{
  gboolean pushed;
  GstFlowReturn result;
} PushData;

static GstPad *gst_tee_request_new_pad (GstElement * element,
    GstPadTemplate * temp, const gchar * unused);
static void gst_tee_release_pad (GstElement * element, GstPad * pad);

static void gst_tee_finalize (GObject * object);
static void gst_tee_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_tee_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GstFlowReturn gst_tee_chain (GstPad * pad, GstBuffer * buffer);
static GstFlowReturn gst_tee_buffer_alloc (GstPad * pad, guint64 offset,
    guint size, GstCaps * caps, GstBuffer ** buf);
static gboolean gst_tee_sink_activate_push (GstPad * pad, gboolean active);
static gboolean gst_tee_src_check_get_range (GstPad * pad);
static gboolean gst_tee_src_activate_pull (GstPad * pad, gboolean active);
static GstFlowReturn gst_tee_src_get_range (GstPad * pad, guint64 offset,
    guint length, GstBuffer ** buf);


static void
gst_tee_base_init (gpointer g_class)
{
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_set_details_simple (gstelement_class,
      "Tee pipe fitting",
      "Generic",
      "1-to-N pipe fitting",
      "Erik Walthinsen <omega@cse.ogi.edu>, " "Wim Taymans <wim@fluendo.com>");
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sinktemplate));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&tee_src_template));

  push_data = g_quark_from_static_string ("tee-push-data");
}

static void
gst_tee_finalize (GObject * object)
{
  GstTee *tee;

  tee = GST_TEE (object);

  g_free (tee->last_message);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_tee_class_init (GstTeeClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gstelement_class = GST_ELEMENT_CLASS (klass);

  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_tee_finalize);
  gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_tee_set_property);
  gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_tee_get_property);

  g_object_class_install_property (gobject_class, PROP_NUM_SRC_PADS,
      g_param_spec_int ("num-src-pads", "Num Src Pads",
          "The number of source pads", 0, G_MAXINT, DEFAULT_PROP_NUM_SRC_PADS,
          G_PARAM_READABLE));
  g_object_class_install_property (gobject_class, PROP_HAS_SINK_LOOP,
      g_param_spec_boolean ("has-sink-loop", "Has Sink Loop",
          "If the element should spawn a thread (unimplemented and deprecated)",
          DEFAULT_PROP_HAS_SINK_LOOP, G_PARAM_CONSTRUCT | G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_HAS_CHAIN,
      g_param_spec_boolean ("has-chain", "Has Chain",
          "If the element can operate in push mode",
          DEFAULT_PROP_HAS_CHAIN, G_PARAM_CONSTRUCT | G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent",
          "Don't produce last_message events", DEFAULT_PROP_SILENT,
          G_PARAM_CONSTRUCT | G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_LAST_MESSAGE,
      g_param_spec_string ("last_message", "Last Message",
          "The message describing current status", DEFAULT_PROP_LAST_MESSAGE,
          G_PARAM_READABLE));
  g_object_class_install_property (gobject_class, PROP_PULL_MODE,
      g_param_spec_enum ("pull-mode", "Pull mode",
          "Behavior of tee in pull mode", GST_TYPE_TEE_PULL_MODE,
          DEFAULT_PULL_MODE, G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

  gstelement_class->request_new_pad =
      GST_DEBUG_FUNCPTR (gst_tee_request_new_pad);
  gstelement_class->release_pad = GST_DEBUG_FUNCPTR (gst_tee_release_pad);
}

static void
gst_tee_init (GstTee * tee, GstTeeClass * g_class)
{
  tee->sinkpad = gst_pad_new_from_static_template (&sinktemplate, "sink");
  tee->sink_mode = GST_ACTIVATE_NONE;

  gst_pad_set_setcaps_function (tee->sinkpad,
      GST_DEBUG_FUNCPTR (gst_pad_proxy_setcaps));
  gst_pad_set_getcaps_function (tee->sinkpad,
      GST_DEBUG_FUNCPTR (gst_pad_proxy_getcaps));
  gst_pad_set_bufferalloc_function (tee->sinkpad,
      GST_DEBUG_FUNCPTR (gst_tee_buffer_alloc));
  gst_pad_set_activatepush_function (tee->sinkpad,
      GST_DEBUG_FUNCPTR (gst_tee_sink_activate_push));
  gst_pad_set_chain_function (tee->sinkpad, GST_DEBUG_FUNCPTR (gst_tee_chain));
  gst_element_add_pad (GST_ELEMENT (tee), tee->sinkpad);

  tee->last_message = NULL;
}

static GstPad *
gst_tee_request_new_pad (GstElement * element, GstPadTemplate * templ,
    const gchar * unused)
{
  gchar *name;
  GstPad *srcpad;
  GstTee *tee;
  GstActivateMode mode;
  gboolean res;
  PushData *data;

  tee = GST_TEE (element);

  GST_DEBUG_OBJECT (tee, "requesting pad");

  GST_OBJECT_LOCK (tee);
  name = g_strdup_printf ("src%d", tee->pad_counter++);

  srcpad = gst_pad_new_from_template (templ, name);
  g_free (name);

  mode = tee->sink_mode;

  /* install the data, we automatically free it when the pad is disposed because
   * of _release_pad or when the element goes away. */
  data = g_new0 (PushData, 1);
  data->pushed = FALSE;
  data->result = GST_FLOW_NOT_LINKED;
  g_object_set_qdata_full (G_OBJECT (srcpad), push_data, data, g_free);

  GST_OBJECT_UNLOCK (tee);

  switch (mode) {
    case GST_ACTIVATE_PULL:
      /* we already have a src pad in pull mode, and our pull mode can only be
         SINGLE, so fall through to activate this new pad in push mode */
    case GST_ACTIVATE_PUSH:
      res = gst_pad_activate_push (srcpad, TRUE);
      break;
    default:
      res = TRUE;
      break;
  }

  if (!res)
    goto activate_failed;

  gst_pad_set_setcaps_function (srcpad,
      GST_DEBUG_FUNCPTR (gst_pad_proxy_setcaps));
  gst_pad_set_getcaps_function (srcpad,
      GST_DEBUG_FUNCPTR (gst_pad_proxy_getcaps));
  gst_pad_set_activatepull_function (srcpad,
      GST_DEBUG_FUNCPTR (gst_tee_src_activate_pull));
  gst_pad_set_checkgetrange_function (srcpad,
      GST_DEBUG_FUNCPTR (gst_tee_src_check_get_range));
  gst_pad_set_getrange_function (srcpad,
      GST_DEBUG_FUNCPTR (gst_tee_src_get_range));
  gst_element_add_pad (GST_ELEMENT_CAST (tee), srcpad);

  return srcpad;

  /* ERRORS */
activate_failed:
  {
    GST_OBJECT_LOCK (tee);
    GST_DEBUG_OBJECT (tee, "warning failed to activate request pad");
    if (tee->allocpad == srcpad)
      tee->allocpad = NULL;
    gst_object_unref (srcpad);
    GST_OBJECT_LOCK (tee);
    return NULL;
  }
}

static void
gst_tee_release_pad (GstElement * element, GstPad * pad)
{
  GstTee *tee;

  tee = GST_TEE (element);

  GST_DEBUG_OBJECT (tee, "releasing pad");

  GST_OBJECT_LOCK (tee);
  if (tee->allocpad == pad)
    tee->allocpad = NULL;
  GST_OBJECT_UNLOCK (tee);

  gst_pad_set_active (pad, FALSE);

  gst_element_remove_pad (GST_ELEMENT_CAST (tee), pad);
}

static void
gst_tee_set_property (GObject * object, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  GstTee *tee = GST_TEE (object);

  GST_OBJECT_LOCK (tee);
  switch (prop_id) {
    case PROP_HAS_SINK_LOOP:
      tee->has_sink_loop = g_value_get_boolean (value);
      if (tee->has_sink_loop) {
        g_warning ("tee will never implement has-sink-loop==TRUE");
      }
      break;
    case PROP_HAS_CHAIN:
      tee->has_chain = g_value_get_boolean (value);
      break;
    case PROP_SILENT:
      tee->silent = g_value_get_boolean (value);
      break;
    case PROP_PULL_MODE:
      tee->pull_mode = g_value_get_enum (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
  GST_OBJECT_UNLOCK (tee);
}

static void
gst_tee_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  GstTee *tee = GST_TEE (object);

  GST_OBJECT_LOCK (tee);
  switch (prop_id) {
    case PROP_NUM_SRC_PADS:
      g_value_set_int (value, GST_ELEMENT (tee)->numsrcpads);
      break;
    case PROP_HAS_SINK_LOOP:
      g_value_set_boolean (value, tee->has_sink_loop);
      break;
    case PROP_HAS_CHAIN:
      g_value_set_boolean (value, tee->has_chain);
      break;
    case PROP_SILENT:
      g_value_set_boolean (value, tee->silent);
      break;
    case PROP_LAST_MESSAGE:
      g_value_set_string (value, tee->last_message);
      break;
    case PROP_PULL_MODE:
      g_value_set_enum (value, tee->pull_mode);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
  GST_OBJECT_UNLOCK (tee);
}

/* we have no previous source pad we can use to proxy the pad alloc. Loop over
 * the source pads, try to alloc a buffer on each one of them. Keep a reference
 * to the first pad that succeeds, we will be using it to alloc more buffers
 * later. */
static GstFlowReturn
gst_tee_find_buffer_alloc (GstTee * tee, guint64 offset, guint size,
    GstCaps * caps, GstBuffer ** buf)
{
  GstFlowReturn res;
  GList *pads;
  guint32 cookie;

  res = GST_FLOW_NOT_LINKED;

retry:
  pads = GST_ELEMENT_CAST (tee)->srcpads;
  cookie = GST_ELEMENT_CAST (tee)->pads_cookie;

  while (pads) {
    GstPad *pad;

    pad = GST_PAD_CAST (pads->data);
    gst_object_ref (pad);
    GST_DEBUG_OBJECT (tee, "try alloc on pad %s:%s", GST_DEBUG_PAD_NAME (pad));
    GST_OBJECT_UNLOCK (tee);

    res = gst_pad_alloc_buffer (pad, offset, size, caps, buf);

    GST_DEBUG_OBJECT (tee, "got return value %d", res);

    gst_object_unref (pad);

    GST_OBJECT_LOCK (tee);
    if (GST_ELEMENT_CAST (tee)->pads_cookie != cookie) {
      GST_DEBUG_OBJECT (tee, "pad list changed, restart");
      /* pad list changed, restart. If the pad alloc function returned OK we
       * need to unref the buffer */
      if (res == GST_FLOW_OK)
        gst_buffer_unref (*buf);
      goto retry;
    }
    if (res == GST_FLOW_OK) {
      GST_DEBUG_OBJECT (tee, "we have a buffer on pad %s:%s",
          GST_DEBUG_PAD_NAME (pad));
      /* we have a buffer, keep the pad for later and exit the loop. */
      tee->allocpad = pad;
      break;
    }
    /* no valid buffer, try another pad */
    pads = g_list_next (pads);
  }

  return res;
}

static GstFlowReturn
gst_tee_buffer_alloc (GstPad * pad, guint64 offset, guint size,
    GstCaps * caps, GstBuffer ** buf)
{
  GstTee *tee;
  GstFlowReturn res;
  GstPad *allocpad;

  tee = GST_TEE (GST_PAD_PARENT (pad));

  res = GST_FLOW_NOT_LINKED;

  GST_OBJECT_LOCK (tee);
  if ((allocpad = tee->allocpad)) {
    /* if we had a previous pad we used for allocating a buffer, continue using
     * it. */
    GST_DEBUG_OBJECT (tee, "using pad %s:%s for alloc",
        GST_DEBUG_PAD_NAME (allocpad));
    gst_object_ref (allocpad);
    GST_OBJECT_UNLOCK (tee);

    res = gst_pad_alloc_buffer (allocpad, offset, size, caps, buf);
    gst_object_unref (allocpad);

    GST_OBJECT_LOCK (tee);
  }
  /* either we failed to alloc on the the previous pad or we did not have a
   * previous pad. */
  if (res == GST_FLOW_NOT_LINKED) {
    /* find a new pad to alloc a buffer on */
    GST_DEBUG_OBJECT (tee, "finding pad for alloc");
    res = gst_tee_find_buffer_alloc (tee, offset, size, caps, buf);
  }
  GST_OBJECT_UNLOCK (tee);

  return res;
}

static GstFlowReturn
gst_tee_do_push (GstTee * tee, GstPad * pad, GstBuffer * buffer)
{
  GstFlowReturn res;

  if (G_UNLIKELY (!tee->silent)) {
    GST_OBJECT_LOCK (tee);
    g_free (tee->last_message);
    tee->last_message =
        g_strdup_printf ("chain        ******* (%s:%s)t (%d bytes, %"
        G_GUINT64_FORMAT ") %p", GST_DEBUG_PAD_NAME (pad),
        GST_BUFFER_SIZE (buffer), GST_BUFFER_TIMESTAMP (buffer), buffer);
    GST_OBJECT_UNLOCK (tee);
    g_object_notify (G_OBJECT (tee), "last_message");
  }

  /* Push */
  if (pad == tee->pull_pad) {
    /* don't push on the pad we're pulling from */
    res = GST_FLOW_OK;
  } else {
    res = gst_pad_push (pad, gst_buffer_ref (buffer));
  }
  return res;
}

static void
clear_pads (GstPad * pad, GstTee * tee)
{
  PushData *data;

  data = g_object_get_qdata (G_OBJECT (pad), push_data);

  /* the data must be there or we have a screwed up internal state */
  g_assert (data != NULL);

  data->pushed = FALSE;
  data->result = GST_FLOW_NOT_LINKED;
}

static GstFlowReturn
gst_tee_handle_buffer (GstTee * tee, GstBuffer * buffer)
{
  GList *pads;
  guint32 cookie;
  GstFlowReturn ret, cret;

  tee->offset += GST_BUFFER_SIZE (buffer);

  GST_OBJECT_LOCK (tee);
  /* mark all pads as 'not pushed on yet' */
  g_list_foreach (GST_ELEMENT_CAST (tee)->srcpads, (GFunc) clear_pads, tee);

restart:
  cret = GST_FLOW_NOT_LINKED;
  pads = GST_ELEMENT_CAST (tee)->srcpads;
  cookie = GST_ELEMENT_CAST (tee)->pads_cookie;

  while (pads) {
    GstPad *pad;
    PushData *data;

    pad = GST_PAD_CAST (pads->data);

    /* get the private data, something is really wrong with the internal state
     * when it is not there */
    data = g_object_get_qdata (G_OBJECT (pad), push_data);

    g_assert (data != NULL);

    if (!data->pushed) {
      /* not yet pushed, release lock and start pushing */
      gst_object_ref (pad);
      GST_OBJECT_UNLOCK (tee);

      GST_LOG_OBJECT (tee, "Starting to push buffer %p", buffer);

      ret = gst_tee_do_push (tee, pad, buffer);

      GST_LOG_OBJECT (tee, "Pushing buffer %p yielded result %s", buffer,
          gst_flow_get_name (ret));

      GST_OBJECT_LOCK (tee);
      /* keep track of which pad we pushed and the result value. We need to do
       * this before we release the refcount on the pad, the PushData is
       * destroyed when the last ref of the pad goes away. */
      data->pushed = TRUE;
      data->result = ret;
      gst_object_unref (pad);
    } else {
      /* already pushed, use previous return value */
      ret = data->result;
      GST_LOG_OBJECT (tee, "pad already pushed with %s",
          gst_flow_get_name (ret));
    }
    /* stop pushing more buffers when we have a fatal error */
    if (GST_FLOW_IS_FATAL (ret))
      goto error;

    /* keep all other return values, overwriting the previous one */
    GST_LOG_OBJECT (tee, "Replacing ret val %d with %d", cret, ret);
    if (cret == GST_FLOW_NOT_LINKED)
      cret = ret;

    if (GST_ELEMENT_CAST (tee)->pads_cookie != cookie) {
      GST_LOG_OBJECT (tee, "pad list changed");
      /* the list of pads changed, restart iteration. Pads that we already
       * pushed on and are still in the new list, will not be pushed on
       * again. */
      goto restart;
    }
    pads = g_list_next (pads);
  }
  GST_OBJECT_UNLOCK (tee);

  gst_buffer_unref (buffer);

  /* no need to unset gvalue */
  return cret;

  /* ERRORS */
error:
  {
    GST_DEBUG_OBJECT (tee, "received error %s", gst_flow_get_name (ret));
    gst_buffer_unref (buffer);
    GST_OBJECT_UNLOCK (tee);
    return ret;
  }
}

static GstFlowReturn
gst_tee_chain (GstPad * pad, GstBuffer * buffer)
{
  GstFlowReturn res;
  GstTee *tee;

  tee = GST_TEE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT (tee, "received buffer %p", buffer);

  res = gst_tee_handle_buffer (tee, buffer);

  GST_DEBUG_OBJECT (tee, "handled buffer %s", gst_flow_get_name (res));

  gst_object_unref (tee);

  return res;
}

static gboolean
gst_tee_sink_activate_push (GstPad * pad, gboolean active)
{
  GstTee *tee;

  tee = GST_TEE (GST_OBJECT_PARENT (pad));

  GST_OBJECT_LOCK (tee);
  tee->sink_mode = active && GST_ACTIVATE_PUSH;

  if (active && !tee->has_chain)
    goto no_chain;
  GST_OBJECT_UNLOCK (tee);

  return TRUE;

  /* ERRORS */
no_chain:
  {
    GST_OBJECT_UNLOCK (tee);
    GST_INFO_OBJECT (tee,
        "Tee cannot operate in push mode with has-chain==FALSE");
    return FALSE;
  }
}

static gboolean
gst_tee_src_activate_pull (GstPad * pad, gboolean active)
{
  GstTee *tee;
  gboolean res;
  GstPad *sinkpad;

  tee = GST_TEE (gst_pad_get_parent (pad));

  GST_OBJECT_LOCK (tee);

  if (tee->pull_mode == GST_TEE_PULL_MODE_NEVER)
    goto cannot_pull;

  if (tee->pull_mode == GST_TEE_PULL_MODE_SINGLE && active && tee->pull_pad)
    goto cannot_pull_multiple_srcs;

  sinkpad = gst_object_ref (tee->sinkpad);

  GST_OBJECT_UNLOCK (tee);

  res = gst_pad_activate_pull (sinkpad, active);
  gst_object_unref (sinkpad);

  if (!res)
    goto sink_activate_failed;

  GST_OBJECT_LOCK (tee);
  if (active) {
    if (tee->pull_mode == GST_TEE_PULL_MODE_SINGLE)
      tee->pull_pad = pad;
  } else {
    if (pad == tee->pull_pad)
      tee->pull_pad = NULL;
  }
  tee->sink_mode = active && GST_ACTIVATE_PULL;
  GST_OBJECT_UNLOCK (tee);

  gst_object_unref (tee);

  return res;

  /* ERRORS */
cannot_pull:
  {
    GST_OBJECT_UNLOCK (tee);
    GST_INFO_OBJECT (tee, "Cannot activate in pull mode, pull-mode "
        "set to NEVER");
    gst_object_unref (tee);
    return FALSE;
  }
cannot_pull_multiple_srcs:
  {
    GST_OBJECT_UNLOCK (tee);
    GST_INFO_OBJECT (tee, "Cannot activate multiple src pads in pull mode, "
        "pull-mode set to SINGLE");
    gst_object_unref (tee);
    return FALSE;
  }
sink_activate_failed:
  {
    GST_INFO_OBJECT (tee, "Failed to %sactivate sink pad in pull mode",
        active ? "" : "de");
    gst_object_unref (tee);
    return FALSE;
  }
}

static gboolean
gst_tee_src_check_get_range (GstPad * pad)
{
  GstTee *tee;
  gboolean res;
  GstPad *sinkpad;

  tee = GST_TEE (gst_pad_get_parent (pad));

  GST_OBJECT_LOCK (tee);

  if (tee->pull_mode == GST_TEE_PULL_MODE_NEVER)
    goto cannot_pull;

  if (tee->pull_mode == GST_TEE_PULL_MODE_SINGLE && tee->pull_pad)
    goto cannot_pull_multiple_srcs;

  sinkpad = gst_object_ref (tee->sinkpad);

  GST_OBJECT_UNLOCK (tee);

  res = gst_pad_check_pull_range (sinkpad);
  gst_object_unref (sinkpad);

  gst_object_unref (tee);

  return res;

  /* ERRORS */
cannot_pull:
  {
    GST_OBJECT_UNLOCK (tee);
    GST_INFO_OBJECT (tee, "Cannot activate in pull mode, pull-mode "
        "set to NEVER");
    gst_object_unref (tee);
    return FALSE;
  }
cannot_pull_multiple_srcs:
  {
    GST_OBJECT_UNLOCK (tee);
    GST_INFO_OBJECT (tee, "Cannot activate multiple src pads in pull mode, "
        "pull-mode set to SINGLE");
    gst_object_unref (tee);
    return FALSE;
  }
}

static void
gst_tee_push_eos (GstPad * pad, GstTee * tee)
{
  if (pad != tee->pull_pad)
    gst_pad_push_event (pad, gst_event_new_eos ());
  gst_object_unref (pad);
}

static void
gst_tee_pull_eos (GstTee * tee)
{
  GstIterator *iter;

  iter = gst_element_iterate_src_pads (GST_ELEMENT (tee));
  gst_iterator_foreach (iter, (GFunc) gst_tee_push_eos, tee);
  gst_iterator_free (iter);
}

static GstFlowReturn
gst_tee_src_get_range (GstPad * pad, guint64 offset, guint length,
    GstBuffer ** buf)
{
  GstTee *tee;
  GstFlowReturn ret;

  tee = GST_TEE (gst_pad_get_parent (pad));

  ret = gst_pad_pull_range (tee->sinkpad, offset, length, buf);

  if (ret == GST_FLOW_OK)
    ret = gst_tee_handle_buffer (tee, gst_buffer_ref (*buf));
  else if (ret == GST_FLOW_UNEXPECTED)
    gst_tee_pull_eos (tee);

  gst_object_unref (tee);

  return ret;
}
