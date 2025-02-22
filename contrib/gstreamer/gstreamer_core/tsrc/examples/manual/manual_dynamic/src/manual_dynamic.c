
/*** block a  from ../../../docs/manual/advanced-autoplugging.xml ***/

#include <gst/gst_global.h>
#include <gst/gst.h>
#include <stdio.h>

#define LOG_FILE "c:\\logs\\dynamic_logs.txt" 
#include "std_log_result.h" 
#define LOG_FILENAME_LINE __FILE__, __LINE__

GstElement *pipeline;

/*** block b  from ../../../docs/manual/advanced-autoplugging.xml ***/
static GList *factories;

/*
 * This function is called by the registry loader. Its return value
 * (TRUE or FALSE) decides whether the given feature will be included
 * in the list that we're generating further down.
 */

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml(xmlfile);
    close_log_file();
}
static gboolean
cb_feature_filter (GstPluginFeature *feature,
		   gpointer          data)
{
  const gchar *klass;
  guint rank;

  /* we only care about element factories */
  if (!GST_IS_ELEMENT_FACTORY (feature))
    return FALSE;

  /* only parsers, demuxers and decoders */
  klass = gst_element_factory_get_klass (GST_ELEMENT_FACTORY (feature));
  if (g_strrstr (klass, "Demux") == NULL &&
      g_strrstr (klass, "Decoder") == NULL &&
      g_strrstr (klass, "Parse") == NULL)
    return FALSE;

  /* only select elements with autoplugging rank */
  rank = gst_plugin_feature_get_rank (feature);
  if (rank < GST_RANK_MARGINAL)
    return FALSE;

  return TRUE;
}

/*
 * This function is called to sort features by rank.
 */

static gint
cb_compare_ranks (GstPluginFeature *f1,
		  GstPluginFeature *f2)
{
  return gst_plugin_feature_get_rank (f2) - gst_plugin_feature_get_rank (f1);
}

static void
init_factories (void)
{
  /* first filter out the interesting element factories */
  factories = gst_registry_feature_filter (
      gst_registry_get_default (),
      (GstPluginFeatureFilter) cb_feature_filter, FALSE, NULL);

  /* sort them according to their ranks */
  factories = g_list_sort (factories, (GCompareFunc) cb_compare_ranks);
}

/*** block c  from ../../../docs/manual/advanced-autoplugging.xml ***/
static void try_to_plug (GstPad *pad, const GstCaps *caps);

static GstElement *audiosink;

static void
cb_newpad (GstElement *element,
	   GstPad     *pad,
	   gpointer    data)
{
  GstCaps *caps;

  caps = gst_pad_get_caps (pad);
  try_to_plug (pad, caps);
  gst_caps_unref (caps);
}

static void
close_link (GstPad      *srcpad,
	    GstElement  *sinkelement,
	    const gchar *padname,
	    const GList *templlist)
{
  GstPad *pad;
  gboolean has_dynamic_pads = FALSE;

  g_print ("Plugging pad %s:%s to newly created %s:%s\n",
	   gst_object_get_name (GST_OBJECT (gst_pad_get_parent (srcpad))),
	   gst_pad_get_name (srcpad),
	   gst_object_get_name (GST_OBJECT (sinkelement)), padname);

  /* add the element to the pipeline and set correct state */
  if (sinkelement != audiosink) {
    gst_bin_add (GST_BIN (pipeline), sinkelement);
    gst_element_set_state (sinkelement, GST_STATE_READY);
  }
  pad = gst_element_get_pad (sinkelement, padname);
  gst_pad_link (srcpad, pad);
  if (sinkelement != audiosink) {
    gst_element_set_state (sinkelement, GST_STATE_PAUSED);
  }
  gst_object_unref (GST_OBJECT (pad));

  /* if we have static source pads, link those. If we have dynamic
   * source pads, listen for pad-added signals on the element */
  for ( ; templlist != NULL; templlist = templlist->next) {
    GstStaticPadTemplate *templ = templlist->data;

    /* only sourcepads, no request pads */
    if (templ->direction != GST_PAD_SRC ||
        templ->presence == GST_PAD_REQUEST) {
      continue;
    }

    switch (templ->presence) {
      case GST_PAD_ALWAYS: {
        GstPad *pad = gst_element_get_pad (sinkelement, templ->name_template);
        GstCaps *caps = gst_pad_get_caps (pad);

        /* link */
        try_to_plug (pad, caps);
        gst_object_unref (GST_OBJECT (pad));
        gst_caps_unref (caps);
        break;
      }
      case GST_PAD_SOMETIMES:
        has_dynamic_pads = TRUE;
        break;
      default:
        break;
    }
  }

  /* listen for newly created pads if this element supports that */
  if (has_dynamic_pads) {
    g_signal_connect (sinkelement, "pad-added", G_CALLBACK (cb_newpad), NULL);
  }
}

static void
try_to_plug (GstPad        *pad,
	     const GstCaps *caps)
{
  GstObject *parent = GST_OBJECT (GST_OBJECT_PARENT (pad));
  const gchar *mime;
  const GList *item;
  GstCaps *res, *audiocaps;

  /* don't plug if we're already plugged - FIXME: memleak for pad */
  if (GST_PAD_IS_LINKED (gst_element_get_pad (audiosink, "sink"))) {
    g_print ("Omitting link for pad %s:%s because we're already linked\n",
	     GST_OBJECT_NAME (parent), GST_OBJECT_NAME (pad));
    return;
  }

  /* as said above, we only try to plug audio... Omit video */
  mime = gst_structure_get_name (gst_caps_get_structure (caps, 0));
  if (g_strrstr (mime, "video")) {
    g_print ("Omitting link for pad %s:%s because mimetype %s is non-audio\n",
	     GST_OBJECT_NAME (parent), GST_OBJECT_NAME (pad), mime);
    return;
  }

  /* can it link to the audiopad? */
  audiocaps = gst_pad_get_caps (gst_element_get_pad (audiosink, "sink"));
  res = gst_caps_intersect (caps, audiocaps);
  if (res && !gst_caps_is_empty (res)) {
    g_print ("Found pad to link to audiosink - plugging is now done\n");
    close_link (pad, audiosink, "sink", NULL);
    gst_caps_unref (audiocaps);
    gst_caps_unref (res);
    return;
  }
  gst_caps_unref (audiocaps);
  gst_caps_unref (res);

  /* try to plug from our list */
  for (item = factories; item != NULL; item = item->next) {
    GstElementFactory *factory = GST_ELEMENT_FACTORY (item->data);
    const GList *pads;

    for (pads = gst_element_factory_get_static_pad_templates (factory);
         pads != NULL; pads = pads->next) {
      GstStaticPadTemplate *templ = pads->data;

      /* find the sink template - need an always pad*/
      if (templ->direction != GST_PAD_SINK ||
          templ->presence != GST_PAD_ALWAYS) {
        continue;
      }

      /* can it link? */
      res = gst_caps_intersect (caps,
          gst_static_caps_get (&templ->static_caps));
      if (res && !gst_caps_is_empty (res)) {
        GstElement *element;
        gchar *name_template = g_strdup (templ->name_template);

        /* close link and return */
        gst_caps_unref (res);
        element = gst_element_factory_create (factory, NULL);
        close_link (pad, element, name_template,
		    gst_element_factory_get_static_pad_templates (factory));
        g_free (name_template);
        return;
      }
      gst_caps_unref (res);

      /* we only check one sink template per factory, so move on to the
       * next factory now */
      break;
    }
  }

  /* if we get here, no item was found */
  g_print ("No compatible pad found to decode %s on %s:%s\n",
	   mime, GST_OBJECT_NAME (parent), GST_OBJECT_NAME (pad));
}

static void
cb_typefound (GstElement *typefind,
	      guint       probability,
	      GstCaps    *caps,
	      gpointer    data)
{
  gchar *s;
  GstPad *pad;

  s = gst_caps_to_string (caps);
  g_print ("Detected media type %s\n", s);
  g_free (s);

  /* actually plug now */
  pad = gst_element_get_pad (typefind, "src");
  try_to_plug (pad, caps);
  gst_object_unref (GST_OBJECT (pad));
}

/*** block d  from ../../../docs/manual/advanced-autoplugging.xml ***/
static gboolean
my_bus_callback (GstBus     *bus,
                 GstMessage *message,
                 gpointer    data)
{
  GMainLoop *loop = data;
  std_log(LOG_FILENAME_LINE, "Callback ENTER");
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      std_log(LOG_FILENAME_LINE, "ERROR:");
      g_print ("Error: %s\n", err->message);
      printf("ERROR: %s\n",err->message);
      g_error_free (err);
      g_free (debug);
      assert_failed = 1;
      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      g_main_loop_quit (loop);
      std_log(LOG_FILENAME_LINE, "END-of-stream");
      break;
    default:
      break;
  }

  /* remove from queue */
  std_log(LOG_FILENAME_LINE, "Callback EXIT");
  return TRUE;
}

gint
main (gint   argc,
      gchar *argv[])
{
  GMainLoop *loop;
  GstElement *typefind, *realsink;
  GstElement *filesrc, *typefind1;
  GstBus *bus;
  GError *err = NULL;
  gchar *p;
  
  xmlfile = "manual_dynamic";
  std_log(LOG_FILENAME_LINE, "Test Started manual_dynamic");

  /* init GStreamer and ourselves */
  gst_init (&argc, &argv);
  loop = g_main_loop_new (NULL, FALSE);
  init_factories ();

  /* args */
  if (argc != 2) {
    g_print ("Usage: %s <filename>\n", argv[0]);
    std_log(LOG_FILENAME_LINE, "Test Failed argument need to be passed ");
    create_xml(1); 
    exit (-1);
  }

  /* pipeline */
  //changes for parse launch
  /*
  p = g_strdup_printf ("filesrc location=\"%s\" ! typefind name=tf", argv[1]);  
  std_log(LOG_FILENAME_LINE, "parse launch Start");
  pipeline = gst_parse_launch (p, &err);
  std_log(LOG_FILENAME_LINE, "parse launch Done");
  g_free (p);

  if (err) {
    std_log(LOG_FILENAME_LINE, "Could not construct pipeline");
    create_xml(1);
    g_error ("Could not construct pipeline: %s", err->message);
    g_error_free (err);
    return -1;
  }
  */
  
  filesrc = gst_element_factory_make ("filesrc", "src");
  g_assert (filesrc);  
  g_object_set (G_OBJECT (filesrc), "location", argv[1], NULL);
  
  typefind1 = gst_element_factory_make ("typefind", "tf");
  g_assert (typefind1);  
  
  pipeline = gst_pipeline_new ("pipeline");
  g_assert (pipeline);
  
  gst_bin_add_many (GST_BIN (pipeline), filesrc, typefind1, NULL);

    /* link the elements */
  gst_element_link_many (filesrc, typefind1, NULL);
  
  
//////////////////////////////////////////  
  
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, my_bus_callback, loop);
  gst_object_unref (bus);
  
  if(!bus)
      {
      std_log(LOG_FILENAME_LINE, "failed to create bus");
      create_xml(1);
      return -1;
      }

  typefind = gst_bin_get_by_name (GST_BIN (pipeline), "tf");
  if(!typefind)
      {
      std_log(LOG_FILENAME_LINE, "Failed to create typefind");
      create_xml(1);
      return -1;
      }
  g_signal_connect (typefind, "have-type", G_CALLBACK (cb_typefound), NULL);
  gst_object_unref (GST_OBJECT (typefind));
  
  std_log(LOG_FILENAME_LINE, "Element Create Start"); 
  audiosink = gst_element_factory_make ("audioconvert", "aconv");
  realsink = gst_element_factory_make ("filesink", "audiosink");
  std_log(LOG_FILENAME_LINE, "Element Create Done");
  if(!audiosink || !realsink)
      {
      std_log(LOG_FILENAME_LINE, "Failed to create audiosink or realsink");
      create_xml(1);
      return -1;
      }
  std_log(LOG_FILENAME_LINE, "Create out.txt Start");
  g_object_set(realsink,"location","c:\\data\\out.txt",NULL);
  std_log(LOG_FILENAME_LINE, "Create out.txt Done");
  gst_bin_add_many (GST_BIN (pipeline), audiosink, realsink, NULL);
  std_log(LOG_FILENAME_LINE, "Element Link Start");
  gst_element_link (audiosink, realsink);
  std_log(LOG_FILENAME_LINE, "Element Link Done");
  std_log(LOG_FILENAME_LINE, "Set PLAY State");
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  std_log(LOG_FILENAME_LINE, "Set Play State Done");

  /* run */
  std_log(LOG_FILENAME_LINE, "main loop run Start");
  g_main_loop_run (loop);
  std_log(LOG_FILENAME_LINE, "main loop run Done");

  /* exit */
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (pipeline));
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  return 0;
}
