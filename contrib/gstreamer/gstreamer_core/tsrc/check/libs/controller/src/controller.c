/* GStreamer
 *
 * unit test for the controller library
 *
 * Copyright (C) <2005> Stefan Kost <ensonic at users dot sf dot net>
 * Copyright (C) <2006-2007> Sebastian Dröge <slomo@circular-chaos.org>
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


#include "libgstreamer_wsd_solution.h" 

#define LOG_FILE "c:\\logs\\controller_logs.txt"
#include <gst/gst_global.h> 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

#include <gst/gst.h>
#include <gst/check/gstcheck.h>
#include <gst/controller/gstcontroller.h>
#include <gst/controller/gstcontrolsource.h>
#include <gst/controller/gstinterpolationcontrolsource.h>
#include <gst/controller/gstlfocontrolsource.h>

#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_critical,gstcheck,gboolean)
#define _gst_check_raised_critical (*GET_GSTREAMER_WSD_VAR_NAME(raised_critical,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_critical ;
#endif
//gboolean _gst_check_raised_warning = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(raised_warning,gstcheck,gboolean)
#define _gst_check_raised_warning (*GET_GSTREAMER_WSD_VAR_NAME(raised_warning,gstcheck,g)())
#else 
extern gboolean _gst_check_raised_warning ;
#endif
//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(expecting_log,gstcheck,gboolean)
#define _gst_check_expecting_log (*GET_GSTREAMER_WSD_VAR_NAME(expecting_log,gstcheck,g)())
#else 
extern gboolean _gst_check_expecting_log ;
#endif

//gboolean _gst_check_expecting_log = FALSE;
#if EMULATOR
static GET_GLOBAL_VAR_FROM_TLS(threads_running,gstcheck,gboolean)
#define _gst_check_threads_running (*GET_GSTREAMER_WSD_VAR_NAME(threads_running,gstcheck,g)())
#else 
extern gboolean _gst_check_threads_running ;
#endif


#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(buffers,gstcheck,GList*)
#define buffers (*GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g)())
#else 
extern GList *buffers;
#endif


#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(thread_list,gstcheck,GList*)
#define thread_list (*GET_GSTREAMER_WSD_VAR_NAME(thread_list,gstcheck,g)())
#else 
extern GList *thread_list;
#endif

//GMutex *mutex;
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(mutex,gstcheck,GMutex*)
#define mutex (*GET_GSTREAMER_WSD_VAR_NAME(mutex,gstcheck,g)())
#else 
extern GMutex *mutex;
#endif

//GCond *start_cond;              /* used to notify main thread of thread startups */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(start_cond,gstcheck,GCond*)
#define start_cond (*GET_GSTREAMER_WSD_VAR_NAME(start_cond,gstcheck,g)())
#else 
extern GCond *start_cond;
#endif

//GCond *sync_cond;               /* used to synchronize all threads and main thread */
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(sync_cond,gstcheck,GCond*)
#define sync_cond (*GET_GSTREAMER_WSD_VAR_NAME(sync_cond,gstcheck,g)())
#else 
extern GCond *sync_cond;
#endif


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}


/* LOCAL TEST ELEMENT */

enum
{
  ARG_ULONG = 1,
  ARG_FLOAT,
  ARG_DOUBLE,
  ARG_BOOLEAN,
  ARG_READONLY,
  ARG_STATIC,
  ARG_CONSTRUCTONLY,
  ARG_COUNT
};

#define GST_TYPE_TEST_MONO_SOURCE            (gst_test_mono_source_get_type ())
#define GST_TEST_MONO_SOURCE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_TEST_MONO_SOURCE, GstTestMonoSource))
#define GST_TEST_MONO_SOURCE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_TEST_MONO_SOURCE, GstTestMonoSourceClass))
#define GST_IS_TEST_MONO_SOURCE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_TEST_MONO_SOURCE))
#define GST_IS_TEST_MONO_SOURCE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_TEST_MONO_SOURCE))
#define GST_TEST_MONO_SOURCE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_TEST_MONO_SOURCE, GstTestMonoSourceClass))

typedef struct _GstTestMonoSource GstTestMonoSource;
typedef struct _GstTestMonoSourceClass GstTestMonoSourceClass;

struct _GstTestMonoSource
{
  GstElement parent;
  gulong val_ulong;
  gfloat val_float;
  gdouble val_double;
  gboolean val_boolean;
};
struct _GstTestMonoSourceClass
{
  GstElementClass parent_class;
};

static GType gst_test_mono_source_get_type (void);

static void
gst_test_mono_source_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec)
{
  GstTestMonoSource *self = GST_TEST_MONO_SOURCE (object);

  switch (property_id) {
    case ARG_ULONG:
      g_value_set_ulong (value, self->val_ulong);
      break;
    case ARG_FLOAT:
      g_value_set_float (value, self->val_float);
      break;
    case ARG_DOUBLE:
      g_value_set_double (value, self->val_double);
      break;
    case ARG_BOOLEAN:
      g_value_set_boolean (value, self->val_boolean);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gst_test_mono_source_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec)
{
  GstTestMonoSource *self = GST_TEST_MONO_SOURCE (object);

  switch (property_id) {
    case ARG_ULONG:
      self->val_ulong = g_value_get_ulong (value);
      break;
    case ARG_FLOAT:
      self->val_float = g_value_get_float (value);
      break;
    case ARG_DOUBLE:
      self->val_double = g_value_get_double (value);
      break;
    case ARG_BOOLEAN:
      self->val_boolean = g_value_get_boolean (value);
      break;
    case ARG_CONSTRUCTONLY:
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
gst_test_mono_source_class_init (GstTestMonoSourceClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gst_test_mono_source_set_property;
  gobject_class->get_property = gst_test_mono_source_get_property;

  g_object_class_install_property (gobject_class, ARG_ULONG,
      g_param_spec_ulong ("ulong",
          "ulong prop",
          "ulong number parameter for the test_mono_source",
          0, G_MAXULONG, 0, G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE));

  g_object_class_install_property (gobject_class, ARG_FLOAT,
      g_param_spec_float ("float",
          "float prop",
          "float number parameter for the test_mono_source",
          0.0, 100.0, 0.0, G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE));

  g_object_class_install_property (gobject_class, ARG_DOUBLE,
      g_param_spec_double ("double",
          "double prop",
          "double number parameter for the test_mono_source",
          0.0, 100.0, 0.0, G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE));

  g_object_class_install_property (gobject_class, ARG_BOOLEAN,
      g_param_spec_boolean ("boolean",
          "boolean prop",
          "boolean parameter for the test_mono_source",
          FALSE, G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE));

  g_object_class_install_property (gobject_class, ARG_READONLY,
      g_param_spec_ulong ("readonly",
          "readonly prop",
          "readonly parameter for the test_mono_source",
          0, G_MAXULONG, 0, G_PARAM_READABLE | GST_PARAM_CONTROLLABLE));

  g_object_class_install_property (gobject_class, ARG_STATIC,
      g_param_spec_ulong ("static",
          "static prop",
          "static parameter for the test_mono_source",
          0, G_MAXULONG, 0, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, ARG_CONSTRUCTONLY,
      g_param_spec_ulong ("construct-only",
          "construct-only prop",
          "construct-only parameter for the test_mono_source",
          0, G_MAXULONG, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void
gst_test_mono_source_base_init (GstTestMonoSourceClass * klass)
{
  static const GstElementDetails details = {
    "Monophonic source for unit tests",
    "Source/Audio/MonoSource",
    "Use in unit tests",
    "Stefan Kost <ensonic@users.sf.net>"
  };
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  gst_element_class_set_details (element_class, &details);
}

static GType
gst_test_mono_source_get_type (void)
{
  static GType type = 0;

  if (type == 0) {
    static const GTypeInfo info = {
      (guint16) sizeof (GstTestMonoSourceClass),
      (GBaseInitFunc) gst_test_mono_source_base_init,   // base_init
      NULL,                     // base_finalize
      (GClassInitFunc) gst_test_mono_source_class_init, // class_init
      NULL,                     // class_finalize
      NULL,                     // class_data
      (guint16) sizeof (GstTestMonoSource),
      0,                        // n_preallocs
      NULL,                     // instance_init
      NULL                      // value_table
    };
    type =
        g_type_register_static (GST_TYPE_ELEMENT, "GstTestMonoSource", &info,
        0);
  }
  return type;
}

/* so we don't have to paste the gst_element_register into 50 places below */
static gboolean
local_gst_controller_init (int *argc, char ***argv)
{
  fail_unless (gst_controller_init (argc, argv));

  fail_unless (gst_element_register (NULL, "testmonosource", GST_RANK_NONE,
          GST_TYPE_TEST_MONO_SOURCE));

  return TRUE;
}

#define gst_controller_init(a,b) local_gst_controller_init(a,b)

/* TESTS */
/* double init should not harm */
void controller_init()
{
	xmlfile = "controller_init";
  std_log(LOG_FILENAME_LINE, "Test Started controller_init");
  
  gst_controller_init (NULL, NULL);
  gst_controller_init (NULL, NULL);
  gst_controller_init (NULL, NULL);
  gst_controller_init (NULL, NULL);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* tests for an element with no controlled params */
void controller_new_fail1()
{
  GstController *ctrl;
  GstElement *elem;

		xmlfile = "controller_new_fail1";
  std_log(LOG_FILENAME_LINE, "Test Started controller_new_fail1");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("fakesrc", "test_source");

  /* that property should not exist */
  ctrl = gst_controller_new (G_OBJECT (elem), "_schrompf_", NULL);
  fail_unless (ctrl == NULL, NULL);

  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
}



/* tests for an element with controlled params, but none given */
void controller_new_fail2()
{
  GstController *ctrl;
  GstElement *elem;

		xmlfile = "controller_new_fail2";
  std_log(LOG_FILENAME_LINE, "Test Started controller_new_fail2");


  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* no property given */
  ctrl = gst_controller_new (G_OBJECT (elem), NULL);
  fail_unless (ctrl == NULL, NULL);

  gst_object_unref (elem);
  
     std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* tests for readonly params */
void controller_new_fail3()
{
  GstController *ctrl;
  GstElement *elem;
  
  		xmlfile = "controller_new_fail3";
  std_log(LOG_FILENAME_LINE, "Test Started controller_new_fail3");



  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and but is readonly */
  ASSERT_CRITICAL (ctrl =
      gst_controller_new (G_OBJECT (elem), "readonly", NULL));
  fail_unless (ctrl == NULL, NULL);

  gst_object_unref (elem);
  
     std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* tests for static params */
void controller_new_fail4()
{
  GstController *ctrl;
  GstElement *elem;


		xmlfile = "controller_new_fail4";
  std_log(LOG_FILENAME_LINE, "Test Started controller_new_fail4");


  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and but is not controlable */
  ASSERT_CRITICAL (ctrl = gst_controller_new (G_OBJECT (elem), "static", NULL));
  fail_unless (ctrl == NULL, NULL);

  gst_object_unref (elem);
  
     std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* tests for static params */
void controller_new_fail5()
{
  GstController *ctrl;
  GstElement *elem;

		xmlfile = "controller_new_fail5";
  std_log(LOG_FILENAME_LINE, "Test Started controller_new_fail5");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and but is construct-only */
  ASSERT_CRITICAL (ctrl =
      gst_controller_new (G_OBJECT (elem), "construct-only", NULL));
  fail_unless (ctrl == NULL, NULL);

  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}




/* tests for an element with controlled params */
void controller_new_okay1()
{
  GstController *ctrl;
  GstElement *elem;

	xmlfile = "controller_new_okay1";
  std_log(LOG_FILENAME_LINE, "Test Started controller_new_okay1");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* tests for an element with several controlled params */
void controller_new_okay2()
{
  GstController *ctrl, *ctrl2, *ctrl3;
  GstElement *elem;

		xmlfile = "controller_new_okay2";
  std_log(LOG_FILENAME_LINE, "Test Started controller_new_okay2");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", "double", "float", NULL);
  fail_unless (ctrl != NULL, NULL);

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  fail_unless_equals_int (G_OBJECT (ctrl)->ref_count, 1);

  ctrl2 = gst_controller_new (G_OBJECT (elem), "boolean", NULL);
  fail_unless (ctrl2 != NULL, NULL);
  fail_unless (ctrl2 == ctrl, NULL);

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  fail_unless_equals_int (G_OBJECT (ctrl)->ref_count, 2);

  /* trying to control the same properties again should correctly
   * increase the refcount of the object returned as well */
  ctrl3 =
      gst_controller_new (G_OBJECT (elem), "ulong", "double", "float", NULL);
  fail_unless (ctrl3 != NULL, NULL);
  fail_unless (ctrl3 == ctrl, NULL);

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  fail_unless_equals_int (G_OBJECT (ctrl)->ref_count, 3);

  g_object_unref (ctrl);
  g_object_unref (ctrl2);
  g_object_unref (ctrl3);
  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* controlling several params should return the same controller */
void controller_new_okay3()
{
  GstController *ctrl1, *ctrl2, *ctrl3;
  GstElement *elem;

		xmlfile = "controller_new_okay3";
  std_log(LOG_FILENAME_LINE, "Test Started controller_new_okay3");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl1 = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl1 != NULL, NULL);

  /* that property should exist and should be controllable */
  ctrl2 = gst_controller_new (G_OBJECT (elem), "double", NULL);
  fail_unless (ctrl2 != NULL, NULL);
  fail_unless (ctrl1 == ctrl2, NULL);

  /* that property should exist and should be controllable */
  ctrl3 = gst_controller_new (G_OBJECT (elem), "float", NULL);
  fail_unless (ctrl3 != NULL, NULL);
  fail_unless (ctrl1 == ctrl3, NULL);

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl1)->ref_count);
  fail_unless_equals_int (G_OBJECT (ctrl1)->ref_count, 3);
  g_object_unref (ctrl1);
  g_object_unref (ctrl2);
  g_object_unref (ctrl3);
  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* controlling a params twice should be handled */
void controller_param_twice()
{
  GstController *ctrl;
  GstElement *elem;
  gboolean res;

			xmlfile = "controller_param_twice";
  std_log(LOG_FILENAME_LINE, "Test Started controller_param_twice");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* it should have been added at least once, let remove it */
  res = gst_controller_remove_properties (ctrl, "ulong", NULL);
  fail_unless (res, NULL);

  /* removing it agian should not work */
  res = gst_controller_remove_properties (ctrl, "ulong", NULL);
  fail_unless (!res, NULL);

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* tests if we cleanup properly */
void controller_finalize()
{
  GstController *ctrl;
  GstElement *elem;

			xmlfile = "controller_finalize";
  std_log(LOG_FILENAME_LINE, "Test Started controller_finalize");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* free the controller */
  g_object_unref (ctrl);

  /* object shouldn't have a controller anymore */
  ctrl = gst_object_get_controller (G_OBJECT (elem));
  fail_unless (ctrl == NULL, NULL);

  gst_object_unref (elem);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* tests if we cleanup properly */
void controller_controlsource_refcounts()
{
  GstController *ctrl;
  GstElement *elem;
  GstControlSource *csource, *test_csource;

		xmlfile = "controller_controlsource_refcounts";
  std_log(LOG_FILENAME_LINE, "Test Started controller_controlsource_refcounts");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  csource = (GstControlSource *) gst_interpolation_control_source_new ();
  fail_unless (csource != NULL, NULL);

  fail_unless_equals_int (G_OBJECT (csource)->ref_count, 1);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong", csource));
  fail_unless_equals_int (G_OBJECT (csource)->ref_count, 2);

  g_object_unref (G_OBJECT (csource));

  test_csource = gst_controller_get_control_source (ctrl, "ulong");
  fail_unless (test_csource != NULL, NULL);
  fail_unless (test_csource == csource);
  fail_unless_equals_int (G_OBJECT (csource)->ref_count, 2);
  g_object_unref (csource);

  /* free the controller */
  g_object_unref (ctrl);

  /* object shouldn't have a controller anymore */
  ctrl = gst_object_get_controller (G_OBJECT (elem));
  fail_unless (ctrl == NULL, NULL);

  gst_object_unref (elem);
  
   std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* test timed value handling without interpolation */
void controller_interpolate_none()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstElement *elem;
  gboolean res;
  GValue val_ulong = { 0, };
  
  		xmlfile = "controller_interpolate_none";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolate_none");
  

  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_NONE));

  fail_unless (gst_interpolation_control_source_get_count (csource) == 0);

  /* set control values */
  g_value_init (&val_ulong, G_TYPE_ULONG);
  g_value_set_ulong (&val_ulong, 0);
  res =
      gst_interpolation_control_source_set (csource, 0 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  fail_unless (gst_interpolation_control_source_get_count (csource) == 1);
  g_value_set_ulong (&val_ulong, 100);
  res =
      gst_interpolation_control_source_set (csource, 2 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  fail_unless (gst_interpolation_control_source_get_count (csource) == 2);

  g_object_unref (G_OBJECT (csource));

  /* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
   std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* test timed value handling in trigger mode */
void controller_interpolate_trigger()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstElement *elem;
  gboolean res;
  GValue val_ulong = { 0, };

			xmlfile = "controller_interpolate_trigger";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolate_trigger");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_TRIGGER));

  g_value_init (&val_ulong, G_TYPE_ULONG);
  fail_if (gst_control_source_get_value (GST_CONTROL_SOURCE (csource),
          0 * GST_SECOND, &val_ulong));

  /* set control values */
  g_value_set_ulong (&val_ulong, 50);
  res =
      gst_interpolation_control_source_set (csource, 0 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  g_value_set_ulong (&val_ulong, 100);
  res =
      gst_interpolation_control_source_set (csource, 2 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);


  /* now pull in values for some timestamps */
  fail_unless (gst_control_source_get_value (GST_CONTROL_SOURCE (csource),
          0 * GST_SECOND, &val_ulong));
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  fail_unless (gst_control_source_get_value (GST_CONTROL_SOURCE (csource),
          1 * GST_SECOND, &val_ulong));
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (gst_control_source_get_value (GST_CONTROL_SOURCE (csource),
          2 * GST_SECOND, &val_ulong));
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (G_OBJECT (csource));
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* test timed value handling with linear interpolation */
void controller_interpolate_linear()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstElement *elem;
  gboolean res;
  GValue val_ulong = { 0, };
  
  	xmlfile = "controller_interpolate_linear";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolate_linear");
  

  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_LINEAR));

  /* set control values */
  g_value_init (&val_ulong, G_TYPE_ULONG);
  g_value_set_ulong (&val_ulong, 0);
  res =
      gst_interpolation_control_source_set (csource, 0 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  g_value_set_ulong (&val_ulong, 100);
  res =
      gst_interpolation_control_source_set (csource, 2 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);

  g_object_unref (G_OBJECT (csource));

  /* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
   std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* test timed value handling with cubic interpolation */
void controller_interpolate_cubic()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstElement *elem;
  gboolean res;
  GValue val_double = { 0, };

	 	xmlfile = "controller_interpolate_cubic";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolate_cubic");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "double", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "double",
          GST_CONTROL_SOURCE (csource)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_CUBIC));

  /* set control values */
  g_value_init (&val_double, G_TYPE_DOUBLE);
  g_value_set_double (&val_double, 0.0);
  res =
      gst_interpolation_control_source_set (csource, 0 * GST_SECOND,
      &val_double);
  fail_unless (res, NULL);
  g_value_set_double (&val_double, 5.0);
  res =
      gst_interpolation_control_source_set (csource, 1 * GST_SECOND,
      &val_double);
  fail_unless (res, NULL);
  g_value_set_double (&val_double, 2.0);
  res =
      gst_interpolation_control_source_set (csource, 2 * GST_SECOND,
      &val_double);
  fail_unless (res, NULL);
  g_value_set_double (&val_double, 8.0);
  res =
      gst_interpolation_control_source_set (csource, 4 * GST_SECOND,
      &val_double);
  fail_unless (res, NULL);

  g_object_unref (G_OBJECT (csource));

  /* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_double, 0.0);
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_double, 5.0);
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_double, 2.0);
  gst_controller_sync_values (ctrl, 3 * GST_SECOND);
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double > 2.0 &&
      GST_TEST_MONO_SOURCE (elem)->val_double < 8.0, NULL);
  gst_controller_sync_values (ctrl, 4 * GST_SECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_double, 8.0);
  gst_controller_sync_values (ctrl, 5 * GST_SECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_double, 8.0);

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* make sure we don't crash when someone sets an unsupported interpolation
 * mode */
void controller_interpolate_unimplemented()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstElement *elem;

		xmlfile = "controller_interpolate_unimplemented";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolate_unimplemented");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set completely bogus interpolation mode */
  fail_if (gst_interpolation_control_source_set_interpolation_mode (csource,
          (GstInterpolateMode) 93871));

  g_object_unref (G_OBJECT (csource));

  g_object_unref (ctrl);
  gst_object_unref (elem);
  
     std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* test _unset() */
void controller_interpolation_unset()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstElement *elem;
  gboolean res;
  GValue val_ulong = { 0, };

			xmlfile = "controller_interpolation_unset";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolation_unset");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_NONE));

  /* set control values */
  g_value_init (&val_ulong, G_TYPE_ULONG);
  g_value_set_ulong (&val_ulong, 0);
  res =
      gst_interpolation_control_source_set (csource, 0 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  g_value_set_ulong (&val_ulong, 100);
  res =
      gst_interpolation_control_source_set (csource, 1 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  g_value_set_ulong (&val_ulong, 50);
  res =
      gst_interpolation_control_source_set (csource, 2 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);

  /* verify values */
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;

  /* unset second */
  res = gst_interpolation_control_source_unset (csource, 1 * GST_SECOND);
  fail_unless (res, NULL);

  g_object_unref (csource);

  /* verify value again */
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
   std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* test _unset_all() */
void controller_interpolation_unset_all()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstElement *elem;
  gboolean res;
  GValue val_ulong = { 0, };
  
  	xmlfile = "controller_interpolation_unset_all";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolation_unset_all");
  

  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_NONE));

  /* set control values */
  g_value_init (&val_ulong, G_TYPE_ULONG);
  g_value_set_ulong (&val_ulong, 0);
  res =
      gst_interpolation_control_source_set (csource, 0 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  g_value_set_ulong (&val_ulong, 100);
  res =
      gst_interpolation_control_source_set (csource, 1 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);

  /* verify values */
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;

  /* unset second */
  gst_interpolation_control_source_unset_all (csource);

  g_object_unref (csource);

  /* verify value again */
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* test retrieval of an array of values with get_value_array() */
void controller_interpolation_linear_value_array()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstElement *elem;
  gboolean res;
  GValue val_ulong = { 0, };
  GstValueArray values = { NULL, };

		xmlfile = "controller_interpolation_linear_value_array";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolation_linear_value_array");
  
	
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_LINEAR));

  /* set control values */
  g_value_init (&val_ulong, G_TYPE_ULONG);
  g_value_set_ulong (&val_ulong, 0);
  res =
      gst_interpolation_control_source_set (csource, 0 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  g_value_set_ulong (&val_ulong, 100);
  res =
      gst_interpolation_control_source_set (csource, 2 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);

  /* now pull in values for some timestamps */
  values.property_name = "ulong";
  values.nbsamples = 3;
  values.sample_interval = GST_SECOND;
  values.values = (gpointer) g_new (gulong, 3);

  fail_unless (gst_control_source_get_value_array (GST_CONTROL_SOURCE (csource),
          0, &values));
  fail_unless_equals_int (((gulong *) values.values)[0], 0);
  fail_unless_equals_int (((gulong *) values.values)[1], 50);
  fail_unless_equals_int (((gulong *) values.values)[2], 100);

  g_object_unref (csource);

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_free (values.values);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
   std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



/* test if values below minimum and above maximum are clipped */
void controller_interpolation_linear_invalid_values()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstElement *elem;
  gboolean res;
  GValue val_float = { 0, };
  
  		xmlfile = "controller_interpolation_linear_invalid_values";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolation_linear_invalid_values");
  

  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "float", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "float",
          GST_CONTROL_SOURCE (csource)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_LINEAR));

  /* set control values */
  g_value_init (&val_float, G_TYPE_FLOAT);
  g_value_set_float (&val_float, 200.0);
  res =
      gst_interpolation_control_source_set (csource, 0 * GST_SECOND,
      &val_float);
  fail_unless (res, NULL);
  g_value_set_float (&val_float, -200.0);
  res =
      gst_interpolation_control_source_set (csource, 4 * GST_SECOND,
      &val_float);
  fail_unless (res, NULL);

  g_object_unref (csource);

  /* now pull in values for some timestamps and see if clipping works */
  /* 200.0 */
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_float, 100.0);
  /* 100.0 */
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_float, 100.0);
  /* 50.0 */
  gst_controller_sync_values (ctrl, 1 * GST_SECOND + 500 * GST_MSECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_float, 50.0);
  /* 0.0 */
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_float, 0.0);
  /* -100.0 */
  gst_controller_sync_values (ctrl, 3 * GST_SECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_float, 0.0);
  /* -200.0 */
  gst_controller_sync_values (ctrl, 4 * GST_SECOND);
  fail_unless_equals_float (GST_TEST_MONO_SOURCE (elem)->val_float, 0.0);

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
   std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  
}



void controller_interpolation_linear_default_values()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstElement *elem;
  gboolean res;
  GValue val_ulong = { 0, };

	xmlfile = "controller_interpolation_linear_default_values";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolation_linear_default_values");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_LINEAR));

  /* Should fail if no value was set yet */
  g_value_init (&val_ulong, G_TYPE_ULONG);
  fail_if (gst_control_source_get_value (GST_CONTROL_SOURCE (csource),
          0 * GST_SECOND, &val_ulong));

  /* set control values */
  g_value_set_ulong (&val_ulong, 0);
  res =
      gst_interpolation_control_source_set (csource, 1 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  g_value_set_ulong (&val_ulong, 100);
  res =
      gst_interpolation_control_source_set (csource, 3 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);

  /* now pull in values for some timestamps */
  /* should give the value of the first control point for timestamps before it */
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  gst_controller_sync_values (ctrl, 3 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;

  /* set control values */
  g_value_set_ulong (&val_ulong, 0);
  res =
      gst_interpolation_control_source_set (csource, 0 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  g_value_set_ulong (&val_ulong, 100);
  res =
      gst_interpolation_control_source_set (csource, 2 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);

  /* unset the old ones */
  res = gst_interpolation_control_source_unset (csource, 1 * GST_SECOND);
  fail_unless (res, NULL);
  res = gst_interpolation_control_source_unset (csource, 3 * GST_SECOND);
  fail_unless (res, NULL);

  /* now pull in values for some timestamps */
  /* should now give our value for timestamp 0 */
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;

  g_object_unref (G_OBJECT (csource));

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}



/* test gst_controller_set_disabled() with linear interpolation */
void controller_interpolate_linear_disabled()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource, *csource2;
  GstElement *elem;
  gboolean res;
  GValue val_ulong = { 0, }
  , val_double = {
  0,};
  
  	
	xmlfile = "controller_interpolate_linear_disabled";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolate_linear_disabled");
  

  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", "double", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();
  csource2 = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));
  fail_unless (csource2 != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "double",
          GST_CONTROL_SOURCE (csource2)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_LINEAR));
  fail_unless (gst_interpolation_control_source_set_interpolation_mode
      (csource2, GST_INTERPOLATE_LINEAR));

  /* set control values */
  g_value_init (&val_ulong, G_TYPE_ULONG);
  g_value_set_ulong (&val_ulong, 0);
  res =
      gst_interpolation_control_source_set (csource, 0 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);
  g_value_set_ulong (&val_ulong, 100);
  res =
      gst_interpolation_control_source_set (csource, 2 * GST_SECOND,
      &val_ulong);
  fail_unless (res, NULL);

  g_object_unref (G_OBJECT (csource));

/* set control values */
  g_value_init (&val_double, G_TYPE_DOUBLE);
  g_value_set_double (&val_double, 2.0);
  res =
      gst_interpolation_control_source_set (csource2, 0 * GST_SECOND,
      &val_double);
  fail_unless (res, NULL);
  g_value_set_double (&val_double, 4.0);
  res =
      gst_interpolation_control_source_set (csource2, 2 * GST_SECOND,
      &val_double);
  fail_unless (res, NULL);

  g_object_unref (G_OBJECT (csource2));

  /* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 2.0, NULL);
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 3.0, NULL);
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 4.0, NULL);

  /* now pull in values for some timestamps, prop double disabled */
  GST_TEST_MONO_SOURCE (elem)->val_ulong = 0;
  GST_TEST_MONO_SOURCE (elem)->val_double = 0.0;
  gst_controller_set_property_disabled (ctrl, "double", TRUE);
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 0.0, NULL);
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 0.0, NULL);
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 0.0, NULL);

  /* now pull in values for some timestamps, after enabling double again */
  GST_TEST_MONO_SOURCE (elem)->val_ulong = 0;
  GST_TEST_MONO_SOURCE (elem)->val_double = 0.0;
  gst_controller_set_property_disabled (ctrl, "double", FALSE);
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 2.0, NULL);
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 3.0, NULL);
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 4.0, NULL);

  /* now pull in values for some timestamps, after disabling all props */
  GST_TEST_MONO_SOURCE (elem)->val_ulong = 0;
  GST_TEST_MONO_SOURCE (elem)->val_double = 0.0;
  gst_controller_set_disabled (ctrl, TRUE);
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 0.0, NULL);
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 0.0, NULL);
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 0.0, NULL);

  /* now pull in values for some timestamps, enabling double again */
  GST_TEST_MONO_SOURCE (elem)->val_ulong = 0;
  GST_TEST_MONO_SOURCE (elem)->val_double = 0.0;
  gst_controller_set_property_disabled (ctrl, "double", FALSE);
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 2.0, NULL);
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 3.0, NULL);
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 4.0, NULL);

  /* now pull in values for some timestamps, enabling all */
  GST_TEST_MONO_SOURCE (elem)->val_ulong = 0;
  GST_TEST_MONO_SOURCE (elem)->val_double = 0.0;
  gst_controller_set_disabled (ctrl, FALSE);
  gst_controller_sync_values (ctrl, 0 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 2.0, NULL);
  gst_controller_sync_values (ctrl, 1 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 3.0, NULL);
  gst_controller_sync_values (ctrl, 2 * GST_SECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  fail_unless (GST_TEST_MONO_SOURCE (elem)->val_double == 4.0, NULL);

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
   std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}




void controller_interpolation_set_from_list()
{
  GstController *ctrl;
  GstInterpolationControlSource *csource;
  GstTimedValue *tval;
  GstElement *elem;
  GSList *list = NULL;

		xmlfile = "controller_interpolation_set_from_list";
  std_log(LOG_FILENAME_LINE, "Test Started controller_interpolation_set_from_list");
  
  gst_controller_init (NULL, NULL);

  /* test that an invalid timestamp throws a warning of some sort */
  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_interpolation_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set interpolation mode */
  fail_unless (gst_interpolation_control_source_set_interpolation_mode (csource,
          GST_INTERPOLATE_LINEAR));

  /* set control value */
  tval = g_new0 (GstTimedValue, 1);
  tval->timestamp = GST_CLOCK_TIME_NONE;
  g_value_init (&tval->value, G_TYPE_ULONG);
  g_value_set_ulong (&tval->value, 0);

  list = g_slist_append (list, tval);

  fail_if (gst_interpolation_control_source_set_from_list (csource, list));

  /* try again with a valid stamp, should work now */
  tval->timestamp = 0;
  fail_unless (gst_interpolation_control_source_set_from_list (csource, list));

  g_object_unref (csource);

  /* allocated GstTimedValue now belongs to the controller, but list not */
  g_value_unset (&tval->value);
  g_free (tval);
  g_slist_free (list);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
   std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}



/* test lfo control source with sine waveform */
void controller_lfo_sine()
{
  GstController *ctrl;
  GstLFOControlSource *csource;
  GstElement *elem;
  GValue amp = { 0, }
  , off = {
  0,};

	
		xmlfile = "controller_lfo_sine";
  std_log(LOG_FILENAME_LINE, "Test Started controller_lfo_sine");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_lfo_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set amplitude and offset values */
  g_value_init (&amp, G_TYPE_ULONG);
  g_value_init (&off, G_TYPE_ULONG);
  g_value_set_ulong (&amp, 100);
  g_value_set_ulong (&off, 100);

  /* set waveform mode */
  g_object_set (csource, "waveform", GST_LFO_WAVEFORM_SINE,
      "frequency", 1.0, "timeshift", (GstClockTime) 0,
      "amplitude", &amp, "offset", &off, NULL);

  g_object_unref (G_OBJECT (csource));

  /* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 2000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}



/* test lfo control source with sine waveform and timeshift */
void controller_lfo_sine_timeshift()
{
  GstController *ctrl;
  GstLFOControlSource *csource;
  GstElement *elem;
  GValue amp = { 0, }
  , off = {
  0,};

			xmlfile = "controller_lfo_sine_timeshift";
  std_log(LOG_FILENAME_LINE, "Test Started controller_lfo_sine_timeshift");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_lfo_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set amplitude and offset values */
  g_value_init (&amp, G_TYPE_ULONG);
  g_value_init (&off, G_TYPE_ULONG);
  g_value_set_ulong (&amp, 100);
  g_value_set_ulong (&off, 100);

  /* set waveform mode */
  g_object_set (csource, "waveform", GST_LFO_WAVEFORM_SINE,
      "frequency", 1.0, "timeshift", 250 * GST_MSECOND,
      "amplitude", &amp, "offset", &off, NULL);

  g_object_unref (G_OBJECT (csource));

/* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 2000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
     std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
  
}




/* test lfo control source with square waveform */
void controller_lfo_square()
{
  GstController *ctrl;
  GstLFOControlSource *csource;
  GstElement *elem;
  GValue amp = { 0, }
  , off = {
  0,};

		xmlfile = "controller_lfo_square";
  std_log(LOG_FILENAME_LINE, "Test Started controller_lfo_square");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_lfo_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set amplitude and offset values */
  g_value_init (&amp, G_TYPE_ULONG);
  g_value_init (&off, G_TYPE_ULONG);
  g_value_set_ulong (&amp, 100);
  g_value_set_ulong (&off, 100);

  /* set waveform mode */
  g_object_set (csource, "waveform", GST_LFO_WAVEFORM_SQUARE,
      "frequency", 1.0, "timeshift", (GstClockTime) 0,
      "amplitude", &amp, "offset", &off, NULL);

  g_object_unref (G_OBJECT (csource));

  /* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 2000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}



/* test lfo control source with saw waveform */
void controller_lfo_saw()
{
  GstController *ctrl;
  GstLFOControlSource *csource;
  GstElement *elem;
  GValue amp = { 0, }
  , off = {
  0,};

	xmlfile = "controller_lfo_saw";
  std_log(LOG_FILENAME_LINE, "Test Started controller_lfo_saw");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_lfo_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set amplitude and offset values */
  g_value_init (&amp, G_TYPE_ULONG);
  g_value_init (&off, G_TYPE_ULONG);
  g_value_set_ulong (&amp, 100);
  g_value_set_ulong (&off, 100);

  /* set waveform mode */
  g_object_set (csource, "waveform", GST_LFO_WAVEFORM_SAW,
      "frequency", 1.0, "timeshift", (GstClockTime) 0,
      "amplitude", &amp, "offset", &off, NULL);

  g_object_unref (G_OBJECT (csource));

  /* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 150);;
  gst_controller_sync_values (ctrl, 500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  gst_controller_sync_values (ctrl, 1000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 150);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  gst_controller_sync_values (ctrl, 2000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 150);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
   std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}



/* test lfo control source with reverse saw waveform */
void controller_lfo_rsaw()
{
  GstController *ctrl;
  GstLFOControlSource *csource;
  GstElement *elem;
  GValue amp = { 0, }
  , off = {
  0,};
  
  	xmlfile = "controller_lfo_rsaw";
  std_log(LOG_FILENAME_LINE, "Test Started controller_lfo_rsaw");
  

  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_lfo_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set amplitude and offset values */
  g_value_init (&amp, G_TYPE_ULONG);
  g_value_init (&off, G_TYPE_ULONG);
  g_value_set_ulong (&amp, 100);
  g_value_set_ulong (&off, 100);

  /* set waveform mode */
  g_object_set (csource, "waveform", GST_LFO_WAVEFORM_REVERSE_SAW,
      "frequency", 1.0, "timeshift", (GstClockTime) 0,
      "amplitude", &amp, "offset", &off, NULL);

  g_object_unref (G_OBJECT (csource));

  /* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  gst_controller_sync_values (ctrl, 500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 150);;
  gst_controller_sync_values (ctrl, 1000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 150);;
  gst_controller_sync_values (ctrl, 2000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 50);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 150);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}



/* test lfo control source with saw waveform */
void controller_lfo_triangle()
{
  GstController *ctrl;
  GstLFOControlSource *csource;
  GstElement *elem;
  GValue amp = { 0, }
  , off = {
  0,};

	xmlfile = "controller_lfo_triangle";
  std_log(LOG_FILENAME_LINE, "Test Started controller_lfo_triangle");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_lfo_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  /* set amplitude and offset values */
  g_value_init (&amp, G_TYPE_ULONG);
  g_value_init (&off, G_TYPE_ULONG);
  g_value_set_ulong (&amp, 100);
  g_value_set_ulong (&off, 100);

  /* set waveform mode */
  g_object_set (csource, "waveform", GST_LFO_WAVEFORM_TRIANGLE,
      "frequency", 1.0, "timeshift", (GstClockTime) 0,
      "amplitude", &amp, "offset", &off, NULL);

  g_object_unref (G_OBJECT (csource));

  /* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 2000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 200);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 100);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}



/* test lfo control source with nothing set */
void controller_lfo_none()
{
  GstController *ctrl;
  GstLFOControlSource *csource;
  GstElement *elem;
  
  	xmlfile = "controller_lfo_none";
  std_log(LOG_FILENAME_LINE, "Test Started controller_lfo_none");
  

  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("testmonosource", "test_source");

  /* that property should exist and should be controllable */
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);

  /* Get interpolation control source */
  csource = gst_lfo_control_source_new ();

  fail_unless (csource != NULL);
  fail_unless (gst_controller_set_control_source (ctrl, "ulong",
          GST_CONTROL_SOURCE (csource)));

  g_object_unref (G_OBJECT (csource));

  /* now pull in values for some timestamps */
  gst_controller_sync_values (ctrl, 0 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 2000 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1250 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1500 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;
  gst_controller_sync_values (ctrl, 1750 * GST_MSECOND);
  fail_unless_equals_int (GST_TEST_MONO_SOURCE (elem)->val_ulong, 0);;

  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  g_object_unref (ctrl);
  gst_object_unref (elem);
  
    std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}



/* tests if we can run helper methods against any GObject */
void controller_helper_any_gobject()
{
  GstElement *elem;
  gboolean res;

  	xmlfile = "controller_helper_any_gobject";
  std_log(LOG_FILENAME_LINE, "Test Started controller_helper_any_gobject");
  
  gst_controller_init (NULL, NULL);

  elem = gst_element_factory_make ("bin", "test_elem");

  /* that element is not controllable */
  res = gst_object_sync_values (G_OBJECT (elem), 0LL);
  fail_unless (res == FALSE, NULL);

  gst_object_unref (elem);
     std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
  
}



void controller_refcount_new_list()
{
  GstController *ctrl, *ctrl2;
  GstElement *elem;
  GList *list = NULL;


  	xmlfile = "controller_refcount_new_list";
  std_log(LOG_FILENAME_LINE, "Test Started controller_refcount_new_list");
  
  
  gst_controller_init (NULL, NULL);

  /* that property should exist and should be controllable */
  elem = gst_element_factory_make ("testmonosource", "test_source");
  list = g_list_append (NULL, "ulong");
  ctrl = gst_controller_new_list (G_OBJECT (elem), list);
  fail_unless (ctrl != NULL, NULL);
  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  fail_unless_equals_int (G_OBJECT (ctrl)->ref_count, 1);
  g_list_free (list);
  g_object_unref (ctrl);
  gst_object_unref (elem);

  /* try the same property twice, make sure the refcount is still 1 */
  elem = gst_element_factory_make ("testmonosource", "test_source");
  list = g_list_append (NULL, "ulong");
  list = g_list_append (list, "ulong");
  ctrl = gst_controller_new_list (G_OBJECT (elem), list);
  fail_unless (ctrl != NULL, NULL);
  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  fail_unless_equals_int (G_OBJECT (ctrl)->ref_count, 1);
  g_list_free (list);
  g_object_unref (ctrl);
  gst_object_unref (elem);

  /* try two properties, make sure the refcount is still 1 */
  elem = gst_element_factory_make ("testmonosource", "test_source");
  list = g_list_append (NULL, "ulong");
  list = g_list_append (list, "boolean");
  ctrl = gst_controller_new_list (G_OBJECT (elem), list);
  fail_unless (ctrl != NULL, NULL);
  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  fail_unless_equals_int (G_OBJECT (ctrl)->ref_count, 1);
  g_list_free (list);
  g_object_unref (ctrl);
  gst_object_unref (elem);

  /* try _new_list with existing controller */
  elem = gst_element_factory_make ("testmonosource", "test_source");
  ctrl = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl != NULL, NULL);
  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  list = g_list_append (NULL, "ulong");
  ctrl2 = gst_controller_new_list (G_OBJECT (elem), list);
  fail_unless (ctrl2 != NULL, NULL);
  fail_unless (ctrl == ctrl2, NULL);
  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  fail_unless_equals_int (G_OBJECT (ctrl)->ref_count, 2);
  g_list_free (list);
  g_object_unref (ctrl);
  g_object_unref (ctrl2);
  gst_object_unref (elem);

  /* try _new_list first and then _new */
  elem = gst_element_factory_make ("testmonosource", "test_source");
  list = g_list_append (NULL, "ulong");
  ctrl = gst_controller_new_list (G_OBJECT (elem), list);
  fail_unless (ctrl != NULL, NULL);
  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  ctrl2 = gst_controller_new (G_OBJECT (elem), "ulong", NULL);
  fail_unless (ctrl2 != NULL, NULL);
  fail_unless (ctrl == ctrl2, NULL);
  GST_INFO ("controller->ref_count=%d", G_OBJECT (ctrl)->ref_count);
  fail_unless_equals_int (G_OBJECT (ctrl)->ref_count, 2);
  g_list_free (list);
  g_object_unref (ctrl);
  g_object_unref (ctrl2);
  gst_object_unref (elem);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  
}



void
gst_controller_suite (void)
{
controller_init();
controller_refcount_new_list();
controller_new_fail1();
controller_new_fail2();
controller_new_fail3();
controller_new_fail4();
controller_new_fail5();
controller_new_okay1();
controller_new_okay2();
controller_new_okay3();
controller_param_twice();
controller_finalize();
controller_controlsource_refcounts();
controller_interpolate_none();
controller_interpolate_trigger();
controller_interpolate_linear();
controller_interpolate_cubic();
controller_interpolate_unimplemented();
controller_interpolation_unset();
controller_interpolation_unset_all();
controller_interpolation_linear_value_array();
controller_interpolation_linear_invalid_values();
controller_interpolation_linear_default_values();
controller_interpolate_linear_disabled();
controller_interpolation_set_from_list();
controller_lfo_sine();
controller_lfo_sine_timeshift();
controller_lfo_square();
controller_lfo_saw();
controller_lfo_rsaw();
controller_lfo_triangle();
controller_lfo_none();
controller_helper_any_gobject();
}

void (*fn[]) (void) = {
controller_init,
controller_refcount_new_list,
controller_new_fail1,
controller_new_fail2,
controller_new_fail3,
controller_new_fail4,
controller_new_fail5,
controller_new_okay1,
controller_new_okay2,
controller_new_okay3,
controller_param_twice,
controller_finalize,
controller_controlsource_refcounts,
controller_interpolate_none,
controller_interpolate_trigger,
controller_interpolate_linear,
controller_interpolate_cubic,
controller_interpolate_unimplemented,
controller_interpolation_unset,
controller_interpolation_unset_all,
controller_interpolation_linear_value_array,
controller_interpolation_linear_invalid_values,
controller_interpolation_linear_default_values,
controller_interpolate_linear_disabled,
controller_interpolation_set_from_list,
controller_lfo_sine,
controller_lfo_sine_timeshift,
controller_lfo_square,
controller_lfo_saw,
controller_lfo_rsaw,
controller_lfo_triangle,
controller_lfo_none,
controller_helper_any_gobject
};

char *args[] = {
"controller_init",
"controller_refcount_new_list",
"controller_new_fail1",
"controller_new_fail2",
"controller_new_fail3",
"controller_new_fail4",
"controller_new_fail5",
"controller_new_okay1",
"controller_new_okay2",
"controller_new_okay3",
"controller_param_twice",
"controller_finalize",
"controller_controlsource_refcounts",
"controller_interpolate_none",
"controller_interpolate_trigger",
"controller_interpolate_linear",
"controller_interpolate_cubic",
"controller_interpolate_unimplemented",
"controller_interpolation_unset",
"controller_interpolation_unset_all",
"controller_interpolation_linear_value_array",
"controller_interpolation_linear_invalid_values",
"controller_interpolation_linear_default_values",
"controller_interpolate_linear_disabled",
"controller_interpolation_set_from_list",
"controller_lfo_sine",
"controller_lfo_sine_timeshift",
"controller_lfo_square",
"controller_lfo_saw",
"controller_lfo_rsaw",
"controller_lfo_triangle",
"controller_lfo_none",
"controller_helper_any_gobject"
};

GST_CHECK_MAIN (gst_controller);
#if 0 
int main(int argc,char** argv)
{
		int i;
		gst_init(&argc,&argv);
		for(i=0;i<33;i++)
			fn[i]();
			
} 
#endif