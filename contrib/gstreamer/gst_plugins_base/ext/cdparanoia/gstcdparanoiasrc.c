/* -*- Mode: C; tab-width: 2; indent-tabs-mode: t; c-basic-offset: 2 -*- */
/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 *               <2005> Wim Taymans <wim@fluendo.com>
 *               <2005> Tim-Philipp Müller <tim centricular net>
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

#include <string.h>
#include <errno.h>

#include "gstcdparanoiasrc.h"
#include "gst/gst-i18n-plugin.h"

enum
{
  TRANSPORT_ERROR,
  UNCORRECTED_ERROR,
  NUM_SIGNALS
};

enum
{
  PROP_0,
  PROP_READ_SPEED,
  PROP_PARANOIA_MODE,
  PROP_SEARCH_OVERLAP,
  PROP_GENERIC_DEVICE
};

#define DEFAULT_READ_SPEED              -1
#define DEFAULT_SEARCH_OVERLAP          -1
#define DEFAULT_PARANOIA_MODE            PARANOIA_MODE_FRAGMENT
#define DEFAULT_GENERIC_DEVICE           NULL

GST_DEBUG_CATEGORY_STATIC (gst_cd_paranoia_src_debug);
#define GST_CAT_DEFAULT gst_cd_paranoia_src_debug

GST_BOILERPLATE (GstCdParanoiaSrc, gst_cd_paranoia_src, GstCddaBaseSrc,
    GST_TYPE_CDDA_BASE_SRC);

static void gst_cd_paranoia_src_finalize (GObject * obj);
static void gst_cd_paranoia_src_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_cd_paranoia_src_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static GstBuffer *gst_cd_paranoia_src_read_sector (GstCddaBaseSrc * src,
    gint sector);
static gboolean gst_cd_paranoia_src_open (GstCddaBaseSrc * src,
    const gchar * device);
static void gst_cd_paranoia_src_close (GstCddaBaseSrc * src);

static const GstElementDetails cdparanoia_details =
GST_ELEMENT_DETAILS ("CD Audio (cdda) Source, Paranoia IV",
    "Source/File",
    "Read audio from CD in paranoid mode",
    "Erik Walthinsen <omega@cse.ogi.edu>, " "Wim Taymans <wim@fluendo.com>");

/* We use these to serialize calls to paranoia_read() among several
 * cdparanoiasrc instances. We do this because it's the only reasonably
 * easy way to find out the calling object from within the paranoia
 * callback, and we need the object instance in there to emit our signals */
static GstCdParanoiaSrc *cur_cb_source;
static GStaticMutex cur_cb_mutex = G_STATIC_MUTEX_INIT;

static gint cdpsrc_signals[NUM_SIGNALS];        /* all 0 */

#define GST_TYPE_CD_PARANOIA_MODE (gst_cd_paranoia_mode_get_type())
static GType
gst_cd_paranoia_mode_get_type (void)
{
  static const GFlagsValue paranoia_modes[] = {
    {PARANOIA_MODE_DISABLE, "PARANOIA_MODE_DISABLE", "disable"},
    {PARANOIA_MODE_FRAGMENT, "PARANOIA_MODE_FRAGMENT", "fragment"},
    {PARANOIA_MODE_OVERLAP, "PARANOIA_MODE_OVERLAP", "overlap"},
    {PARANOIA_MODE_SCRATCH, "PARANOIA_MODE_SCRATCH", "scratch"},
    {PARANOIA_MODE_REPAIR, "PARANOIA_MODE_REPAIR", "repair"},
    {PARANOIA_MODE_FULL, "PARANOIA_MODE_FULL", "full"},
    {0, NULL, NULL},
  };

  static GType type;            /* 0 */

  if (!type) {
    type = g_flags_register_static ("GstCdParanoiaMode", paranoia_modes);
  }

  return type;
}

static void
gst_cd_paranoia_src_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_set_details (element_class, &cdparanoia_details);
}

static void
gst_cd_paranoia_src_init (GstCdParanoiaSrc * src, GstCdParanoiaSrcClass * klass)
{
  src->d = NULL;
  src->p = NULL;
  src->next_sector = -1;

  src->search_overlap = DEFAULT_SEARCH_OVERLAP;
  src->paranoia_mode = DEFAULT_PARANOIA_MODE;
  src->read_speed = DEFAULT_READ_SPEED;
  src->generic_device = g_strdup (DEFAULT_GENERIC_DEVICE);
}

static void
gst_cd_paranoia_src_class_init (GstCdParanoiaSrcClass * klass)
{
  GstCddaBaseSrcClass *cddabasesrc_class = GST_CDDA_BASE_SRC_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gst_cd_paranoia_src_set_property;
  gobject_class->get_property = gst_cd_paranoia_src_get_property;
  gobject_class->finalize = gst_cd_paranoia_src_finalize;

  cddabasesrc_class->open = gst_cd_paranoia_src_open;
  cddabasesrc_class->close = gst_cd_paranoia_src_close;
  cddabasesrc_class->read_sector = gst_cd_paranoia_src_read_sector;

  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_GENERIC_DEVICE,
      g_param_spec_string ("generic-device", "Generic device",
          "Use specified generic scsi device", DEFAULT_GENERIC_DEVICE,
          G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_READ_SPEED,
      g_param_spec_int ("read-speed", "Read speed",
          "Read from device at specified speed", -1, G_MAXINT,
          DEFAULT_READ_SPEED, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_PARANOIA_MODE,
      g_param_spec_flags ("paranoia-mode", "Paranoia mode",
          "Type of checking to perform", GST_TYPE_CD_PARANOIA_MODE,
          DEFAULT_PARANOIA_MODE, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_SEARCH_OVERLAP,
      g_param_spec_int ("search-overlap", "Search overlap",
          "Force minimum overlap search during verification to n sectors", -1,
          75, DEFAULT_SEARCH_OVERLAP, G_PARAM_READWRITE));

  /* FIXME: we don't really want signals for this, but messages on the bus,
   * but then we can't check any longer whether anyone is interested in them */
  cdpsrc_signals[TRANSPORT_ERROR] =
      g_signal_new ("transport-error", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstCdParanoiaSrcClass, transport_error),
      NULL, NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);
  cdpsrc_signals[UNCORRECTED_ERROR] =
      g_signal_new ("uncorrected-error", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstCdParanoiaSrcClass, uncorrected_error),
      NULL, NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);
}

static gboolean
gst_cd_paranoia_src_open (GstCddaBaseSrc * cddabasesrc, const gchar * device)
{
  GstCdParanoiaSrc *src = GST_CD_PARANOIA_SRC (cddabasesrc);
  gint i;

  GST_DEBUG_OBJECT (src, "trying to open device %s (generic-device=%s) ...",
      device, GST_STR_NULL (src->generic_device));

  /* find the device */
  if (src->generic_device != NULL) {
    src->d = cdda_identify_scsi (src->generic_device, device, FALSE, NULL);
  } else {
    if (device != NULL) {
      src->d = cdda_identify (device, FALSE, NULL);
    } else {
      src->d = cdda_identify ("/dev/cdrom", FALSE, NULL);
    }
  }

  /* fail if the device couldn't be found */
  if (src->d == NULL)
    goto no_device;

  /* set verbosity mode */
  cdda_verbose_set (src->d, CDDA_MESSAGE_FORGETIT, CDDA_MESSAGE_FORGETIT);

  /* open the disc */
  if (cdda_open (src->d))
    goto open_failed;

  if (src->read_speed != -1) {
    cdda_speed_set (src->d, src->read_speed);
  }

  for (i = 1; i < src->d->tracks + 1; i++) {
    GstCddaBaseSrcTrack track = { 0, };

    track.num = i;
    track.is_audio = IS_AUDIO (src->d, i - 1);
    track.start = cdda_track_firstsector (src->d, i);
    track.end = cdda_track_lastsector (src->d, i);
    track.tags = NULL;

    gst_cdda_base_src_add_track (GST_CDDA_BASE_SRC (src), &track);
  }

  /* create the paranoia struct and set it up */
  src->p = paranoia_init (src->d);
  if (src->p == NULL)
    goto init_failed;

  paranoia_modeset (src->p, src->paranoia_mode);

  if (src->search_overlap != -1)
    paranoia_overlapset (src->p, src->search_overlap);

  src->next_sector = -1;

  return TRUE;

  /* ERRORS */
no_device:
  {
    GST_ELEMENT_ERROR (src, RESOURCE, OPEN_READ,
        (_("Could not open CD device for reading.")), ("cdda_identify failed"));
    return FALSE;
  }
open_failed:
  {
    GST_ELEMENT_ERROR (src, RESOURCE, OPEN_READ,
        (_("Could not open CD device for reading.")), ("cdda_open failed"));
    cdda_close (src->d);
    src->d = NULL;
    return FALSE;
  }
init_failed:
  {
    GST_ELEMENT_ERROR (src, LIBRARY, INIT,
        ("failed to initialize paranoia"), ("failed to initialize paranoia"));
    return FALSE;
  }
}

static void
gst_cd_paranoia_src_close (GstCddaBaseSrc * cddabasesrc)
{
  GstCdParanoiaSrc *src = GST_CD_PARANOIA_SRC (cddabasesrc);

  if (src->p) {
    paranoia_free (src->p);
    src->p = NULL;
  }

  if (src->d) {
    cdda_close (src->d);
    src->d = NULL;
  }

  src->next_sector = -1;
}

static void
gst_cd_paranoia_dummy_callback (long inpos, int function)
{
  /* Used by instanced where no one is interested what's happening here */
}

static void
gst_cd_paranoia_paranoia_callback (long inpos, int function)
{
  GstCdParanoiaSrc *src = cur_cb_source;
  gint sector = (gint) (inpos / CD_FRAMEWORDS);

  switch (function) {
    case PARANOIA_CB_SKIP:
      GST_INFO_OBJECT (src, "Skip at sector %d", sector);
      g_signal_emit (src, cdpsrc_signals[UNCORRECTED_ERROR], 0, sector);
      break;
    case PARANOIA_CB_READERR:
      GST_INFO_OBJECT (src, "Transport error at sector %d", sector);
      g_signal_emit (src, cdpsrc_signals[TRANSPORT_ERROR], 0, sector);
      break;
    default:
      break;
  }
}

static gboolean
gst_cd_paranoia_src_signal_is_being_watched (GstCdParanoiaSrc * src, gint sig)
{
  return g_signal_has_handler_pending (src, cdpsrc_signals[sig], 0, FALSE);
}

static GstBuffer *
gst_cd_paranoia_src_read_sector (GstCddaBaseSrc * cddabasesrc, gint sector)
{
  GstCdParanoiaSrc *src = GST_CD_PARANOIA_SRC (cddabasesrc);
  GstBuffer *buf;
  gboolean do_serialize;
  gint16 *cdda_buf;

#if 0
  /* Do we really need to output this? (tpm) */
  /* Due to possible autocorrections of start sectors of audio tracks on 
   * multisession cds, we can maybe not compute the correct discid.
   * So issue a warning.
   * See cdparanoia/interface/common-interface.c:FixupTOC */
  if (src->d && src->d->cd_extra) {
    g_message
        ("DiscID on multisession discs might be broken. Use at own risk.");
  }
#endif

  if (src->next_sector == -1 || src->next_sector != sector) {
    if (paranoia_seek (src->p, sector, SEEK_SET) == -1)
      goto seek_failed;

    GST_DEBUG_OBJECT (src, "successfully seeked to sector %d", sector);
    src->next_sector = sector;
  }

  do_serialize =
      gst_cd_paranoia_src_signal_is_being_watched (src, TRANSPORT_ERROR) ||
      gst_cd_paranoia_src_signal_is_being_watched (src, UNCORRECTED_ERROR);

  if (do_serialize) {
    GST_LOG_OBJECT (src, "Signal handlers connected, serialising access");
    g_static_mutex_lock (&cur_cb_mutex);
    GST_LOG_OBJECT (src, "Got lock");
    cur_cb_source = src;

    cdda_buf = paranoia_read (src->p, gst_cd_paranoia_paranoia_callback);

    cur_cb_source = NULL;
    GST_LOG_OBJECT (src, "Releasing lock");
    g_static_mutex_unlock (&cur_cb_mutex);
  } else {
    cdda_buf = paranoia_read (src->p, gst_cd_paranoia_dummy_callback);
  }

  if (cdda_buf == NULL)
    goto read_failed;

  buf = gst_buffer_new_and_alloc (CD_FRAMESIZE_RAW);
  memcpy (GST_BUFFER_DATA (buf), cdda_buf, CD_FRAMESIZE_RAW);

  /* cdda base class will take care of timestamping etc. */
  ++src->next_sector;

  return buf;

  /* ERRORS */
seek_failed:
  {
    GST_WARNING_OBJECT (src, "seek to sector %d failed!", sector);
    GST_ELEMENT_ERROR (src, RESOURCE, SEEK,
        (_("Could not seek CD.")),
        ("paranoia_seek to %d failed: %s", sector, g_strerror (errno)));
    return NULL;
  }
read_failed:
  {
    GST_WARNING_OBJECT (src, "read at sector %d failed!", sector);
    GST_ELEMENT_ERROR (src, RESOURCE, READ,
        (_("Could not read CD.")),
        ("paranoia_read at %d failed: %s", sector, g_strerror (errno)));
    return NULL;
  }
}

static void
gst_cd_paranoia_src_finalize (GObject * obj)
{
  GstCdParanoiaSrc *src = GST_CD_PARANOIA_SRC (obj);

  g_free (src->generic_device);

  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
gst_cd_paranoia_src_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstCdParanoiaSrc *src = GST_CD_PARANOIA_SRC (object);

  GST_OBJECT_LOCK (src);

  switch (prop_id) {
    case PROP_GENERIC_DEVICE:{
      g_free (src->generic_device);
      src->generic_device = g_value_dup_string (value);
      if (src->generic_device && src->generic_device[0] == '\0') {
        g_free (src->generic_device);
        src->generic_device = NULL;
      }
      break;
    }
    case PROP_READ_SPEED:{
      src->read_speed = g_value_get_int (value);
      if (src->read_speed == 0)
        src->read_speed = -1;
      break;
    }
    case PROP_PARANOIA_MODE:{
      src->paranoia_mode = g_value_get_flags (value) & PARANOIA_MODE_FULL;
      break;
    }
    case PROP_SEARCH_OVERLAP:{
      src->search_overlap = g_value_get_int (value);
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (src);
}

static void
gst_cd_paranoia_src_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstCdParanoiaSrc *src = GST_CD_PARANOIA_SRC (object);

  GST_OBJECT_LOCK (src);

  switch (prop_id) {
    case PROP_READ_SPEED:
      g_value_set_int (value, src->read_speed);
      break;
    case PROP_PARANOIA_MODE:
      g_value_set_flags (value, src->paranoia_mode);
      break;
    case PROP_GENERIC_DEVICE:
      g_value_set_string (value, src->generic_device);
      break;
    case PROP_SEARCH_OVERLAP:
      g_value_set_int (value, src->search_overlap);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (src);
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (gst_cd_paranoia_src_debug, "cdparanoiasrc", 0,
      "CD Paranoia Source");

  if (!gst_element_register (plugin, "cdparanoiasrc", GST_RANK_SECONDARY,
          GST_TYPE_CD_PARANOIA_SRC))
    return FALSE;

#ifdef ENABLE_NLS
  GST_DEBUG ("binding text domain %s to locale dir %s", GETTEXT_PACKAGE,
      LOCALEDIR);
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif


  return TRUE;
}


GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "cdparanoia",
    "Read audio from CD in paranoid mode",
    plugin_init, VERSION, "GPL", GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
