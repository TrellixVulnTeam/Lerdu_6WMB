/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *
 * gst.c: Initialization and non-pipeline operations
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

/**
 * SECTION:gst
 * @short_description: Media library supporting arbitrary formats and filter
 *                     graphs.
 * @see_also: Check out both <ulink url="http://www.cse.ogi.edu/sysl/">OGI's
 *            pipeline</ulink> and Microsoft's DirectShow for some background.
 *
 * GStreamer is a framework for constructing graphs of various filters
 * (termed elements here) that will handle streaming media.  Any discreet
 * (packetizable) media type is supported, with provisions for automatically
 * determining source type.  Formatting/framing information is provided with
 * a powerful negotiation framework.  Plugins are heavily used to provide for
 * all elements, allowing one to construct plugins outside of the GST
 * library, even released binary-only if license require (please don't).
 *
 * GStreamer borrows heavily from both the <ulink
 * url="http://www.cse.ogi.edu/sysl/">OGI media pipeline</ulink> and
 * Microsoft's DirectShow, hopefully taking the best of both and leaving the
 * cruft behind. Its interface is slowly getting stable.
 *
 * The <application>GStreamer</application> library should be initialized with
 * gst_init() before it can be used. You should pass pointers to the main argc
 * and argv variables so that GStreamer can process its own command line
 * options, as shown in the following example.
 *
 * <example>
 * <title>Initializing the gstreamer library</title>
 * <programlisting language="c">
 * int
 * main (int argc, char *argv[])
 * {
 *   // initialize the GStreamer library
 *   gst_init (&amp;argc, &amp;argv);
 *   ...
 * }
 * </programlisting>
 * </example>
 *
 * It's allowed to pass two NULL pointers to gst_init() in case you don't want
 * to pass the command line args to GStreamer.
 *
 * You can also use GOption to initialize your own parameters as shown in
 * the next code fragment:
 * <example>
 * <title>Initializing own parameters when initializing gstreamer</title>
 * <programlisting>
 * static gboolean stats = FALSE;
 * ...
 * int
 * main (int argc, char *argv[])
 * {
 *  GOptionEntry options[] = {
 *   {"tags", 't', 0, G_OPTION_ARG_NONE, &amp;tags,
 *       N_("Output tags (also known as metadata)"), NULL},
 *   {NULL}
 *  };
 *  // must initialise the threading system before using any other GLib funtion
 *  if (!g_thread_supported ())
 *    g_thread_init (NULL);
 *  ctx = g_option_context_new ("[ADDITIONAL ARGUMENTS]");
 *  g_option_context_add_main_entries (ctx, options, GETTEXT_PACKAGE);
 *  g_option_context_add_group (ctx, gst_init_get_option_group ());
 *  if (!g_option_context_parse (ctx, &amp;argc, &amp;argv, &amp;err)) {
 *    g_print ("Error initializing: &percnt;s\n", GST_STR_NULL (err->message));
 *    exit (1);
 *  }
 *  g_option_context_free (ctx);
 * ...
 * }
 * </programlisting>
 * </example>
 *
 * Use gst_version() to query the library version at runtime or use the
 * GST_VERSION_* macros to find the version at compile time. Optionally
 * gst_version_string() returns a printable string.
 *
 * The gst_deinit() call is used to clean up all internal resources used
 * by <application>GStreamer</application>. It is mostly used in unit tests 
 * to check for leaks.
 *
 * Last reviewed on 2006-08-11 (0.10.10)
 */

#include "gst_private.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#ifdef HAVE_FORK
#include <sys/wait.h>
#endif /* HAVE_FORK */
#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "gst-i18n-lib.h"
#include <locale.h>             /* for LC_ALL */

#include "gst.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include <config.h>
#include <string.h>
#endif

#define GST_CAT_DEFAULT GST_CAT_GST_INIT

#define MAX_PATH_SPLIT  16
#define GST_PLUGIN_SEPARATOR ","

static gboolean gst_initialized = FALSE;
static gboolean gst_deinitialized = FALSE;

#ifndef GST_DISABLE_REGISTRY
static GList *plugin_paths = NULL;      /* for delayed processing in post_init */
#endif

#ifndef GST_DISABLE_GST_DEBUG
extern const gchar *priv_gst_dump_dot_dir;
#endif

/* defaults */
#ifdef HAVE_FORK
#define DEFAULT_FORK TRUE
#else
#define DEFAULT_FORK FALSE
#endif /* HAVE_FORK */

/* set to TRUE when segfaults need to be left as is */
static gboolean _gst_disable_segtrap = FALSE;

/* control the behaviour of registry rebuild */
static gboolean _gst_enable_registry_fork = DEFAULT_FORK;

#ifdef __SYMBIAN32__
/*set to TRUE when registry needn't to be updated */
static gboolean _gst_disable_registry_update = FALSE;
#endif

static void load_plugin_func (gpointer data, gpointer user_data);
static gboolean init_pre (GOptionContext * context, GOptionGroup * group,
    gpointer data, GError ** error);
static gboolean init_post (GOptionContext * context, GOptionGroup * group,
    gpointer data, GError ** error);
#ifndef GST_DISABLE_OPTION_PARSING
static gboolean parse_goption_arg (const gchar * s_opt,
    const gchar * arg, gpointer data, GError ** err);
#endif

static GSList *preload_plugins = NULL;

const gchar g_log_domain_gstreamer[] = "GStreamer";

static void
debug_log_handler (const gchar * log_domain,
    GLogLevelFlags log_level, const gchar * message, gpointer user_data)
{
  g_log_default_handler (log_domain, log_level, message, user_data);
  /* FIXME: do we still need this ? fatal errors these days are all
   * other than core errors */
  /* g_on_error_query (NULL); */
}

enum
{
  ARG_VERSION = 1,
  ARG_FATAL_WARNINGS,
#ifndef GST_DISABLE_GST_DEBUG
  ARG_DEBUG_LEVEL,
  ARG_DEBUG,
  ARG_DEBUG_DISABLE,
  ARG_DEBUG_NO_COLOR,
  ARG_DEBUG_HELP,
#endif
  ARG_PLUGIN_SPEW,
  ARG_PLUGIN_PATH,
  ARG_PLUGIN_LOAD,
  ARG_SEGTRAP_DISABLE,
#ifdef __SYMBIAN32__
  ARG_REGISTRY_UPDATE_DISABLE,
#endif
  ARG_REGISTRY_FORK_DISABLE
};

/* debug-spec ::= category-spec [, category-spec]*
 * category-spec ::= category:val | val
 * category ::= [^:]+
 * val ::= [0-5]
 */

#ifndef NUL
#define NUL '\0'
#endif

#ifndef GST_DISABLE_GST_DEBUG
static gboolean
parse_debug_category (gchar * str, const gchar ** category)
{
  if (!str)
    return FALSE;

  /* works in place */
  g_strstrip (str);

  if (str[0] != NUL) {
    *category = str;
    return TRUE;
  }

  return FALSE;
}

static gboolean
parse_debug_level (gchar * str, gint * level)
{
  if (!str)
    return FALSE;

  /* works in place */
  g_strstrip (str);

  if (str[0] != NUL && str[1] == NUL
      && str[0] >= '0' && str[0] < '0' + GST_LEVEL_COUNT) {
    *level = str[0] - '0';
    return TRUE;
  }

  return FALSE;
}

static void
parse_debug_list (const gchar * list)
{
  gchar **split;
  gchar **walk;

  g_return_if_fail (list != NULL);

  split = g_strsplit (list, ",", 0);

  for (walk = split; *walk; walk++) {
    if (strchr (*walk, ':')) {
      gchar **values = g_strsplit (*walk, ":", 2);

      if (values[0] && values[1]) {
        gint level;
        const gchar *category;

        if (parse_debug_category (values[0], &category)
            && parse_debug_level (values[1], &level))
          gst_debug_set_threshold_for_name (category, level);
      }

      g_strfreev (values);
    } else {
      gint level;

      if (parse_debug_level (*walk, &level))
        gst_debug_set_default_threshold (level);
    }
  }

  g_strfreev (split);
}
#endif

/**
 * gst_init_get_option_group:
 *
 * Returns a #GOptionGroup with GStreamer's argument specifications. The
 * group is set up to use standard GOption callbacks, so when using this
 * group in combination with GOption parsing methods, all argument parsing
 * and initialization is automated.
 *
 * This function is useful if you want to integrate GStreamer with other
 * libraries that use GOption (see g_option_context_add_group() ).
 *
 * If you use this function, you should make sure you initialise the GLib
 * threading system as one of the very first things in your program
 * (see the example at the beginning of this section).
 *
 * Returns: a pointer to GStreamer's option group.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


GOptionGroup *
gst_init_get_option_group (void)
{
#ifndef GST_DISABLE_OPTION_PARSING
  GOptionGroup *group;
  const static GOptionEntry gst_args[] = {
    {"gst-version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
        (gpointer) parse_goption_arg, N_("Print the GStreamer version"), NULL},
    {"gst-fatal-warnings", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
        (gpointer) parse_goption_arg, N_("Make all warnings fatal"), NULL},
#ifndef GST_DISABLE_GST_DEBUG
    {"gst-debug-help", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
          (gpointer) parse_goption_arg,
          N_("Print available debug categories and exit"),
        NULL},
    {"gst-debug-level", 0, 0, G_OPTION_ARG_CALLBACK,
          (gpointer) parse_goption_arg,
          N_("Default debug level from 1 (only error) to 5 (anything) or "
              "0 for no output"),
        N_("LEVEL")},
    {"gst-debug", 0, 0, G_OPTION_ARG_CALLBACK, (gpointer) parse_goption_arg,
          N_("Comma-separated list of category_name:level pairs to set "
              "specific levels for the individual categories. Example: "
              "GST_AUTOPLUG:5,GST_ELEMENT_*:3"),
        N_("LIST")},
    {"gst-debug-no-color", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
          (gpointer) parse_goption_arg, N_("Disable colored debugging output"),
        NULL},
    {"gst-debug-disable", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
        (gpointer) parse_goption_arg, N_("Disable debugging"), NULL},
#endif
    {"gst-plugin-spew", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
          (gpointer) parse_goption_arg,
          N_("Enable verbose plugin loading diagnostics"),
        NULL},
    {"gst-plugin-path", 0, 0, G_OPTION_ARG_CALLBACK,
          (gpointer) parse_goption_arg,
        N_("Colon-separated paths containing plugins"), N_("PATHS")},
    {"gst-plugin-load", 0, 0, G_OPTION_ARG_CALLBACK,
          (gpointer) parse_goption_arg,
          N_("Comma-separated list of plugins to preload in addition to the "
              "list stored in environment variable GST_PLUGIN_PATH"),
        N_("PLUGINS")},
    {"gst-disable-segtrap", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
          (gpointer) parse_goption_arg,
          N_("Disable trapping of segmentation faults during plugin loading"),
        NULL},
#ifdef __SYMBIAN32__		
    {"gst-disable-registry-update", 0, G_OPTION_FLAG_NO_ARG,
          G_OPTION_ARG_CALLBACK,
          (gpointer) parse_goption_arg,
          N_("Disable updating the registry"),
        NULL},
#endif		
    {"gst-disable-registry-fork", 0, G_OPTION_FLAG_NO_ARG,
          G_OPTION_ARG_CALLBACK,
          (gpointer) parse_goption_arg,
          N_("Disable the use of fork() while scanning the registry"),
        NULL},
    {NULL}
  };

  /* The GLib threading system must be initialised before calling any other
   * GLib function according to the documentation; if the application hasn't
   * called gst_init() yet or initialised the threading system otherwise, we
   * better issue a warning here (since chances are high that the application
   * has already called other GLib functions such as g_option_context_new() */
  if (!g_thread_supported ()) {
    g_warning ("The GStreamer function gst_init_get_option_group() was\n"
        "\tcalled, but the GLib threading system has not been initialised\n"
        "\tyet, something that must happen before any other GLib function\n"
        "\tis called. The application needs to be fixed so that it calls\n"
        "\t   if (!g_thread_supported ()) g_thread_init(NULL);\n"
        "\tas very first thing in its main() function. Please file a bug\n"
        "\tagainst this application.");
    g_thread_init (NULL);
  }

  group = g_option_group_new ("gst", _("GStreamer Options"),
      _("Show GStreamer Options"), NULL, NULL);
  g_option_group_set_parse_hooks (group, (GOptionParseFunc) init_pre,
      (GOptionParseFunc) init_post);

  g_option_group_add_entries (group, gst_args);
  g_option_group_set_translation_domain (group, GETTEXT_PACKAGE);

  return group;
#else
  return NULL;
#endif
}

/**
 * gst_init_check:
 * @argc: pointer to application's argc
 * @argv: pointer to application's argv
 * @err: pointer to a #GError to which a message will be posted on error
 *
 * Initializes the GStreamer library, setting up internal path lists,
 * registering built-in elements, and loading standard plugins.
 *
 * This function will return %FALSE if GStreamer could not be initialized
 * for some reason.  If you want your program to fail fatally,
 * use gst_init() instead.
 *
 * This function should be called before calling any other GLib functions. If
 * this is not an option, your program must initialise the GLib thread system
 * using g_thread_init() before any other GLib functions are called.
 *
 * Returns: %TRUE if GStreamer could be initialized.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_init_check (int *argc, char **argv[], GError ** err)
{
#ifndef GST_DISABLE_OPTION_PARSING
  GOptionGroup *group;
  GOptionContext *ctx;
#endif
  gboolean res;

#ifdef __SYMBIAN32__
	char* checkexe,*temp;
	/// registry will be updated only if gst_init is called from GSTRegistryGenerator.exe
	if( argv )
	{
		checkexe = *argv[0];
		temp = checkexe+strlen(checkexe);
		while(temp!=checkexe && *temp!='\\')
		    {
		    temp--;
		    }

		checkexe = ++temp;
		
		if( strcmp(checkexe,GST_REGISTRY_GENERATOR) == 0 )
		{
			/// setting flag for update registry if called from GSTRegistryGenerator.exe
			setenv("GST_REGISTRY_UPDATE", "yes", 1);
		}
		else
		{
			/// setting flag for disable update registry
			setenv("GST_REGISTRY_UPDATE", "no", 0);
		}
	}
	else
	{
		/// setting flag for disable update registry
	  setenv("GST_REGISTRY_UPDATE", "no", 0);
	}
#endif

  if (!g_thread_supported ())
    g_thread_init (NULL);

  if (gst_initialized) {
    GST_DEBUG ("already initialized gst");
    return TRUE;
  }
#ifndef GST_DISABLE_OPTION_PARSING
  ctx = g_option_context_new ("- GStreamer initialization");
  g_option_context_set_ignore_unknown_options (ctx, TRUE);
  group = gst_init_get_option_group ();
  g_option_context_add_group (ctx, group);
  res = g_option_context_parse (ctx, argc, argv, err);
  g_option_context_free (ctx);
#else
  init_pre (NULL, NULL, NULL, NULL);
  init_post (NULL, NULL, NULL, NULL);
  res = TRUE;
#endif

  gst_initialized = res;

  if (res) {
    GST_INFO ("initialized GStreamer successfully");
  } else {
    GST_INFO ("failed to initialize GStreamer");
  }

  return res;
}

/**
 * gst_init:
 * @argc: pointer to application's argc
 * @argv: pointer to application's argv
 *
 * Initializes the GStreamer library, setting up internal path lists,
 * registering built-in elements, and loading standard plugins.
 *
 * This function should be called before calling any other GLib functions. If
 * this is not an option, your program must initialise the GLib thread system
 * using g_thread_init() before any other GLib functions are called.
 *
 * <note><para>
 * This function will terminate your program if it was unable to initialize
 * GStreamer for some reason.  If you want your program to fall back,
 * use gst_init_check() instead.
 * </para></note>
 *
 * WARNING: This function does not work in the same way as corresponding
 * functions in other glib-style libraries, such as gtk_init().  In
 * particular, unknown command line options cause this function to
 * abort program execution.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_init (int *argc, char **argv[])
{
  GError *err = NULL;

  if (!gst_init_check (argc, argv, &err)) {
    g_print ("Could not initialize GStreamer: %s\n",
        err ? err->message : "unknown error occurred");
    if (err) {
      g_error_free (err);
    }
    exit (1);
  }
}

#ifndef GST_DISABLE_REGISTRY
static void
add_path_func (gpointer data, gpointer user_data)
{
  GST_INFO ("Adding plugin path: \"%s\", will scan later", (gchar *) data);
  plugin_paths = g_list_append (plugin_paths, g_strdup (data));
}
#endif

#ifndef GST_DISABLE_OPTION_PARSING
static void
prepare_for_load_plugin_func (gpointer data, gpointer user_data)
{
  preload_plugins = g_slist_prepend (preload_plugins, g_strdup (data));
}
#endif

static void
load_plugin_func (gpointer data, gpointer user_data)
{
  GstPlugin *plugin;
  const gchar *filename;
  GError *err = NULL;

  filename = (const gchar *) data;

  plugin = gst_plugin_load_file (filename, &err);

  if (plugin) {
    GST_INFO ("Loaded plugin: \"%s\"", filename);

    gst_default_registry_add_plugin (plugin);
  } else {
    if (err) {
      /* Report error to user, and free error */
      GST_ERROR ("Failed to load plugin: %s", err->message);
      g_error_free (err);
    } else {
      GST_WARNING ("Failed to load plugin: \"%s\"", filename);
    }
  }
}

#ifndef GST_DISABLE_OPTION_PARSING
static void
split_and_iterate (const gchar * stringlist, gchar * separator, GFunc iterator,
    gpointer user_data)
{
  gchar **strings;
  gint j = 0;
  gchar *lastlist = g_strdup (stringlist);

  while (lastlist) {
    strings = g_strsplit (lastlist, separator, MAX_PATH_SPLIT);
    g_free (lastlist);
    lastlist = NULL;

    while (strings[j]) {
      iterator (strings[j], user_data);
      if (++j == MAX_PATH_SPLIT) {
        lastlist = g_strdup (strings[j]);
        j = 0;
        break;
      }
    }
    g_strfreev (strings);
  }
}
#endif

/* we have no fail cases yet, but maybe in the future */
static gboolean
init_pre (GOptionContext * context, GOptionGroup * group, gpointer data,
    GError ** error)
{
  if (gst_initialized) {
    GST_DEBUG ("already initialized");
    return TRUE;
  }

  /* GStreamer was built against a GLib >= 2.8 and is therefore not doing
   * the refcount hack. Check that it isn't being run against an older GLib */
  if (glib_major_version < 2 ||
      (glib_major_version == 2 && glib_minor_version < 8)) {
    g_warning ("GStreamer was compiled against GLib %d.%d.%d but is running"
        " against %d.%d.%d. This will cause reference counting issues",
        GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION, GLIB_MICRO_VERSION,
        glib_major_version, glib_minor_version, glib_micro_version);
  }

  g_type_init ();

  /* we need threading to be enabled right here */
  g_assert (g_thread_supported ());
  _gst_debug_init ();

#ifdef ENABLE_NLS
  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif /* ENABLE_NLS */

#ifndef GST_DISABLE_GST_DEBUG
  {
    const gchar *debug_list;

    if (g_getenv ("GST_DEBUG_NO_COLOR") != NULL)
      gst_debug_set_colored (FALSE);

    debug_list = g_getenv ("GST_DEBUG");
    if (debug_list) {
      parse_debug_list (debug_list);
    }
  }

  priv_gst_dump_dot_dir = g_getenv ("GST_DEBUG_DUMP_DOT_DIR");
#endif
  /* This is the earliest we can make stuff show up in the logs.
   * So give some useful info about GStreamer here */
  GST_INFO ("Initializing GStreamer Core Library version %s", VERSION);
  GST_INFO ("Using library installed in %s", LIBDIR);

  /* Print some basic system details if possible (OS/architecture) */
#ifdef HAVE_SYS_UTSNAME_H
  {
    struct utsname sys_details;

    if (uname (&sys_details) == 0) {
      GST_INFO ("%s %s %s %s %s", sys_details.sysname,
          sys_details.nodename, sys_details.release, sys_details.version,
          sys_details.machine);
    }
  }
#endif

  return TRUE;
}

static gboolean
gst_register_core_elements (GstPlugin * plugin)
{
  /* register some standard builtin types */
  if (!gst_element_register (plugin, "bin", GST_RANK_PRIMARY,
          GST_TYPE_BIN) ||
      !gst_element_register (plugin, "pipeline", GST_RANK_PRIMARY,
          GST_TYPE_PIPELINE)
      )
    g_assert_not_reached ();

  return TRUE;
}

#ifndef GST_DISABLE_REGISTRY

typedef enum
{
  REGISTRY_SCAN_AND_UPDATE_FAILURE = 0,
  REGISTRY_SCAN_AND_UPDATE_SUCCESS_NOT_CHANGED,
  REGISTRY_SCAN_AND_UPDATE_SUCCESS_UPDATED
} GstRegistryScanAndUpdateResult;

/*
 * scan_and_update_registry:
 * @default_registry: the #GstRegistry
 * @registry_file: registry filename
 * @write_changes: write registry if it has changed?
 *
 * Scans for registry changes and eventually updates the registry cache. 
 *
 * Return: %REGISTRY_SCAN_AND_UPDATE_FAILURE if the registry could not scanned
 *         or updated, %REGISTRY_SCAN_AND_UPDATE_SUCCESS_NOT_CHANGED if the
 *         registry is clean and %REGISTRY_SCAN_AND_UPDATE_SUCCESS_UPDATED if
 *         it has been updated and the cache needs to be re-read.
 */
static GstRegistryScanAndUpdateResult
scan_and_update_registry (GstRegistry * default_registry,
    const gchar * registry_file, gboolean write_changes, GError ** error)
{
  const gchar *plugin_path;
  gboolean changed = FALSE;
  GList *l;

  /* scan paths specified via --gst-plugin-path */
  GST_DEBUG ("scanning paths added via --gst-plugin-path");
  for (l = plugin_paths; l != NULL; l = l->next) {
    GST_INFO ("Scanning plugin path: \"%s\"", (gchar *) l->data);
    /* CHECKME: add changed |= here as well? */
    gst_registry_scan_path (default_registry, (gchar *) l->data);
  }
  /* keep plugin_paths around in case a re-scan is forced later on */

  /* GST_PLUGIN_PATH specifies a list of directories to scan for
   * additional plugins.  These take precedence over the system plugins */
  plugin_path = g_getenv ("GST_PLUGIN_PATH");
  if (plugin_path) {
    char **list;
    int i;

    GST_DEBUG ("GST_PLUGIN_PATH set to %s", plugin_path);
    list = g_strsplit (plugin_path, G_SEARCHPATH_SEPARATOR_S, 0);
    for (i = 0; list[i]; i++) {
      changed |= gst_registry_scan_path (default_registry, list[i]);
    }
    g_strfreev (list);
  } else {
    GST_DEBUG ("GST_PLUGIN_PATH not set");
  }

  /* GST_PLUGIN_SYSTEM_PATH specifies a list of plugins that are always
   * loaded by default.  If not set, this defaults to the system-installed
   * path, and the plugins installed in the user's home directory */
  plugin_path = g_getenv ("GST_PLUGIN_SYSTEM_PATH");
  if (plugin_path == NULL) {
    char *home_plugins;

    GST_DEBUG ("GST_PLUGIN_SYSTEM_PATH not set");

    /* plugins in the user's home directory take precedence over
     * system-installed ones */
    home_plugins = g_build_filename (g_get_home_dir (),
        ".gstreamer-" GST_MAJORMINOR, "plugins", NULL);
    GST_DEBUG ("scanning home plugins %s", home_plugins);
    changed |= gst_registry_scan_path (default_registry, home_plugins);
    g_free (home_plugins);

    /* add the main (installed) library path */
    GST_DEBUG ("scanning main plugins %s", PLUGINDIR);
    changed |= gst_registry_scan_path (default_registry, PLUGINDIR);
  } else {
    gchar **list;
    gint i;

    GST_DEBUG ("GST_PLUGIN_SYSTEM_PATH set to %s", plugin_path);
    list = g_strsplit (plugin_path, G_SEARCHPATH_SEPARATOR_S, 0);
    for (i = 0; list[i]; i++) {
      changed |= gst_registry_scan_path (default_registry, list[i]);
    }
    g_strfreev (list);
  }

  /* Remove cached plugins so stale info is cleared. */
  changed |= _priv_gst_registry_remove_cache_plugins (default_registry);

  if (!changed) {
    GST_INFO ("Registry cache has not changed");
    return REGISTRY_SCAN_AND_UPDATE_SUCCESS_NOT_CHANGED;
  }

  if (!write_changes) {
    GST_INFO ("Registry cached changed, but writing is disabled. Not writing.");
    return REGISTRY_SCAN_AND_UPDATE_FAILURE;
  }

  GST_INFO ("Registry cache changed. Writing new registry cache");
#ifdef USE_BINARY_REGISTRY
  if (!gst_registry_binary_write_cache (default_registry, registry_file)) {
#else
  if (!gst_registry_xml_write_cache (default_registry, registry_file)) {
#endif
    g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_FAILED,
        _("Error writing registry cache to %s: %s"),
        registry_file, g_strerror (errno));
    return REGISTRY_SCAN_AND_UPDATE_FAILURE;
  }

  GST_INFO ("Registry cache written successfully");
  return REGISTRY_SCAN_AND_UPDATE_SUCCESS_UPDATED;
}

static gboolean
ensure_current_registry_nonforking (GstRegistry * default_registry,
    const gchar * registry_file, GError ** error)
{
  /* fork() not available */
  GST_INFO ("reading registry cache: %s", registry_file);
#ifdef USE_BINARY_REGISTRY
  gst_registry_binary_read_cache (default_registry, registry_file);
#else
  gst_registry_xml_read_cache (default_registry, registry_file);
#endif
  GST_DEBUG ("Updating registry cache in-process");
  scan_and_update_registry (default_registry, registry_file, TRUE, error);
  return TRUE;
}

/* when forking is not available this function always does nothing but return
 * TRUE immediatly */
static gboolean
ensure_current_registry_forking (GstRegistry * default_registry,
    const gchar * registry_file, GError ** error)
{
#ifdef HAVE_FORK
  pid_t pid;
  int pfd[2];
  int ret;

  /* We fork here, and let the child read and possibly rebuild the registry.
   * After that, the parent will re-read the freshly generated registry. */
  GST_DEBUG ("forking to update registry");

  if (pipe (pfd) == -1) {
    g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_FAILED,
        _("Error re-scanning registry %s: %s"),
        ", could not create pipes. Error", g_strerror (errno));
    return FALSE;
  }

  GST_INFO ("reading registry cache: %s", registry_file);
#ifdef USE_BINARY_REGISTRY
  gst_registry_binary_read_cache (default_registry, registry_file);
#else
  gst_registry_xml_read_cache (default_registry, registry_file);
#endif

  pid = fork ();
  if (pid == -1) {
    GST_ERROR ("Failed to fork()");
    g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_FAILED,
        _("Error re-scanning registry %s: %s"),
        ", failed to fork. Error", g_strerror (errno));
    return FALSE;
  }

  if (pid == 0) {
    gint result_code;

    /* this is the child. Close the read pipe */
    (void) close (pfd[0]);

    GST_DEBUG ("child reading registry cache");
    result_code =
        scan_and_update_registry (default_registry, registry_file, TRUE, NULL);

    /* need to use _exit, so that any exit handlers registered don't
     * bring down the main program */
    GST_DEBUG ("child exiting: %d", result_code);

    /* make valgrind happy (yes, you can call it insane) */
    g_free ((char *) registry_file);

    /* write a result byte to the pipe */
    do {
      ret = write (pfd[1], &result_code, sizeof (result_code));
    } while (ret == -1 && errno == EINTR);
    /* if ret == -1 now, we could not write to pipe, probably 
     * means parent has exited before us */
    (void) close (pfd[1]);

    _exit (0);
  } else {
    gint result_code;

    /* parent. Close write pipe */
    (void) close (pfd[1]);

    /* Wait for result from the pipe */
    GST_DEBUG ("Waiting for data from child");
    do {
      ret = read (pfd[0], &result_code, sizeof (result_code));
    } while (ret == -1 && errno == EINTR);

    if (ret == -1) {
      g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_FAILED,
          _("Error re-scanning registry %s: %s"),
          ", read returned error", g_strerror (errno));
      close (pfd[0]);
      return FALSE;
    }
    (void) close (pfd[0]);

    /* Wait to ensure the child is reaped, but ignore the result */
    GST_DEBUG ("parent waiting on child");
    waitpid (pid, NULL, 0);
    GST_DEBUG ("parent done waiting on child");

    if (ret == 0) {
      GST_ERROR ("child did not exit normally, terminated by signal");
      g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_FAILED,
          _("Error re-scanning registry %s"), ", child terminated by signal");
      return FALSE;
    }

    if (result_code == REGISTRY_SCAN_AND_UPDATE_SUCCESS_UPDATED) {
      GST_DEBUG ("Child succeeded. Parent reading registry cache");
      _priv_gst_registry_remove_cache_plugins (default_registry);
#ifdef USE_BINARY_REGISTRY
      gst_registry_binary_read_cache (default_registry, registry_file);
#else
      gst_registry_xml_read_cache (default_registry, registry_file);
#endif
    } else if (result_code == REGISTRY_SCAN_AND_UPDATE_FAILURE) {
      GST_DEBUG ("Child failed. Parent re-scanning registry, ignoring errors.");
      scan_and_update_registry (default_registry, registry_file, FALSE, NULL);
    }
  }
#endif /* HAVE_FORK */
  return TRUE;
}

static gboolean
ensure_current_registry (GError ** error)
{
  char *registry_file;
  GstRegistry *default_registry;
  gboolean ret = TRUE;
  gboolean do_fork;
#ifdef __SYMBIAN32__  
  gboolean do_update;
  char *temp;
  gboolean have_cache = FALSE;
  
#endif

  default_registry = gst_registry_get_default ();
  registry_file = g_strdup (g_getenv ("GST_REGISTRY"));
  if (registry_file == NULL) {
#ifdef USE_BINARY_REGISTRY
#ifdef __SYMBIAN32__  
    registry_file = g_build_filename (GSTREAMER_REGISTERY_PATH,
        ".gstreamer-" GST_MAJORMINOR, "registry." HOST_CPU ".bin", NULL);
    
    if (!g_file_test (registry_file, G_FILE_TEST_EXISTS))
    {
        registry_file = g_build_filename (GSTREAMER_REGISTERY_PATH_IN_ROM,
            ".gstreamer-" GST_MAJORMINOR, "registry." HOST_CPU ".bin", NULL);
    }

#else
    registry_file = g_build_filename (g_get_home_dir (),
        ".gstreamer-" GST_MAJORMINOR, "registry." HOST_CPU ".bin", NULL);
#endif    
#else
    registry_file = g_build_filename (g_get_home_dir (),
        ".gstreamer-" GST_MAJORMINOR, "registry." HOST_CPU ".xml", NULL);
#endif
  }

#ifdef __SYMBIAN32__
  GST_INFO ("reading registry cache: %s", registry_file);
#ifdef USE_BINARY_REGISTRY
  have_cache = gst_registry_binary_read_cache (default_registry, registry_file);
#else
  have_cache = gst_registry_xml_read_cache (default_registry, registry_file);
#endif

  if (have_cache) {
    do_update = !_gst_disable_registry_update;
    if (do_update) {
      const gchar *update_env;

      if ((update_env = g_getenv ("GST_REGISTRY_UPDATE"))) {
#ifndef __WINSCW__
        /* do update for any value different from "no" */
        do_update = (strcmp (update_env, "yes") == 0);
#endif
      }
    }
  } else {
    do_update = TRUE;
  }

  if (do_update) 
#endif  
  {
#ifdef __SYMBIAN32__
    /// registry can't be created in ROM, so fixed it to GSTREAMER_REGISTERY_PATH
    registry_file = g_build_filename (GSTREAMER_REGISTERY_PATH,
       ".gstreamer-" GST_MAJORMINOR, "registry." HOST_CPU ".bin", NULL);
#endif
  /* first see if forking is enabled */
  do_fork = _gst_enable_registry_fork;
  if (do_fork) {
    const gchar *fork_env;

    /* forking enabled, see if it is disabled with an env var */
    if ((fork_env = g_getenv ("GST_REGISTRY_FORK"))) {
      /* fork enabled for any value different from "no" */
      do_fork = strcmp (fork_env, "no") != 0;
    }
  }

  /* now check registry with or without forking */
  if (do_fork) {
    GST_DEBUG ("forking for registry rebuild");
    ret = ensure_current_registry_forking (default_registry, registry_file,
        error);
  } else {
    GST_DEBUG ("requested not to fork for registry rebuild");
    ret = ensure_current_registry_nonforking (default_registry, registry_file,
        error);
  }
  }

  g_free (registry_file);
  GST_INFO ("registry reading and updating done, result = %d", ret);

  return ret;
}
#endif /* GST_DISABLE_REGISTRY */

/*
 * this bit handles:
 * - initalization of threads if we use them
 * - log handler
 * - initial output
 * - initializes gst_format
 * - registers a bunch of types for gst_objects
 *
 * - we don't have cases yet where this fails, but in the future
 *   we might and then it's nice to be able to return that
 */
static gboolean
init_post (GOptionContext * context, GOptionGroup * group, gpointer data,
    GError ** error)
{
  GLogLevelFlags llf;

#ifndef GST_DISABLE_TRACE
  GstTrace *gst_trace;
#endif /* GST_DISABLE_TRACE */

  if (gst_initialized) {
    GST_DEBUG ("already initialized");
    return TRUE;
  }

  llf = G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_ERROR | G_LOG_FLAG_FATAL;
  g_log_set_handler (g_log_domain_gstreamer, llf, debug_log_handler, NULL);

  _priv_gst_quarks_initialize ();
  _gst_format_initialize ();
  _gst_query_initialize ();
  g_type_class_ref (gst_object_get_type ());
  g_type_class_ref (gst_pad_get_type ());
  g_type_class_ref (gst_element_factory_get_type ());
  g_type_class_ref (gst_element_get_type ());
  g_type_class_ref (gst_type_find_factory_get_type ());
  g_type_class_ref (gst_bin_get_type ());

#ifndef GST_DISABLE_INDEX
  g_type_class_ref (gst_index_factory_get_type ());
#endif /* GST_DISABLE_INDEX */
#ifndef GST_DISABLE_URI
  gst_uri_handler_get_type ();
#endif /* GST_DISABLE_URI */

  gst_structure_get_type ();
  _gst_value_initialize ();
  g_type_class_ref (gst_param_spec_fraction_get_type ());
  gst_caps_get_type ();
  _gst_event_initialize ();
  _gst_buffer_initialize ();
  _gst_message_initialize ();
  _gst_tag_initialize ();

  _gst_plugin_initialize ();

  /* register core plugins */
  gst_plugin_register_static (GST_VERSION_MAJOR, GST_VERSION_MINOR,
      "staticelements", "core elements linked into the GStreamer library",
      gst_register_core_elements, VERSION, GST_LICENSE, PACKAGE,
      GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN);

  /*
   * Any errors happening below this point are non-fatal, we therefore mark
   * gstreamer as being initialized, since it is the case from a plugin point of
   * view.
   *
   * If anything fails, it will be put back to FALSE in gst_init_check().
   * This allows some special plugins that would call gst_init() to not cause a
   * looping effect (i.e. initializing GStreamer twice).
   */
  gst_initialized = TRUE;

#ifndef GST_DISABLE_REGISTRY
  if (!ensure_current_registry (error))
    return FALSE;
#endif /* GST_DISABLE_REGISTRY */

  /* if we need to preload plugins, do so now */
  g_slist_foreach (preload_plugins, load_plugin_func, NULL);
  /* keep preload_plugins around in case a re-scan is forced later on */

#ifndef GST_DISABLE_TRACE
  _gst_trace_on = 0;
  if (_gst_trace_on) {
    gst_trace = gst_trace_new ("gst.trace", 1024);
    gst_trace_set_default (gst_trace);
  }
#endif /* GST_DISABLE_TRACE */

  return TRUE;
}

#ifndef GST_DISABLE_GST_DEBUG
static gboolean
select_all (GstPlugin * plugin, gpointer user_data)
{
  return TRUE;
}

static gint
sort_by_category_name (gconstpointer a, gconstpointer b)
{
  return strcmp (gst_debug_category_get_name ((GstDebugCategory *) a),
      gst_debug_category_get_name ((GstDebugCategory *) b));
}

static void
gst_debug_help (void)
{
  GSList *list, *walk;
  GList *list2, *g;

  /* Need to ensure the registry is loaded to get debug categories */
  if (!init_post (NULL, NULL, NULL, NULL))
    exit (1);

  list2 = gst_registry_plugin_filter (gst_registry_get_default (),
      select_all, FALSE, NULL);

  /* FIXME this is gross.  why don't debug have categories PluginFeatures? */
  for (g = list2; g; g = g_list_next (g)) {
    GstPlugin *plugin = GST_PLUGIN_CAST (g->data);

    gst_plugin_load (plugin);
  }
  g_list_free (list2);

  list = gst_debug_get_all_categories ();
  walk = list = g_slist_sort (list, sort_by_category_name);

  g_print ("\n");
  g_print ("name                  level    description\n");
  g_print ("---------------------+--------+--------------------------------\n");

  while (walk) {
    GstDebugCategory *cat = (GstDebugCategory *) walk->data;

    if (gst_debug_is_colored ()) {
      gchar *color = gst_debug_construct_term_color (cat->color);

      g_print ("%s%-20s\033[00m  %1d %s  %s%s\033[00m\n",
          color,
          gst_debug_category_get_name (cat),
          gst_debug_category_get_threshold (cat),
          gst_debug_level_get_name (gst_debug_category_get_threshold (cat)),
          color, gst_debug_category_get_description (cat));
      g_free (color);
    } else {
      g_print ("%-20s  %1d %s  %s\n", gst_debug_category_get_name (cat),
          gst_debug_category_get_threshold (cat),
          gst_debug_level_get_name (gst_debug_category_get_threshold (cat)),
          gst_debug_category_get_description (cat));
    }
    walk = g_slist_next (walk);
  }
  g_slist_free (list);
  g_print ("\n");
}
#endif

#ifndef GST_DISABLE_OPTION_PARSING
static gboolean
parse_one_option (gint opt, const gchar * arg, GError ** err)
{
  switch (opt) {
    case ARG_VERSION:
      g_print ("GStreamer Core Library version %s\n", PACKAGE_VERSION);
      exit (0);
    case ARG_FATAL_WARNINGS:{
      GLogLevelFlags fatal_mask;

      fatal_mask = g_log_set_always_fatal (G_LOG_FATAL_MASK);
      fatal_mask |= G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL;
      g_log_set_always_fatal (fatal_mask);
      break;
    }
#ifndef GST_DISABLE_GST_DEBUG
    case ARG_DEBUG_LEVEL:{
      gint tmp = 0;

      tmp = strtol (arg, NULL, 0);
      if (tmp >= 0 && tmp < GST_LEVEL_COUNT) {
        gst_debug_set_default_threshold (tmp);
      }
      break;
    }
    case ARG_DEBUG:
      parse_debug_list (arg);
      break;
    case ARG_DEBUG_NO_COLOR:
      gst_debug_set_colored (FALSE);
      break;
    case ARG_DEBUG_DISABLE:
      gst_debug_set_active (FALSE);
      break;
    case ARG_DEBUG_HELP:
      gst_debug_help ();
      exit (0);
#endif
    case ARG_PLUGIN_SPEW:
      break;
    case ARG_PLUGIN_PATH:
#ifndef GST_DISABLE_REGISTRY
      split_and_iterate (arg, G_SEARCHPATH_SEPARATOR_S, add_path_func, NULL);
#endif /* GST_DISABLE_REGISTRY */
      break;
    case ARG_PLUGIN_LOAD:
      split_and_iterate (arg, ",", prepare_for_load_plugin_func, NULL);
      break;
    case ARG_SEGTRAP_DISABLE:
      _gst_disable_segtrap = TRUE;
      break;
#ifdef __SYMBIAN32__	  
    case ARG_REGISTRY_UPDATE_DISABLE:
      _gst_disable_registry_update = TRUE;
      break;
#endif	  
    case ARG_REGISTRY_FORK_DISABLE:
      _gst_enable_registry_fork = FALSE;
      break;
    default:
      g_set_error (err, G_OPTION_ERROR, G_OPTION_ERROR_UNKNOWN_OPTION,
          _("Unknown option"));
      return FALSE;
  }

  return TRUE;
}

static gboolean
parse_goption_arg (const gchar * opt,
    const gchar * arg, gpointer data, GError ** err)
{
  static const struct
  {
    gchar *opt;
    int val;
  } options[] = {
    {
    "--gst-version", ARG_VERSION}, {
    "--gst-fatal-warnings", ARG_FATAL_WARNINGS},
#ifndef GST_DISABLE_GST_DEBUG
    {
    "--gst-debug-level", ARG_DEBUG_LEVEL}, {
    "--gst-debug", ARG_DEBUG}, {
    "--gst-debug-disable", ARG_DEBUG_DISABLE}, {
    "--gst-debug-no-color", ARG_DEBUG_NO_COLOR}, {
    "--gst-debug-help", ARG_DEBUG_HELP},
#endif
    {
    "--gst-plugin-spew", ARG_PLUGIN_SPEW}, {
    "--gst-plugin-path", ARG_PLUGIN_PATH}, {
    "--gst-plugin-load", ARG_PLUGIN_LOAD}, {
    "--gst-disable-segtrap", ARG_SEGTRAP_DISABLE}, {
#ifdef __SYMBIAN32__	
    "--gst-disable-registry-update", ARG_REGISTRY_UPDATE_DISABLE}, {
#endif    
	"--gst-disable-registry-fork", ARG_REGISTRY_FORK_DISABLE}, {
    NULL}
  };
  gint val = 0, n;

  for (n = 0; options[n].opt; n++) {
    if (!strcmp (opt, options[n].opt)) {
      val = options[n].val;
      break;
    }
  }

  return parse_one_option (val, arg, err);
}
#endif

extern GstRegistry *_gst_registry_default;

/**
 * gst_deinit:
 *
 * Clean up any resources created by GStreamer in gst_init().
 *
 * It is normally not needed to call this function in a normal application
 * as the resources will automatically be freed when the program terminates.
 * This function is therefore mostly used by testsuites and other memory
 * profiling tools.
 *
 * After this call GStreamer (including this method) should not be used anymore. 
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_deinit (void)
{
  GstClock *clock;

  GST_INFO ("deinitializing GStreamer");

  if (gst_deinitialized) {
    GST_DEBUG ("already deinitialized");
    return;
  }

  g_slist_foreach (preload_plugins, (GFunc) g_free, NULL);
  g_slist_free (preload_plugins);
  preload_plugins = NULL;

#ifndef GST_DISABLE_REGISTRY
  g_list_foreach (plugin_paths, (GFunc) g_free, NULL);
  g_list_free (plugin_paths);
  plugin_paths = NULL;
#endif

  clock = gst_system_clock_obtain ();
  gst_object_unref (clock);
  gst_object_unref (clock);

  _priv_gst_registry_cleanup ();

  g_type_class_unref (g_type_class_peek (gst_object_get_type ()));
  g_type_class_unref (g_type_class_peek (gst_pad_get_type ()));
  g_type_class_unref (g_type_class_peek (gst_element_factory_get_type ()));
  g_type_class_unref (g_type_class_peek (gst_element_get_type ()));
  g_type_class_unref (g_type_class_peek (gst_type_find_factory_get_type ()));
  g_type_class_unref (g_type_class_peek (gst_bin_get_type ()));
#ifndef GST_DISABLE_INDEX
  g_type_class_unref (g_type_class_peek (gst_index_factory_get_type ()));
#endif /* GST_DISABLE_INDEX */
  g_type_class_unref (g_type_class_peek (gst_param_spec_fraction_get_type ()));

  gst_deinitialized = TRUE;
  GST_INFO ("deinitialized GStreamer");
}

/**
 * gst_version:
 * @major: pointer to a guint to store the major version number
 * @minor: pointer to a guint to store the minor version number
 * @micro: pointer to a guint to store the micro version number
 * @nano:  pointer to a guint to store the nano version number
 *
 * Gets the version number of the GStreamer library.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_version (guint * major, guint * minor, guint * micro, guint * nano)
{
  g_return_if_fail (major);
  g_return_if_fail (minor);
  g_return_if_fail (micro);
  g_return_if_fail (nano);

  *major = GST_VERSION_MAJOR;
  *minor = GST_VERSION_MINOR;
  *micro = GST_VERSION_MICRO;
  *nano = GST_VERSION_NANO;
}

/**
 * gst_version_string:
 *
 * This function returns a string that is useful for describing this version
 * of GStreamer to the outside world: user agent strings, logging, ...
 *
 * Returns: a newly allocated string describing this version of GStreamer.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif


gchar *
gst_version_string ()
{
  guint major, minor, micro, nano;

  gst_version (&major, &minor, &micro, &nano);
  if (nano == 0)
    return g_strdup_printf ("GStreamer %d.%d.%d", major, minor, micro);
  else if (nano == 1)
    return g_strdup_printf ("GStreamer %d.%d.%d (CVS)", major, minor, micro);
  else
    return g_strdup_printf ("GStreamer %d.%d.%d (prerelease)", major, minor,
        micro);
}

/**
 * gst_segtrap_is_enabled:
 *
 * Some functions in the GStreamer core might install a custom SIGSEGV handler
 * to better catch and report errors to the application. Currently this feature
 * is enabled by default when loading plugins.
 *
 * Applications might want to disable this behaviour with the
 * gst_segtrap_set_enabled() function. This is typically done if the application
 * wants to install its own handler without GStreamer interfering.
 *
 * Returns: %TRUE if GStreamer is allowed to install a custom SIGSEGV handler.
 *
 * Since: 0.10.10
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_segtrap_is_enabled (void)
{
  /* yeps, it's enabled when it's not disabled */
  return !_gst_disable_segtrap;
}

/**
 * gst_segtrap_set_enabled:
 * @enabled: whether a custom SIGSEGV handler should be installed.
 *
 * Applications might want to disable/enable the SIGSEGV handling of
 * the GStreamer core. See gst_segtrap_is_enabled() for more information.
 *
 * Since: 0.10.10
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_segtrap_set_enabled (gboolean enabled)
{
  _gst_disable_segtrap = !enabled;
}

/**
 * gst_registry_fork_is_enabled:
 *
 * By default GStreamer will perform a fork() when scanning and rebuilding the
 * registry file. 
 *
 * Applications might want to disable this behaviour with the
 * gst_registry_fork_set_enabled() function. 
 *
 * Returns: %TRUE if GStreamer will use fork() when rebuilding the registry. On
 * platforms without fork(), this function will always return %FALSE.
 *
 * Since: 0.10.10
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_registry_fork_is_enabled (void)
{
  return _gst_enable_registry_fork;
}

/**
 * gst_registry_fork_set_enabled:
 * @enabled: whether rebuilding the registry may fork
 *
 * Applications might want to disable/enable the usage of fork() when rebuilding
 * the registry. See gst_registry_fork_is_enabled() for more information.
 *
 * On platforms without fork(), this function will have no effect on the return
 * value of gst_registry_fork_is_enabled().
 *
 * Since: 0.10.10
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_registry_fork_set_enabled (gboolean enabled)
{
#ifdef HAVE_FORK
  _gst_enable_registry_fork = enabled;
#endif /* HAVE_FORK */
}


/**
 * gst_update_registry:
 *
 * Forces GStreamer to re-scan its plugin paths and update the default
 * plugin registry.
 *
 * Applications will almost never need to call this function, it is only
 * useful if the application knows new plugins have been installed (or old
 * ones removed) since the start of the application (or, to be precise, the
 * first call to gst_init()) and the application wants to make use of any
 * newly-installed plugins without restarting the application.
 *
 * Applications should assume that the registry update is neither atomic nor
 * thread-safe and should therefore not have any dynamic pipelines running
 * (including the playbin and decodebin elements) and should also not create
 * any elements or access the GStreamer registry while the update is in
 * progress.
 *
 * Note that this function may block for a significant amount of time.
 *
 * Returns: %TRUE if the registry has been updated successfully (does not
 *          imply that there were changes), otherwise %FALSE.
 *
 * Since: 0.10.12
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_update_registry (void)
{
  gboolean res = FALSE;

#ifndef GST_DISABLE_REGISTRY
  GError *err = NULL;

  res = ensure_current_registry (&err);
  if (err) {
    GST_WARNING ("registry update failed: %s", err->message);
    g_error_free (err);
  } else {
    GST_LOG ("registry update succeeded");
  }

  if (preload_plugins) {
    g_slist_foreach (preload_plugins, load_plugin_func, NULL);
  }
#else
  GST_WARNING ("registry update failed: %s", "registry disabled");
#endif /* GST_DISABLE_REGISTRY */

  return res;
}
