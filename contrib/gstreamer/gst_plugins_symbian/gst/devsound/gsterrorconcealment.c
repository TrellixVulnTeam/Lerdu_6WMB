/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:
*
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "gsterrorconcealmentinterface.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include <gobject_global.h>

#endif

static void gst_error_concealment_base_init (gpointer g_class);

EXPORT_C GType gst_error_concealment_get_type (void)
{
  static GType error_concealment_type = 0;

  if (G_UNLIKELY (error_concealment_type == 0)) {
    static const GTypeInfo error_concealment_info = {
      sizeof (GstErrorConcealmentIntfc),  /* class_size */
      gst_error_concealment_base_init,  /* base_init */
      NULL,                     /* base_finalize */
      NULL,
      NULL,                     /* class_finalize */
      NULL,                     /* class_data */
      0,
      0,
      NULL
    };

  
    error_concealment_type = g_type_register_static (G_TYPE_INTERFACE, "GstErrorConcealment",
        &error_concealment_info, 0);

  }

  return error_concealment_type;
}

static void gst_error_concealment_base_init (gpointer g_class)
    {
    static gboolean initialized = FALSE;
    
    if (G_UNLIKELY (!initialized)) 
        {
        initialized = TRUE;
        }
    }
