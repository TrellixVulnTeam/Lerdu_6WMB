/* GStreamer
 * Copyright (C) <2005> Philippe Khalaf <burger@speedy.org> 
 * Copyright (C) <2005> Nokia Corporation <kai.vehmanen@nokia.com>
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
 * SECTION:gstbasertpdepayload
 * @short_description: Base class for RTP depayloader
 *
 * <refsect2>
 * <para>
 * Provides a base class for RTP depayloaders
 * </para>
 * </refsect2>
 */

#include "gstbasertpdepayload.h"

#ifdef GST_DISABLE_DEPRECATED
#define QUEUE_LOCK_INIT(base)   (g_static_rec_mutex_init(&base->queuelock))
#define QUEUE_LOCK_FREE(base)   (g_static_rec_mutex_free(&base->queuelock))
#define QUEUE_LOCK(base)        (g_static_rec_mutex_lock(&base->queuelock))
#define QUEUE_UNLOCK(base)      (g_static_rec_mutex_unlock(&base->queuelock))
#else
/* otherwise it's already been defined in the header (FIXME 0.11)*/
#endif

GST_DEBUG_CATEGORY_STATIC (basertpdepayload_debug);
#define GST_CAT_DEFAULT (basertpdepayload_debug)

#define GST_BASE_RTP_DEPAYLOAD_GET_PRIVATE(obj)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GST_TYPE_BASE_RTP_DEPAYLOAD, GstBaseRTPDepayloadPrivate))

struct _GstBaseRTPDepayloadPrivate
{
  GstClockTime npt_start;
  GstClockTime npt_stop;
  gdouble play_speed;
  gdouble play_scale;

  gboolean discont;
  GstClockTime timestamp;
  GstClockTime duration;
};

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

#define DEFAULT_QUEUE_DELAY	0

enum
{
  PROP_0,
  PROP_QUEUE_DELAY
};

static void gst_base_rtp_depayload_finalize (GObject * object);
static void gst_base_rtp_depayload_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_base_rtp_depayload_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_base_rtp_depayload_setcaps (GstPad * pad, GstCaps * caps);
static GstFlowReturn gst_base_rtp_depayload_chain (GstPad * pad,
    GstBuffer * in);
static gboolean gst_base_rtp_depayload_handle_sink_event (GstPad * pad,
    GstEvent * event);

static GstStateChangeReturn gst_base_rtp_depayload_change_state (GstElement *
    element, GstStateChange transition);

static void gst_base_rtp_depayload_set_gst_timestamp
    (GstBaseRTPDepayload * filter, guint32 rtptime, GstBuffer * buf);

GST_BOILERPLATE (GstBaseRTPDepayload, gst_base_rtp_depayload, GstElement,
    GST_TYPE_ELEMENT);

static void
gst_base_rtp_depayload_base_init (gpointer klass)
{
  /*GstElementClass *element_class = GST_ELEMENT_CLASS (klass); */
}

static void
gst_base_rtp_depayload_class_init (GstBaseRTPDepayloadClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gstelement_class = (GstElementClass *) klass;
  parent_class = g_type_class_peek_parent (klass);

  g_type_class_add_private (klass, sizeof (GstBaseRTPDepayloadPrivate));

  gobject_class->finalize = gst_base_rtp_depayload_finalize;
  gobject_class->set_property = gst_base_rtp_depayload_set_property;
  gobject_class->get_property = gst_base_rtp_depayload_get_property;

  /**
   * GstBaseRTPDepayload::queue-delay
   *
   * Control the amount of packets to buffer.
   *
   * Deprecated: Use a jitterbuffer or RTP session manager to delay packet
   * playback. This property has no effect anymore since 0.10.15.
   */
#ifndef GST_REMOVE_DEPRECATED
  g_object_class_install_property (gobject_class, PROP_QUEUE_DELAY,
      g_param_spec_uint ("queue-delay", "Queue Delay",
          "Amount of ms to queue/buffer, deprecated", 0, G_MAXUINT,
          DEFAULT_QUEUE_DELAY, G_PARAM_READWRITE));
#endif

  gstelement_class->change_state = gst_base_rtp_depayload_change_state;

  klass->set_gst_timestamp = gst_base_rtp_depayload_set_gst_timestamp;

  GST_DEBUG_CATEGORY_INIT (basertpdepayload_debug, "basertpdepayload", 0,
      "Base class for RTP Depayloaders");
}

static void
gst_base_rtp_depayload_init (GstBaseRTPDepayload * filter,
    GstBaseRTPDepayloadClass * klass)
{
  GstPadTemplate *pad_template;
  GstBaseRTPDepayloadPrivate *priv;

  priv = GST_BASE_RTP_DEPAYLOAD_GET_PRIVATE (filter);
  filter->priv = priv;

  GST_DEBUG_OBJECT (filter, "init");

  pad_template =
      gst_element_class_get_pad_template (GST_ELEMENT_CLASS (klass), "sink");
  g_return_if_fail (pad_template != NULL);
  filter->sinkpad = gst_pad_new_from_template (pad_template, "sink");
  gst_pad_set_setcaps_function (filter->sinkpad,
      gst_base_rtp_depayload_setcaps);
  gst_pad_set_chain_function (filter->sinkpad, gst_base_rtp_depayload_chain);
  gst_pad_set_event_function (filter->sinkpad,
      gst_base_rtp_depayload_handle_sink_event);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  pad_template =
      gst_element_class_get_pad_template (GST_ELEMENT_CLASS (klass), "src");
  g_return_if_fail (pad_template != NULL);
  filter->srcpad = gst_pad_new_from_template (pad_template, "src");
  gst_pad_use_fixed_caps (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->queue = g_queue_new ();
  filter->queue_delay = DEFAULT_QUEUE_DELAY;

  gst_segment_init (&filter->segment, GST_FORMAT_UNDEFINED);
}

static void
gst_base_rtp_depayload_finalize (GObject * object)
{
  GstBaseRTPDepayload *filter = GST_BASE_RTP_DEPAYLOAD (object);

  g_queue_free (filter->queue);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean
gst_base_rtp_depayload_setcaps (GstPad * pad, GstCaps * caps)
{
  GstBaseRTPDepayload *filter;
  GstBaseRTPDepayloadClass *bclass;
  GstBaseRTPDepayloadPrivate *priv;
  gboolean res;
  GstStructure *caps_struct;
  const GValue *value;

  filter = GST_BASE_RTP_DEPAYLOAD (gst_pad_get_parent (pad));
  priv = filter->priv;

  bclass = GST_BASE_RTP_DEPAYLOAD_GET_CLASS (filter);

  GST_DEBUG_OBJECT (filter, "Set caps");

  caps_struct = gst_caps_get_structure (caps, 0);

  /* get other values for newsegment */
  value = gst_structure_get_value (caps_struct, "npt-start");
  if (value && G_VALUE_HOLDS_UINT64 (value))
    priv->npt_start = g_value_get_uint64 (value);
  else
    priv->npt_start = 0;
  GST_DEBUG_OBJECT (filter, "NPT start %" G_GUINT64_FORMAT, priv->npt_start);

  value = gst_structure_get_value (caps_struct, "npt-stop");
  if (value && G_VALUE_HOLDS_UINT64 (value))
    priv->npt_stop = g_value_get_uint64 (value);
  else
    priv->npt_stop = -1;

  GST_DEBUG_OBJECT (filter, "NPT stop %" G_GUINT64_FORMAT, priv->npt_stop);

  value = gst_structure_get_value (caps_struct, "play-speed");
  if (value && G_VALUE_HOLDS_DOUBLE (value))
    priv->play_speed = g_value_get_double (value);
  else
    priv->play_speed = 1.0;

  value = gst_structure_get_value (caps_struct, "play-scale");
  if (value && G_VALUE_HOLDS_DOUBLE (value))
    priv->play_scale = g_value_get_double (value);
  else
    priv->play_scale = 1.0;

  if (bclass->set_caps)
    res = bclass->set_caps (filter, caps);
  else
    res = TRUE;

  gst_object_unref (filter);

  return res;
}

static GstFlowReturn
gst_base_rtp_depayload_chain (GstPad * pad, GstBuffer * in)
{
  GstBaseRTPDepayload *filter;
  GstBaseRTPDepayloadPrivate *priv;
  GstBaseRTPDepayloadClass *bclass;
  GstFlowReturn ret = GST_FLOW_OK;
  GstBuffer *out_buf;
  GstClockTime timestamp;

  filter = GST_BASE_RTP_DEPAYLOAD (GST_OBJECT_PARENT (pad));

  priv = filter->priv;
  priv->discont = GST_BUFFER_IS_DISCONT (in);

  /* convert to running_time and save the timestamp, this is the timestamp
   * we put on outgoing buffers. */
  timestamp = GST_BUFFER_TIMESTAMP (in);
  timestamp = gst_segment_to_running_time (&filter->segment, GST_FORMAT_TIME,
      timestamp);
  priv->timestamp = timestamp;
  priv->duration = GST_BUFFER_DURATION (in);

  bclass = GST_BASE_RTP_DEPAYLOAD_GET_CLASS (filter);

  /* let's send it out to processing */
  out_buf = bclass->process (filter, in);
  if (out_buf) {
    guint32 rtptime;

    rtptime = gst_rtp_buffer_get_timestamp (in);

    /* we pass rtptime as backward compatibility, in reality, the incomming
     * buffer timestamp is always applied to the outgoing packet. */
    ret = gst_base_rtp_depayload_push_ts (filter, rtptime, out_buf);
  }
  gst_buffer_unref (in);

  return ret;
}

static gboolean
gst_base_rtp_depayload_handle_sink_event (GstPad * pad, GstEvent * event)
{
  GstBaseRTPDepayload *filter;
  gboolean res = TRUE;

  filter = GST_BASE_RTP_DEPAYLOAD (GST_OBJECT_PARENT (pad));

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_FLUSH_STOP:
      res = gst_pad_push_event (filter->srcpad, event);

      gst_segment_init (&filter->segment, GST_FORMAT_UNDEFINED);
      filter->need_newsegment = TRUE;
      break;
    case GST_EVENT_NEWSEGMENT:
    {
      gboolean update;
      gdouble rate;
      GstFormat fmt;
      gint64 start, stop, position;

      gst_event_parse_new_segment (event, &update, &rate, &fmt, &start, &stop,
          &position);

      gst_segment_set_newsegment (&filter->segment, update, rate, fmt,
          start, stop, position);

      /* don't pass the event downstream, we generate our own segment including
       * the NTP time and other things we receive in caps */
      gst_event_unref (event);
      break;
    }
    default:
      /* pass other events forward */
      res = gst_pad_push_event (filter->srcpad, event);
      break;
  }
  return res;
}

static GstFlowReturn
gst_base_rtp_depayload_push_full (GstBaseRTPDepayload * filter,
    gboolean do_ts, guint32 rtptime, GstBuffer * out_buf)
{
  GstFlowReturn ret;
  GstCaps *srccaps;
  GstBaseRTPDepayloadClass *bclass;
  GstBaseRTPDepayloadPrivate *priv;

  priv = filter->priv;

  /* set the caps if any */
  srccaps = GST_PAD_CAPS (filter->srcpad);
  if (srccaps)
    gst_buffer_set_caps (out_buf, srccaps);

  bclass = GST_BASE_RTP_DEPAYLOAD_GET_CLASS (filter);

  /* set the timestamp if we must and can */
  if (bclass->set_gst_timestamp && do_ts)
    bclass->set_gst_timestamp (filter, rtptime, out_buf);

  if (priv->discont) {
    GST_BUFFER_FLAG_SET (out_buf, GST_BUFFER_FLAG_DISCONT);
    priv->discont = FALSE;
  }

  /* push it */
  GST_LOG_OBJECT (filter, "Pushing buffer size %d, timestamp %" GST_TIME_FORMAT,
      GST_BUFFER_SIZE (out_buf),
      GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (out_buf)));
  ret = gst_pad_push (filter->srcpad, out_buf);
  GST_LOG_OBJECT (filter, "Pushed buffer: %s", gst_flow_get_name (ret));

  return ret;
}

/**
 * gst_base_rtp_depayload_push_ts:
 * @filter: a #GstBaseRTPDepayload
 * @timestamp: an RTP timestamp to apply
 * @out_buf: a #GstBuffer
 *
 * Push @out_buf to the peer of @filter. This function takes ownership of
 * @out_buf.
 *
 * Unlike gst_base_rtp_depayload_push(), this function will apply @timestamp
 * on the outgoing buffer, using the configured clock_rate to convert the
 * timestamp to a valid GStreamer clock time.
 *
 * Returns: a #GstFlowReturn.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstFlowReturn
gst_base_rtp_depayload_push_ts (GstBaseRTPDepayload * filter, guint32 timestamp,
    GstBuffer * out_buf)
{
  return gst_base_rtp_depayload_push_full (filter, TRUE, timestamp, out_buf);
}

/**
 * gst_base_rtp_depayload_push:
 * @filter: a #GstBaseRTPDepayload
 * @out_buf: a #GstBuffer
 *
 * Push @out_buf to the peer of @filter. This function takes ownership of
 * @out_buf.
 *
 * Unlike gst_base_rtp_depayload_push_ts(), this function will not apply
 * any timestamp on the outgoing buffer.
 *
 * Returns: a #GstFlowReturn.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstFlowReturn
gst_base_rtp_depayload_push (GstBaseRTPDepayload * filter, GstBuffer * out_buf)
{
  return gst_base_rtp_depayload_push_full (filter, FALSE, 0, out_buf);
}

static void
gst_base_rtp_depayload_set_gst_timestamp (GstBaseRTPDepayload * filter,
    guint32 rtptime, GstBuffer * buf)
{
  GstBaseRTPDepayloadPrivate *priv;
  GstClockTime timestamp, duration;

  priv = filter->priv;

  timestamp = GST_BUFFER_TIMESTAMP (buf);
  duration = GST_BUFFER_DURATION (buf);

  /* apply last incomming timestamp and duration to outgoing buffer if
   * not otherwise set. */
  if (!GST_CLOCK_TIME_IS_VALID (timestamp))
    GST_BUFFER_TIMESTAMP (buf) = priv->timestamp;
  if (!GST_CLOCK_TIME_IS_VALID (duration))
    GST_BUFFER_DURATION (buf) = priv->duration;

  /* if this is the first buffer send a NEWSEGMENT */
  if (filter->need_newsegment) {
    GstEvent *event;
    GstClockTime stop, position;

    if (priv->npt_stop != -1)
      stop = priv->npt_stop - priv->npt_start;
    else
      stop = -1;

    position = priv->npt_start;

    event =
        gst_event_new_new_segment_full (FALSE, priv->play_speed,
        priv->play_scale, GST_FORMAT_TIME, 0, stop, position);

    gst_pad_push_event (filter->srcpad, event);

    filter->need_newsegment = FALSE;
    GST_DEBUG_OBJECT (filter, "Pushed newsegment event on this first buffer");
  }
}

static GstStateChangeReturn
gst_base_rtp_depayload_change_state (GstElement * element,
    GstStateChange transition)
{
  GstBaseRTPDepayload *filter;
  GstBaseRTPDepayloadPrivate *priv;
  GstStateChangeReturn ret;

  filter = GST_BASE_RTP_DEPAYLOAD (element);
  priv = filter->priv;

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      filter->need_newsegment = TRUE;
      priv->npt_start = 0;
      priv->npt_stop = -1;
      priv->play_speed = 1.0;
      priv->play_scale = 1.0;
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
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }
  return ret;
}

static void
gst_base_rtp_depayload_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstBaseRTPDepayload *filter;

  filter = GST_BASE_RTP_DEPAYLOAD (object);

  switch (prop_id) {
    case PROP_QUEUE_DELAY:
      filter->queue_delay = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_base_rtp_depayload_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstBaseRTPDepayload *filter;

  filter = GST_BASE_RTP_DEPAYLOAD (object);

  switch (prop_id) {
    case PROP_QUEUE_DELAY:
      g_value_set_uint (value, filter->queue_delay);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}
