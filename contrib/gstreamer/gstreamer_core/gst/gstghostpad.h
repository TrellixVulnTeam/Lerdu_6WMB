/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wim.taymans@chello.be>
 *                    2005 Andy Wingo <wingo@pobox.com>
 *
 * gstghostpad.h: Proxy pads
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


#ifndef __GST_GHOST_PAD_H__
#define __GST_GHOST_PAD_H__


#include <gst/gstpad.h>


G_BEGIN_DECLS

#define GST_TYPE_GHOST_PAD		(gst_ghost_pad_get_type ())
#define GST_IS_GHOST_PAD(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_GHOST_PAD))
#define GST_IS_GHOST_PAD_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_GHOST_PAD))
#define GST_GHOST_PAD(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_GHOST_PAD, GstGhostPad))
#define GST_GHOST_PAD_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_GHOST_PAD, GstGhostPadClass))
#define GST_GHOST_PAD_CAST(obj)		((GstGhostPad*)(obj))

/**
 * GstGhostPad:
 *
 * Opaque #GstGhostPad structure.
 */
typedef struct _GstGhostPad GstGhostPad;
typedef struct _GstGhostPadClass GstGhostPadClass;
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GType		 gst_ghost_pad_get_type 	(void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstPad*		 gst_ghost_pad_new		(const gchar *name, GstPad *target);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstPad*		 gst_ghost_pad_new_no_target	(const gchar *name, GstPadDirection dir);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstPad*		 gst_ghost_pad_new_from_template (const gchar *name, GstPad * target, GstPadTemplate * templ);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstPad*		 gst_ghost_pad_new_no_target_from_template (const gchar *name, GstPadTemplate * templ);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstPad*		 gst_ghost_pad_get_target	(GstGhostPad *gpad);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean	 gst_ghost_pad_set_target	(GstGhostPad *gpad, GstPad *newtarget);

G_END_DECLS

#endif /* __GST_GHOST_PAD_H__ */
