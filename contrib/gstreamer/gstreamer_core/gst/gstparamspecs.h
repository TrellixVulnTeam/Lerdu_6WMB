/* GStreamer - GParamSpecs for for some of our types
 * Copyright (C) 2007 Tim-Philipp Müller  <tim centricular net>
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

#ifndef __GST_PARAMSPECS_H__
#define __GST_PARAMSPECS_H__

#include <gst/gstvalue.h>

G_BEGIN_DECLS

/* --- type macros --- */

#define GST_TYPE_PARAM_FRACTION           (gst_param_spec_fraction_get_type ())
#define GST_IS_PARAM_SPEC_FRACTION(pspec) (G_TYPE_CHECK_INSTANCE_TYPE ((pspec), GST_TYPE_PARAM_FRACTION))
#define GST_PARAM_SPEC_FRACTION(pspec)    (G_TYPE_CHECK_INSTANCE_CAST ((pspec), GST_TYPE_PARAM_FRACTION, GstParamSpecFraction))


/* --- get_type functions --- */
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GType  gst_param_spec_fraction_get_type (void);


/* --- typedefs & structures --- */

typedef struct _GstParamSpecFraction GstParamSpecFraction;

/**
 * GstParamSpecFraction:
 * @parent_instance: super class
 * @min_num: minimal numerator
 * @min_den: minimal denominator
 * @max_num: maximal numerator
 * @max_den: maximal denominator
 * @def_num: default numerator
 * @def_den: default denominator
 *
 * A GParamSpec derived structure that contains the meta data for fractional
 * properties.
 */
struct _GstParamSpecFraction {
  GParamSpec    parent_instance;
  
  gint          min_num, min_den;
  gint          max_num, max_den;
  gint          def_num, def_den;
};


/* --- GParamSpec prototypes --- */
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GParamSpec  * gst_param_spec_fraction (const gchar * name,
                                       const gchar * nick,
                                       const gchar * blurb,
                                       gint min_num, gint min_denom,
                                       gint max_num, gint max_denom,
                                       gint default_num, gint default_denom,
                                       GParamFlags flags);

G_END_DECLS

#endif /* __GST_PARAMSPECS_H__ */

