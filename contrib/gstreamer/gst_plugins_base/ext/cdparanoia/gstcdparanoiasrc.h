/* GStreamer
 * Copyright (C) 1999 Erik Walthinsen <omega@cse.ogi.edu>
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

#ifndef __GST_CD_PARANOIA_SRC_H__
#define __GST_CD_PARANOIA_SRC_H__

#include "gst/cdda/gstcddabasesrc.h"

G_BEGIN_DECLS

#define size16 gint16
#define size32 gint32

#ifdef CDPARANOIA_HEADERS_IN_DIR
  #include <cdda/cdda_interface.h>
  #include <cdda/cdda_paranoia.h>
#else
  #include <cdda_interface.h>
  #include <cdda_paranoia.h>
#endif

#define GST_TYPE_CD_PARANOIA_SRC            (gst_cd_paranoia_src_get_type())
#define GST_CD_PARANOIA_SRC(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CD_PARANOIA_SRC,GstCdParanoiaSrc))
#define GST_CD_PARANOIA_SRC_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CD_PARANOIA_SRC,GstCdParanoiaSrcClass))
#define GST_IS_CD_PARANOIA_SRC(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CD_PARANOIA_SRC))
#define GST_IS_CD_PARANOIA_SRC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CD_PARANOIA_SRC))
#define GST_CD_PARANOIA_SRC_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_CDDA_BASAE_SRC, GstCdParanoiaSrcClass))

typedef struct _GstCdParanoiaSrc GstCdParanoiaSrc;
typedef struct _GstCdParanoiaSrcClass GstCdParanoiaSrcClass;

/**
 * GstCdParanoiaSrc:
 *
 * The cdparanoia object structure.
 */
struct _GstCdParanoiaSrc {
  GstCddaBaseSrc   cddabasesrc;

  /*< private >*/
  cdrom_drive     *d;
  cdrom_paranoia  *p;

  gint             next_sector; /* -1 or next sector we expect to
                                 * read, so we know when to do a seek */

  gint             paranoia_mode;
  gint             read_speed;
  gint             search_overlap;

  gchar           *generic_device;
};

struct _GstCdParanoiaSrcClass {
  GstCddaBaseSrcClass parent_class;

  /* signal callbacks */
  /**
   * GstCdParanoiaSrcClass::transport-error:
   * @src: the GstCddaBaseSrc source element object
   * @sector: the sector at which the error happened
   *
   * This signal is emitted when a sector could not be read
   * because of a transport error.
   */
  void (*transport_error)	(GstCdParanoiaSrc * src, gint sector);
  /**
   * GstCdParanoiaSrcClass::uncorrected-error:
   * @src: the GstCddaBaseSrc source element object
   * @sector: the sector at which the error happened
   *
   * This signal is emitted when a sector could not be read
   * because of a transport error.
   */
  void (*uncorrected_error)	(GstCdParanoiaSrc * src, gint sector);
};

GType    gst_cd_paranoia_src_get_type (void);

G_END_DECLS

#endif /* __GST_CD_PARANOIA_SRC_H__ */

