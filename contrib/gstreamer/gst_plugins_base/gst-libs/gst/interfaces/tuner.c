/* GStreamer Tuner
 * Copyright (C) 2003 Ronald Bultje <rbultje@ronald.bitfreak.net>
 *
 * tuner.c: tuner design virtual class function wrappers
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tuner.h"
#include "interfaces-marshal.h"

#include <string.h>

/**
 * SECTION:gsttuner
 * @short_description: Interface for elements providing tuner operations
 */

enum
{
  NORM_CHANGED,
  CHANNEL_CHANGED,
  FREQUENCY_CHANGED,
  SIGNAL_CHANGED,
  LAST_SIGNAL
};

static void gst_tuner_class_init (GstTunerClass * klass);

static guint gst_tuner_signals[LAST_SIGNAL] = { 0 };
#ifdef __SYMBIAN32__
EXPORT_C
#endif


GType
gst_tuner_get_type (void)
{
  static GType gst_tuner_type = 0;

  if (!gst_tuner_type) {
    static const GTypeInfo gst_tuner_info = {
      sizeof (GstTunerClass),
      (GBaseInitFunc) gst_tuner_class_init,
      NULL,
      NULL,
      NULL,
      NULL,
      0,
      0,
      NULL,
    };

    gst_tuner_type = g_type_register_static (G_TYPE_INTERFACE,
        "GstTuner", &gst_tuner_info, 0);
    g_type_interface_add_prerequisite (gst_tuner_type,
        GST_TYPE_IMPLEMENTS_INTERFACE);
  }

  return gst_tuner_type;
}

static void
gst_tuner_class_init (GstTunerClass * klass)
{
  static gboolean initialized = FALSE;

  if (!initialized) {
    gst_tuner_signals[NORM_CHANGED] =
        g_signal_new ("norm-changed",
        GST_TYPE_TUNER, G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET (GstTunerClass, norm_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE, 1, GST_TYPE_TUNER_NORM);
    gst_tuner_signals[CHANNEL_CHANGED] =
        g_signal_new ("channel-changed",
        GST_TYPE_TUNER, G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET (GstTunerClass, channel_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE, 1,
        GST_TYPE_TUNER_CHANNEL);
    gst_tuner_signals[FREQUENCY_CHANGED] =
        g_signal_new ("frequency-changed",
        GST_TYPE_TUNER, G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET (GstTunerClass, frequency_changed),
        NULL, NULL,
        gst_interfaces_marshal_VOID__OBJECT_ULONG, G_TYPE_NONE, 2,
        GST_TYPE_TUNER_CHANNEL, G_TYPE_ULONG);
    gst_tuner_signals[SIGNAL_CHANGED] =
        g_signal_new ("signal-changed",
        GST_TYPE_TUNER, G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET (GstTunerClass, signal_changed),
        NULL, NULL,
        gst_interfaces_marshal_VOID__OBJECT_INT, G_TYPE_NONE, 2,
        GST_TYPE_TUNER_CHANNEL, G_TYPE_INT);

    initialized = TRUE;
  }

  /* default virtual functions */
  klass->list_channels = NULL;
  klass->set_channel = NULL;
  klass->get_channel = NULL;

  klass->list_norms = NULL;
  klass->set_norm = NULL;
  klass->get_norm = NULL;

  klass->set_frequency = NULL;
  klass->get_frequency = NULL;
  klass->signal_strength = NULL;
}

/**
 * gst_tuner_list_channels:
 * @tuner: the #GstTuner (a #GstElement) to get the channels from.
 *
 * Retrieve a list of channels (e.g. 'composite', 's-video', ...)
 * from the given tuner object.
 *
 * Returns: a list of channels available on this tuner.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


const GList *
gst_tuner_list_channels (GstTuner * tuner)
{
  GstTunerClass *klass;

  g_return_val_if_fail (GST_IS_TUNER (tuner), NULL);

  klass = GST_TUNER_GET_CLASS (tuner);
  if (klass->list_channels) {
    return klass->list_channels (tuner);
  }

  return NULL;
}

/**
 * gst_tuner_set_channel:
 * @tuner: the #GstTuner (a #GstElement) that owns the channel.
 * @channel: the channel to tune to.
 *
 * Tunes the object to the given channel.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
gst_tuner_set_channel (GstTuner * tuner, GstTunerChannel * channel)
{
  GstTunerClass *klass;

  g_return_if_fail (GST_IS_TUNER (tuner));

  klass = GST_TUNER_GET_CLASS (tuner);
  if (klass->set_channel) {
    klass->set_channel (tuner, channel);
  }
}

/**
 * gst_Tuner_get_channel:
 * @tuner: the #GstTuner (a #GstElement) to get the current channel from.
 *
 * Retrieve the current channel from the tuner.
 *
 * Returns: the current channel of the tuner object.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


GstTunerChannel *
gst_tuner_get_channel (GstTuner * tuner)
{
  GstTunerClass *klass;

  g_return_val_if_fail (GST_IS_TUNER (tuner), NULL);

  klass = GST_TUNER_GET_CLASS (tuner);
  if (klass->get_channel) {
    return klass->get_channel (tuner);
  }

  return NULL;
}

/**
 * gst_tuner_get_norms_list:
 * @tuner: the #GstTuner (*a #GstElement) to get the list of norms from.
 *
 * Retrieve a list of available norms on the currently tuned channel
 * from the given tuner object.
 *
 * Returns: A list of norms available on the current channel for this
 *          tuner object.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


const GList *
gst_tuner_list_norms (GstTuner * tuner)
{
  GstTunerClass *klass;

  g_return_val_if_fail (GST_IS_TUNER (tuner), NULL);

  klass = GST_TUNER_GET_CLASS (tuner);
  if (klass->list_norms) {
    return klass->list_norms (tuner);
  }

  return NULL;
}

/**
 * gst_tuner_set_norm:
 * @tuner: the #GstTuner (a #GstElement) to set the norm on.
 * @norm: the norm to use for the current channel.
 *
 * Changes the video norm on this tuner to the given norm.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
gst_tuner_set_norm (GstTuner * tuner, GstTunerNorm * norm)
{
  GstTunerClass *klass;

  g_return_if_fail (GST_IS_TUNER (tuner));

  klass = GST_TUNER_GET_CLASS (tuner);
  if (klass->set_norm) {
    klass->set_norm (tuner, norm);
  }
}

/**
 * gst_tuner_get_norm:
 * @tuner: the #GstTuner (a #GstElement) to get the current norm from.
 *
 * Get the current video norm from the given tuner object for the
 * currently selected channel.
 *
 * Returns: the current norm.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


GstTunerNorm *
gst_tuner_get_norm (GstTuner * tuner)
{
  GstTunerClass *klass;

  g_return_val_if_fail (GST_IS_TUNER (tuner), NULL);

  klass = GST_TUNER_GET_CLASS (tuner);
  if (klass->get_norm) {
    return klass->get_norm (tuner);
  }

  return NULL;
}

/**
 * gst_tuner_set_frequency:
 * @tuner: the #Gsttuner (a #GstElement) that owns the given channel.
 * @channel: the #GstTunerChannel to set the frequency on.
 * @frequency: the frequency to tune in to.
 *
 * Sets a tuning frequency on the given tuner/channel. Note that this
 * requires the given channel to be a "tuning" channel, which can be
 * checked using GST_TUNER_CHANNEL_HAS_FLAG (), with the proper flag
 * being GST_TUNER_CHANNEL_FREQUENCY.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
gst_tuner_set_frequency (GstTuner * tuner,
    GstTunerChannel * channel, gulong frequency)
{
  GstTunerClass *klass;

  g_return_if_fail (GST_IS_TUNER (tuner));
  g_return_if_fail (GST_IS_TUNER_CHANNEL (channel));
  g_return_if_fail (GST_TUNER_CHANNEL_HAS_FLAG (channel,
          GST_TUNER_CHANNEL_FREQUENCY));

  klass = GST_TUNER_GET_CLASS (tuner);
  if (klass->set_frequency) {
    klass->set_frequency (tuner, channel, frequency);
  }
}

/**
 * gst_tuner_get_frequency:
 * @tuner: the #GstTuner (a #GstElement) that owns the given channel.
 * @channel: the #GstTunerChannel to retrieve the frequency from.
 *
 * Retrieve the current frequency from the given channel. The same
 * applies as for set_frequency (): check the flag.
 *
 * Returns: the current frequency, or 0 on error.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


gulong
gst_tuner_get_frequency (GstTuner * tuner, GstTunerChannel * channel)
{
  GstTunerClass *klass;

  g_return_val_if_fail (GST_IS_TUNER (tuner), 0);
  g_return_val_if_fail (GST_IS_TUNER_CHANNEL (channel), 0);
  g_return_val_if_fail (GST_TUNER_CHANNEL_HAS_FLAG (channel,
          GST_TUNER_CHANNEL_FREQUENCY), 0);

  klass = GST_TUNER_GET_CLASS (tuner);

  if (klass->get_frequency) {
    return klass->get_frequency (tuner, channel);
  }

  return 0;
}

/**
 * gst_tuner_get_signal_strength:
 * @tuner: the #GstTuner (a #GstElement) that owns the given channel.
 * @channel: the #GstTunerChannel to get the signal strength from.
 *
 * get the strength of the signal on this channel. Note that this
 * requires the current channel to be a "tuning" channel, e.g. a
 * channel on which frequency can be set. This can be checked using
 * GST_TUNER_CHANNEL_HAS_FLAG (), and the appropriate flag to check
 * for is GST_TUNER_CHANNEL_FREQUENCY.
 *
 * Returns: signal strength, or 0 on error.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


gint
gst_tuner_signal_strength (GstTuner * tuner, GstTunerChannel * channel)
{
  GstTunerClass *klass;

  g_return_val_if_fail (GST_IS_TUNER (tuner), 0);
  g_return_val_if_fail (GST_IS_TUNER_CHANNEL (channel), 0);
  g_return_val_if_fail (GST_TUNER_CHANNEL_HAS_FLAG (channel,
          GST_TUNER_CHANNEL_FREQUENCY), 0);

  klass = GST_TUNER_GET_CLASS (tuner);
  if (klass->signal_strength) {
    return klass->signal_strength (tuner, channel);
  }

  return 0;
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


GstTunerNorm *
gst_tuner_find_norm_by_name (GstTuner * tuner, gchar * norm)
{
  GList *walk;

  g_return_val_if_fail (GST_IS_TUNER (tuner), NULL);
  g_return_val_if_fail (norm != NULL, NULL);

  walk = (GList *) gst_tuner_list_norms (tuner);
  while (walk) {
    if (strcmp (GST_TUNER_NORM (walk->data)->label, norm) == 0)
      return GST_TUNER_NORM (walk->data);
    walk = g_list_next (walk);
  }
  return NULL;
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


GstTunerChannel *
gst_tuner_find_channel_by_name (GstTuner * tuner, gchar * channel)
{
  GList *walk;

  g_return_val_if_fail (GST_IS_TUNER (tuner), NULL);
  g_return_val_if_fail (channel != NULL, NULL);

  walk = (GList *) gst_tuner_list_channels (tuner);
  while (walk) {
    if (strcmp (GST_TUNER_CHANNEL (walk->data)->label, channel) == 0)
      return GST_TUNER_CHANNEL (walk->data);
    walk = g_list_next (walk);
  }
  return NULL;
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
gst_tuner_channel_changed (GstTuner * tuner, GstTunerChannel * channel)
{
  g_return_if_fail (GST_IS_TUNER (tuner));
  g_return_if_fail (GST_IS_TUNER_CHANNEL (channel));

  g_signal_emit (G_OBJECT (tuner),
      gst_tuner_signals[CHANNEL_CHANGED], 0, channel);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
gst_tuner_norm_changed (GstTuner * tuner, GstTunerNorm * norm)
{
  g_return_if_fail (GST_IS_TUNER (tuner));
  g_return_if_fail (GST_IS_TUNER_NORM (norm));

  g_signal_emit (G_OBJECT (tuner), gst_tuner_signals[NORM_CHANGED], 0, norm);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
gst_tuner_frequency_changed (GstTuner * tuner,
    GstTunerChannel * channel, gulong frequency)
{
  g_return_if_fail (GST_IS_TUNER (tuner));
  g_return_if_fail (GST_IS_TUNER_CHANNEL (channel));

  g_signal_emit (G_OBJECT (tuner),
      gst_tuner_signals[FREQUENCY_CHANGED], 0, channel, frequency);

  g_signal_emit_by_name (G_OBJECT (channel), "frequency_changed", frequency);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
gst_tuner_signal_changed (GstTuner * tuner,
    GstTunerChannel * channel, gint signal)
{
  g_return_if_fail (GST_IS_TUNER (tuner));
  g_return_if_fail (GST_IS_TUNER_CHANNEL (channel));

  g_signal_emit (G_OBJECT (tuner),
      gst_tuner_signals[SIGNAL_CHANGED], 0, channel, signal);

  g_signal_emit_by_name (G_OBJECT (channel), "signal_changed", signal);
}
