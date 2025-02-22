/* GStreamer
 * Copyright (C) 2004 Benjamin Otte <otte@gnome.org>
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

#include <gst/gst.h>

#ifndef __GST_ADAPTER_H__
#define __GST_ADAPTER_H__

G_BEGIN_DECLS


#define GST_TYPE_ADAPTER \
  (gst_adapter_get_type())
#define GST_ADAPTER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_ADAPTER, GstAdapter))
#define GST_ADAPTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_ADAPTER, GstAdapterClass))
#define GST_ADAPTER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_ADAPTER, GstAdapterClass))
#define GST_IS_ADAPTER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_ADAPTER))
#define GST_IS_ADAPTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_ADAPTER))

typedef struct _GstAdapter GstAdapter;
typedef struct _GstAdapterClass GstAdapterClass;

/**
 * GstAdapter:
 * @object: the parent object.
 *
 * The opaque #GstAdapter data structure.
 */
struct _GstAdapter {
  GObject	object;

  /*< private >*/
  GSList *	buflist;
  guint		size;
  guint		skip;

  /* we keep state of assembled pieces */
  guint8 *	assembled_data;
  guint		assembled_size;
  guint		assembled_len;

  /* Remember where the end of our buffer list is to
   * speed up the push */
  GSList *buflist_end;
  gpointer _gst_reserved[GST_PADDING - 1];
};

struct _GstAdapterClass {
  GObjectClass	parent_class;

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstAdapter *		gst_adapter_new			(void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void			gst_adapter_clear		(GstAdapter *adapter);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_adapter_push		(GstAdapter *adapter, GstBuffer* buf);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

const guint8 *		gst_adapter_peek      		(GstAdapter *adapter, guint size);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void    		gst_adapter_copy      		(GstAdapter *adapter, guint8 *dest,
                                                         guint offset, guint size);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_adapter_flush		(GstAdapter *adapter, guint flush);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint8*			gst_adapter_take		(GstAdapter *adapter, guint nbytes);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstBuffer*		gst_adapter_take_buffer		(GstAdapter *adapter, guint nbytes);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint			gst_adapter_available		(GstAdapter *adapter);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint			gst_adapter_available_fast    	(GstAdapter *adapter);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GType			gst_adapter_get_type		(void);

G_END_DECLS

#endif /* __GST_ADAPTER_H__ */
