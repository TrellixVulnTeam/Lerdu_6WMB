/*
 * audio-example.c
 *
 * Builds a pipeline with audiotestsource->alsasink and sweeps frequency and
 * volume.
 *
 * Needs gst-plugin-base installed.
 */


#include <gst/gst_global.h>
#include <gst/gst.h>
#include <gst/controller/gstcontroller.h>
#include <gst/controller/gstinterpolationcontrolsource.h>

#define LOG_FILE "c:\\logs\\audio_example_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__


void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}
gint
main (gint argc, gchar ** argv)
{
  gint res = 1;
  GstElement *src, *sink;
  GstElement *bin;
  GstController *ctrl;
  GstInterpolationControlSource *csource1, *csource2;
  GstClock *clock;
  GstClockID clock_id;
  GstClockReturn wait_ret;
  GValue vol = { 0, };
	xmlfile = "audio_example_logs";
  std_log(LOG_FILENAME_LINE, "Test Started audio_example");
  
  gst_init (&argc, &argv);
  
  gst_controller_init (&argc, &argv);
  /* build pipeline */
  bin = gst_pipeline_new ("pipeline");
  clock = gst_pipeline_get_clock (GST_PIPELINE (bin));
  src = gst_element_factory_make ("audiotestsrc", "gen_audio");
  sink = gst_element_factory_make ("devsoundsink", "play_audio");
  
  
  gst_bin_add_many (GST_BIN (bin), src, sink, NULL);
  if (!gst_element_link (src, sink)) {
    GST_WARNING ("can't link elements");
    goto Error;
  }
  
  /* square wave
     g_object_set (G_OBJECT(src), "wave", 1, NULL);
   */

  /* add a controller to the source */
  if (!(ctrl = gst_controller_new (G_OBJECT (src), "freq", "volume", NULL))) {
    GST_WARNING ("can't control source element");
    goto Error;
  }

  csource1 = gst_interpolation_control_source_new ();
  csource2 = gst_interpolation_control_source_new ();

      
  gst_controller_set_control_source (ctrl, "volume",
      GST_CONTROL_SOURCE (csource1));
  gst_controller_set_control_source (ctrl, "freq",
      GST_CONTROL_SOURCE (csource2));

  
  /* Set interpolation mode */

  gst_interpolation_control_source_set_interpolation_mode (csource1,
      GST_INTERPOLATE_LINEAR);
  gst_interpolation_control_source_set_interpolation_mode (csource2,
      GST_INTERPOLATE_LINEAR);

  
  /* set control values */
  g_value_init (&vol, G_TYPE_DOUBLE);
  g_value_set_double (&vol, 0.0);
  gst_interpolation_control_source_set (csource1, 0 * GST_SECOND, &vol);
  g_value_set_double (&vol, 1.0);
  gst_interpolation_control_source_set (csource1, 5 * GST_SECOND, &vol);

  
  g_object_unref (csource1);

  g_value_set_double (&vol, 220.0);
  gst_interpolation_control_source_set (csource2, 0 * GST_SECOND, &vol);
  g_value_set_double (&vol, 3520.0);
  gst_interpolation_control_source_set (csource2, 3 * GST_SECOND, &vol);
  g_value_set_double (&vol, 440.0); 
  gst_interpolation_control_source_set (csource2, 6 * GST_SECOND, &vol);

  
  g_object_unref (csource2);

  clock_id =
      gst_clock_new_single_shot_id (clock,
      gst_clock_get_time (clock) + (7 * GST_SECOND));

  
  
  /* run for 7 seconds */
  if (gst_element_set_state (bin, GST_STATE_PLAYING)) {
    if ((wait_ret = gst_clock_id_wait (clock_id, NULL)) != GST_CLOCK_OK) {
      GST_WARNING ("clock_id_wait returned: %d", wait_ret);
    }
    gst_element_set_state (bin, GST_STATE_NULL);
  }

  /* cleanup */
  g_object_unref (G_OBJECT (ctrl));
  gst_object_unref (G_OBJECT (clock));
  gst_object_unref (G_OBJECT (bin));
  res = 0;
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0); 
  return (res);
Error:
  std_log(LOG_FILENAME_LINE, "Test failed");
  create_xml(1); 
  return (res);
}
