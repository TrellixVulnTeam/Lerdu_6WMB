/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *                    2005 David A. Schleef <ds@schleef.org>
 *
 * gstregistry.c: handle registry
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
 * SECTION:gstregistry
 * @short_description: Abstract base class for management of #GstPlugin objects
 * @see_also: #GstPlugin, #GstPluginFeature
 *
 * One registry holds the metadata of a set of plugins.
 * All registries build the #GstRegistryPool.
 *
 * <emphasis role="bold">Design:</emphasis>
 *
 * The #GstRegistry object is a list of plugins and some functions for dealing
 * with them. #GstPlugins are matched 1-1 with a file on disk, and may or may
 * not be loaded at a given time. There may be multiple #GstRegistry objects,
 * but the "default registry" is the only object that has any meaning to the
 * core.
 *
 * The registry.xml file is actually a cache of plugin information. This is
 * unlike versions prior to 0.10, where the registry file was the primary source
 * of plugin information, and was created by the gst-register command.
 *
 * The primary source, at all times, of plugin information is each plugin file
 * itself. Thus, if an application wants information about a particular plugin,
 * or wants to search for a feature that satisfies given criteria, the primary
 * means of doing so is to load every plugin and look at the resulting
 * information that is gathered in the default registry. Clearly, this is a time
 * consuming process, so we cache information in the registry.xml file.
 *
 * On startup, plugins are searched for in the plugin search path. This path can
 * be set directly using the %GST_PLUGIN_PATH environment variable. The registry
 * file is loaded from ~/.gstreamer-$GST_MAJORMINOR/registry-$ARCH.xml or the
 * file listed in the %GST_REGISTRY env var. The only reason to change the
 * registry location is for testing.
 *
 * For each plugin that is found in the plugin search path, there could be 3
 * possibilities for cached information:
 * <itemizedlist>
 *   <listitem>
 *     <para>the cache may not contain information about a given file.</para>
 *   </listitem>
 *   <listitem>
 *     <para>the cache may have stale information.</para>
 *   </listitem>
 *   <listitem>
 *     <para>the cache may have current information.</para>
 *   </listitem>
 * </itemizedlist>
 *
 * In the first two cases, the plugin is loaded and the cache updated. In
 * addition to these cases, the cache may have entries for plugins that are not
 * relevant to the current process. These are marked as not available to the
 * current process. If the cache is updated for whatever reason, it is marked
 * dirty.
 *
 * A dirty cache is written out at the end of initialization. Each entry is
 * checked to make sure the information is minimally valid. If not, the entry is
 * simply dropped.
 *
 * <emphasis role="bold">Implementation notes:</emphasis>
 *
 * The "cache" and "default registry" are different concepts and can represent
 * different sets of plugins. For various reasons, at init time, the cache is
 * stored in the default registry, and plugins not relevant to the current
 * process are marked with the %GST_PLUGIN_FLAG_CACHED bit. These plugins are
 * removed at the end of intitialization.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gst_private.h"
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <string.h>


#include "gstinfo.h"
#include "gstregistry.h"
#include "gstmarshal.h"
#include "gstfilter.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#include "helpfile.h" 
#endif

#define GST_CAT_DEFAULT GST_CAT_REGISTRY

/* the one instance of the default registry and the mutex protecting the
 * variable. */
static GStaticMutex _gst_registry_mutex = G_STATIC_MUTEX_INIT;
static GstRegistry *_gst_registry_default = NULL;

/* Element signals and args */
enum
{
  PLUGIN_ADDED,
  FEATURE_ADDED,
  LAST_SIGNAL
};

static void gst_registry_finalize (GObject * object);

static guint gst_registry_signals[LAST_SIGNAL] = { 0 };

static GstPluginFeature *gst_registry_lookup_feature_locked (GstRegistry *
    registry, const char *name);
static GstPlugin *gst_registry_lookup_locked (GstRegistry * registry,
    const char *filename);

G_DEFINE_TYPE (GstRegistry, gst_registry, GST_TYPE_OBJECT);
static GstObjectClass *parent_class = NULL;

static void
gst_registry_class_init (GstRegistryClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  /**
   * GstRegistry::plugin-added:
   * @registry: the registry that emitted the signal
   * @plugin: the plugin that has been added
   *
   * Signals that a plugin has been added to the registry (possibly
   * replacing a previously-added one by the same name)
   */
  gst_registry_signals[PLUGIN_ADDED] =
      g_signal_new ("plugin-added", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET (GstRegistryClass, plugin_added), NULL,
      NULL, gst_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

  /**
   * GstRegistry::feature-added:
   * @registry: the registry that emitted the signal
   * @feature: the feature that has been added
   *
   * Signals that a feature has been added to the registry (possibly
   * replacing a previously-added one by the same name)
   */
  gst_registry_signals[FEATURE_ADDED] =
      g_signal_new ("feature-added", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET (GstRegistryClass, feature_added),
      NULL, NULL, gst_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_registry_finalize);
}

static void
gst_registry_init (GstRegistry * registry)
{
  registry->feature_hash = g_hash_table_new (g_str_hash, g_str_equal);
}

static void
gst_registry_finalize (GObject * object)
{
  GstRegistry *registry = GST_REGISTRY (object);
  GList *plugins, *p;
  GList *features, *f;

  plugins = registry->plugins;
  registry->plugins = NULL;

  GST_DEBUG_OBJECT (registry, "registry finalize");
  p = plugins;
  while (p) {
    GstPlugin *plugin = p->data;

    if (plugin) {
      GST_LOG_OBJECT (registry, "removing plugin %s",
          gst_plugin_get_name (plugin));
      gst_object_unref (plugin);
    }
    p = g_list_next (p);
  }
  g_list_free (plugins);

  features = registry->features;
  registry->features = NULL;

  f = features;
  while (f) {
    GstPluginFeature *feature = f->data;

    if (feature) {
      GST_LOG_OBJECT (registry, "removing feature %p (%s)",
          feature, gst_plugin_feature_get_name (feature));
      gst_object_unref (feature);
    }
    f = g_list_next (f);
  }
  g_list_free (features);

  g_hash_table_destroy (registry->feature_hash);
  registry->feature_hash = NULL;

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

/**
 * gst_registry_get_default:
 *
 * Retrieves the default registry. The caller does not own a reference on the
 * registry, as it is alive as long as GStreamer is initialized.
 *
 * Returns: The default #GstRegistry.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstRegistry *
gst_registry_get_default (void)
{
  GstRegistry *registry;

  g_static_mutex_lock (&_gst_registry_mutex);
  if (G_UNLIKELY (!_gst_registry_default)) {
    _gst_registry_default = g_object_new (GST_TYPE_REGISTRY, NULL);
    gst_object_ref (GST_OBJECT_CAST (_gst_registry_default));
    gst_object_sink (GST_OBJECT_CAST (_gst_registry_default));
  }
  registry = _gst_registry_default;
  g_static_mutex_unlock (&_gst_registry_mutex);

  return registry;
}

/**
 * gst_registry_add_path:
 * @registry: the registry to add the path to
 * @path: the path to add to the registry
 *
 * Add the given path to the registry. The syntax of the
 * path is specific to the registry. If the path has already been
 * added, do nothing.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_registry_add_path (GstRegistry * registry, const gchar * path)
{
  g_return_if_fail (GST_IS_REGISTRY (registry));
  g_return_if_fail (path != NULL);

  if (strlen (path) == 0)
    goto empty_path;

  GST_OBJECT_LOCK (registry);
  if (g_list_find_custom (registry->paths, path, (GCompareFunc) strcmp))
    goto was_added;

  GST_INFO ("Adding plugin path: \"%s\"", path);
  registry->paths = g_list_append (registry->paths, g_strdup (path));
  GST_OBJECT_UNLOCK (registry);

  return;

empty_path:
  {
    GST_INFO ("Ignoring empty plugin path");
    return;
  }
was_added:
  {
    g_warning ("path %s already added to registry", path);
    GST_OBJECT_UNLOCK (registry);
    return;
  }
}

/**
 * gst_registry_get_path_list:
 * @registry: the registry to get the pathlist of
 *
 * Get the list of paths for the given registry.
 *
 * Returns: A Glist of paths as strings. g_list_free after use.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GList *
gst_registry_get_path_list (GstRegistry * registry)
{
  GList *list;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), NULL);

  GST_OBJECT_LOCK (registry);
  /* We don't need to copy the strings, because they won't be deleted
   * as long as the GstRegistry is around */
  list = g_list_copy (registry->paths);
  GST_OBJECT_UNLOCK (registry);

  return list;
}


/**
 * gst_registry_add_plugin:
 * @registry: the registry to add the plugin to
 * @plugin: the plugin to add
 *
 * Add the plugin to the registry. The plugin-added signal will be emitted.
 * This function will sink @plugin.
 *
 * Returns: TRUE on success.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_registry_add_plugin (GstRegistry * registry, GstPlugin * plugin)
{
  GstPlugin *existing_plugin;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), FALSE);
  g_return_val_if_fail (GST_IS_PLUGIN (plugin), FALSE);

  GST_OBJECT_LOCK (registry);
  existing_plugin = gst_registry_lookup_locked (registry, plugin->filename);
  if (G_UNLIKELY (existing_plugin)) {
    GST_DEBUG_OBJECT (registry,
        "Replacing existing plugin %p with new plugin %p for filename \"%s\"",
        existing_plugin, plugin, GST_STR_NULL (plugin->filename));
    registry->plugins = g_list_remove (registry->plugins, existing_plugin);
    gst_object_unref (existing_plugin);
  }

  GST_DEBUG_OBJECT (registry, "adding plugin %p for filename \"%s\"",
      plugin, GST_STR_NULL (plugin->filename));

  registry->plugins = g_list_prepend (registry->plugins, plugin);

  gst_object_ref (plugin);
  gst_object_sink (plugin);
  GST_OBJECT_UNLOCK (registry);

  GST_LOG_OBJECT (registry, "emitting plugin-added for filename \"%s\"",
      GST_STR_NULL (plugin->filename));
  g_signal_emit (G_OBJECT (registry), gst_registry_signals[PLUGIN_ADDED], 0,
      plugin);

  return TRUE;
}

static void
gst_registry_remove_features_for_plugin_unlocked (GstRegistry * registry,
    GstPlugin * plugin)
{
  GList *f;

  g_return_if_fail (GST_IS_REGISTRY (registry));
  g_return_if_fail (GST_IS_PLUGIN (plugin));

  /* Remove all features for this plugin */
  f = registry->features;
  while (f != NULL) {
    GList *next = g_list_next (f);
    GstPluginFeature *feature = f->data;

    if (feature && !strcmp (feature->plugin_name, gst_plugin_get_name (plugin))) {
      GST_DEBUG_OBJECT (registry, "removing feature %p (%s) for plugin %s",
          feature, gst_plugin_feature_get_name (feature),
          gst_plugin_get_name (plugin));

      registry->features = g_list_delete_link (registry->features, f);
      g_hash_table_remove (registry->feature_hash, feature->name);
      gst_object_unref (feature);
    }
    f = next;
  }
}

/**
 * gst_registry_remove_plugin:
 * @registry: the registry to remove the plugin from
 * @plugin: the plugin to remove
 *
 * Remove the plugin from the registry.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_registry_remove_plugin (GstRegistry * registry, GstPlugin * plugin)
{
  g_return_if_fail (GST_IS_REGISTRY (registry));
  g_return_if_fail (GST_IS_PLUGIN (plugin));

  GST_DEBUG_OBJECT (registry, "removing plugin %p (%s)",
      plugin, gst_plugin_get_name (plugin));

  GST_OBJECT_LOCK (registry);
  registry->plugins = g_list_remove (registry->plugins, plugin);
  gst_registry_remove_features_for_plugin_unlocked (registry, plugin);
  GST_OBJECT_UNLOCK (registry);
  gst_object_unref (plugin);
}

/**
 * gst_registry_add_feature:
 * @registry: the registry to add the plugin to
 * @feature: the feature to add
 *
 * Add the feature to the registry. The feature-added signal will be emitted.
 * This function sinks @feature.
 *
 * Returns: TRUE on success.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_registry_add_feature (GstRegistry * registry, GstPluginFeature * feature)
{
  GstPluginFeature *existing_feature;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), FALSE);
  g_return_val_if_fail (GST_IS_PLUGIN_FEATURE (feature), FALSE);
  g_return_val_if_fail (feature->name != NULL, FALSE);
  g_return_val_if_fail (feature->plugin_name != NULL, FALSE);

  GST_OBJECT_LOCK (registry);
  existing_feature = gst_registry_lookup_feature_locked (registry,
      feature->name);
  if (G_UNLIKELY (existing_feature)) {
    GST_DEBUG_OBJECT (registry, "replacing existing feature %p (%s)",
        existing_feature, feature->name);
    /* Remove the existing feature from the list now, before we insert the new
     * one, but don't unref yet because the hash is still storing a reference to     * it. */
    registry->features = g_list_remove (registry->features, existing_feature);
  }

  GST_DEBUG_OBJECT (registry, "adding feature %p (%s)", feature, feature->name);

  registry->features = g_list_prepend (registry->features, feature);
  g_hash_table_replace (registry->feature_hash, feature->name, feature);

  if (G_UNLIKELY (existing_feature)) {
    /* We unref now. No need to remove the feature name from the hash table, it      * got replaced by the new feature */
    gst_object_unref (existing_feature);
  }

  gst_object_ref (feature);
  gst_object_sink (feature);
  GST_OBJECT_UNLOCK (registry);

  GST_LOG_OBJECT (registry, "emitting feature-added for %s", feature->name);
  g_signal_emit (G_OBJECT (registry), gst_registry_signals[FEATURE_ADDED], 0,
      feature);

  return TRUE;
}

/**
 * gst_registry_remove_feature:
 * @registry: the registry to remove the feature from
 * @feature: the feature to remove
 *
 * Remove the feature from the registry.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_registry_remove_feature (GstRegistry * registry, GstPluginFeature * feature)
{
  g_return_if_fail (GST_IS_REGISTRY (registry));
  g_return_if_fail (GST_IS_PLUGIN_FEATURE (feature));

  GST_DEBUG_OBJECT (registry, "removing feature %p (%s)",
      feature, gst_plugin_feature_get_name (feature));

  GST_OBJECT_LOCK (registry);
  registry->features = g_list_remove (registry->features, feature);
  g_hash_table_remove (registry->feature_hash, feature->name);
  GST_OBJECT_UNLOCK (registry);
  gst_object_unref (feature);
}

/**
 * gst_registry_plugin_filter:
 * @registry: registry to query
 * @filter: the filter to use
 * @first: only return first match
 * @user_data: user data passed to the filter function
 *
 * Runs a filter against all plugins in the registry and returns a #GList with
 * the results. If the first flag is set, only the first match is
 * returned (as a list with a single object).
 * Every plugin is reffed; use gst_plugin_list_free() after use, which
 * will unref again.
 *
 * Returns: a #GList of #GstPlugin. Use gst_plugin_list_free() after usage.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GList *
gst_registry_plugin_filter (GstRegistry * registry,
    GstPluginFilter filter, gboolean first, gpointer user_data)
{
  GList *list;
  GList *g;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), NULL);

  GST_OBJECT_LOCK (registry);
  list = gst_filter_run (registry->plugins, (GstFilterFunc) filter, first,
      user_data);
  for (g = list; g; g = g->next) {
    gst_object_ref (GST_PLUGIN_CAST (g->data));
  }
  GST_OBJECT_UNLOCK (registry);

  return list;
}

/**
 * gst_registry_feature_filter:
 * @registry: registry to query
 * @filter: the filter to use
 * @first: only return first match
 * @user_data: user data passed to the filter function
 *
 * Runs a filter against all features of the plugins in the registry
 * and returns a GList with the results.
 * If the first flag is set, only the first match is
 * returned (as a list with a single object).
 *
 * Returns: a GList of plugin features, gst_plugin_feature_list_free after use.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GList *
gst_registry_feature_filter (GstRegistry * registry,
    GstPluginFeatureFilter filter, gboolean first, gpointer user_data)
{
  GList *list;
  GList *g;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), NULL);

  GST_OBJECT_LOCK (registry);
  list = gst_filter_run (registry->features, (GstFilterFunc) filter, first,
      user_data);
  for (g = list; g; g = g->next) {
    gst_object_ref (GST_PLUGIN_FEATURE_CAST (g->data));
  }
  GST_OBJECT_UNLOCK (registry);

  return list;
}

/**
 * gst_registry_find_plugin:
 * @registry: the registry to search
 * @name: the plugin name to find
 *
 * Find the plugin with the given name in the registry.
 * The plugin will be reffed; caller is responsible for unreffing.
 *
 * Returns: The plugin with the given name or NULL if the plugin was not found.
 * gst_object_unref() after usage.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstPlugin *
gst_registry_find_plugin (GstRegistry * registry, const gchar * name)
{
  GList *walk;
  GstPlugin *result = NULL;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  walk = gst_registry_plugin_filter (registry,
      (GstPluginFilter) gst_plugin_name_filter, TRUE, (gpointer) name);
  if (walk) {
    result = GST_PLUGIN_CAST (walk->data);

    gst_object_ref (result);
    gst_plugin_list_free (walk);
  }

  return result;
}

/**
 * gst_registry_find_feature:
 * @registry: the registry to search
 * @name: the pluginfeature name to find
 * @type: the pluginfeature type to find
 *
 * Find the pluginfeature with the given name and type in the registry.
 *
 * Returns: The pluginfeature with the given name and type or NULL
 * if the plugin was not found. gst_object_unref() after usage.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstPluginFeature *
gst_registry_find_feature (GstRegistry * registry, const gchar * name,
    GType type)
{
  GstPluginFeature *feature = NULL;
  GList *walk;
  GstTypeNameData data;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), NULL);
  g_return_val_if_fail (name != NULL, NULL);
  g_return_val_if_fail (g_type_is_a (type, GST_TYPE_PLUGIN_FEATURE), NULL);

  data.name = name;
  data.type = type;

  walk = gst_registry_feature_filter (registry,
      (GstPluginFeatureFilter) gst_plugin_feature_type_name_filter,
      TRUE, &data);

  if (walk) {
    feature = GST_PLUGIN_FEATURE_CAST (walk->data);

    gst_object_ref (feature);
    gst_plugin_feature_list_free (walk);
  }

  return feature;
}

/**
 * gst_registry_get_feature_list:
 * @registry: a #GstRegistry
 * @type: a #GType.
 *
 * Retrieves a #GList of #GstPluginFeature of @type.
 *
 * Returns: a #GList of #GstPluginFeature of @type. gst_plugin_feature_list_free
 * after usage.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GList *
gst_registry_get_feature_list (GstRegistry * registry, GType type)
{
  GstTypeNameData data;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), NULL);
  g_return_val_if_fail (g_type_is_a (type, GST_TYPE_PLUGIN_FEATURE), NULL);

  data.type = type;
  data.name = NULL;

  return gst_registry_feature_filter (registry,
      (GstPluginFeatureFilter) gst_plugin_feature_type_name_filter,
      FALSE, &data);
}

/**
 * gst_registry_get_plugin_list:
 * @registry: the registry to search
 *
 * Get a copy of all plugins registered in the given registry. The refcount
 * of each element in the list in incremented.
 *
 * Returns: a #GList of #GstPlugin. gst_plugin_list_free after use.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GList *
gst_registry_get_plugin_list (GstRegistry * registry)
{
  GList *list;
  GList *g;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), NULL);

  GST_OBJECT_LOCK (registry);
  list = g_list_copy (registry->plugins);
  for (g = list; g; g = g->next) {
    gst_object_ref (GST_PLUGIN_CAST (g->data));
  }
  GST_OBJECT_UNLOCK (registry);

  return list;
}

static GstPluginFeature *
gst_registry_lookup_feature_locked (GstRegistry * registry, const char *name)
{
  if (G_UNLIKELY (name == NULL))
    return NULL;

  return g_hash_table_lookup (registry->feature_hash, name);
}

/**
 * gst_registry_lookup_feature:
 * @registry: a #GstRegistry
 * @name: a #GstPluginFeature name
 *
 * Find a #GstPluginFeature with @name in @registry.
 *
 * Returns: a #GstPluginFeature with its refcount incremented, use
 * gst_object_unref() after usage.
 *
 * MT safe.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstPluginFeature *
gst_registry_lookup_feature (GstRegistry * registry, const char *name)
{
  GstPluginFeature *feature;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  GST_OBJECT_LOCK (registry);
  feature = gst_registry_lookup_feature_locked (registry, name);
  if (feature)
    gst_object_ref (feature);
  GST_OBJECT_UNLOCK (registry);

  return feature;
}

static GstPlugin *
gst_registry_lookup_locked (GstRegistry * registry, const char *filename)
{
  GList *g;
  GstPlugin *plugin;
  gchar *basename;

  if (G_UNLIKELY (filename == NULL))
    return NULL;

  basename = g_path_get_basename (filename);
  /* FIXME: use GTree speed up lookups */
  for (g = registry->plugins; g; g = g_list_next (g)) {
    plugin = GST_PLUGIN_CAST (g->data);
    if (plugin->basename && strcmp (basename, plugin->basename) == 0) {
      g_free (basename);
      return plugin;
    }
  }

  g_free (basename);
  return NULL;
}

/**
 * gst_registry_lookup:
 * @registry: the registry to look up in
 * @filename: the name of the file to look up
 *
 * Look up a plugin in the given registry with the given filename.
 * If found, plugin is reffed.
 *
 * Returns: the #GstPlugin if found, or NULL if not. gst_object_unref()
 * after usage.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstPlugin *
gst_registry_lookup (GstRegistry * registry, const char *filename)
{
  GstPlugin *plugin;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), NULL);
  g_return_val_if_fail (filename != NULL, NULL);

  GST_OBJECT_LOCK (registry);
  plugin = gst_registry_lookup_locked (registry, filename);
  if (plugin)
    gst_object_ref (plugin);
  GST_OBJECT_UNLOCK (registry);

  return plugin;
}

static gboolean
gst_registry_scan_path_level (GstRegistry * registry, const gchar * path,
    int level)
{
  GDir *directory[2];
  GDir *dir;
  const gchar *dirent;
  gchar *filename;
  GstPlugin *plugin;
  GstPlugin *newplugin;
  gboolean changed = FALSE;

#ifdef __SYMBIAN32__  
  GMappedFile *mapped = NULL; 
  gchar *contents = NULL;  
  gchar** arglist=NULL;  
  GError *err = NULL;  
  char* file;
  gint i = 2;
#endif
  directory[0] = g_dir_open ( PLUGIN_LIST, 0, NULL );
  directory[1] = g_dir_open ( PLUGIN_LIST_IN_ROM, 0, NULL );
  while( i ){
   --i; 
    dir = directory[i];
    if (!dir)
        continue;

  while ((dirent = g_dir_read_name (dir))) 
  {    
#ifdef __SYMBIAN32__
    
   if( i == 1 )
        filename = g_strjoin ("\\", PLUGIN_LIST_IN_ROM, dirent, NULL);
    else
        filename = g_strjoin ("\\", PLUGIN_LIST, dirent, NULL);
    mapped = g_mapped_file_new (filename, FALSE, &err);
    contents = g_mapped_file_get_contents (mapped);
    arglist = g_strsplit(contents, "\r\n", -1 );
    
    g_free (filename);
     
    while( *arglist )
    {
      filename = *arglist;
      // trim white spaces from start
      while(*filename ==' ' )
          filename++;
      
      file = filename;
      
      // trim white spaces from last
      while(*file != ' ' && *file != '\0')
          file++;
      *file = '\0';
      
      arglist++;
      /// get the full path of DLL 
      filename = libgstreamer_get_dll_path( filename );

#else
      
	  filename = g_strjoin ("\\", path, dirent, NULL);
#endif      
    GST_LOG_OBJECT (registry, "examining file: %s", filename);

    if (g_file_test (filename, G_FILE_TEST_IS_DIR)) {
      if (level > 0) {
        GST_LOG_OBJECT (registry, "found directory, recursing");
        changed |= gst_registry_scan_path_level (registry, filename, level - 1);
      } else {
        GST_LOG_OBJECT (registry,
            "found directory, but recursion level is too deep");
      }
      g_free (filename);
      continue;
    }
      if (!g_file_test (filename, G_FILE_TEST_IS_REGULAR)) {
        GST_LOG_OBJECT (registry, "not a regular file, ignoring");
        g_free (filename);
        continue;
      }
    if (!g_str_has_suffix (filename, ".so") &&
        !g_str_has_suffix (filename, ".sl") &&
        !g_str_has_suffix (filename, ".dll") &&
        !g_str_has_suffix (filename, ".dynlib")) {
      GST_LOG_OBJECT (registry,
          "extension is not recognized as module file, ignoring");
      g_free (filename);
      continue;
    }

    /* plug-ins are considered unique by basename; if the given name
     * was already seen by the registry, we ignore it */
    plugin = gst_registry_lookup (registry, filename);
    if (plugin) {
      struct stat file_status;

      if (stat (filename, &file_status)) {
        /* Plugin will be removed from cache after the scan completes if it
         * is still marked 'cached' */
        g_free (filename);
        gst_object_unref (plugin);
        continue;
      }
      if (plugin->registered) {
        GST_DEBUG_OBJECT (registry,
            "plugin already registered from path \"%s\"",
            GST_STR_NULL (plugin->filename));
        g_free (filename);
        gst_object_unref (plugin);
        continue;
      }
      if (plugin->file_mtime == file_status.st_mtime &&
          plugin->file_size == file_status.st_size) {
        GST_LOG_OBJECT (registry, "file %s cached", filename);
        plugin->flags &= ~GST_PLUGIN_FLAG_CACHED;
        GST_LOG_OBJECT (registry, "marking plugin %p as registered as %s",
            plugin, filename);
        plugin->registered = TRUE;
        /* Update the file path on which we've seen this cached plugin
         * to ensure the registry cache will reflect up to date information */
        if (strcmp (plugin->filename, filename) != 0) {
          g_free (plugin->filename);
          plugin->filename = g_strdup (filename);
          changed = TRUE;
        }
      } else {
        GST_INFO_OBJECT (registry, "cached info for %s is stale", filename);
        GST_DEBUG_OBJECT (registry, "mtime %ld != %ld or size %"
            G_GINT64_FORMAT " != %"
            G_GINT64_FORMAT, plugin->file_mtime, file_status.st_mtime,
            (gint64) plugin->file_size, (gint64) file_status.st_size);
        gst_registry_remove_plugin (gst_registry_get_default (), plugin);
        /* We don't use a GError here because a failure to load some shared
         * objects as plugins is normal (particularly in the uninstalled case)
         */
        newplugin = gst_plugin_load_file (filename, NULL);
        if (newplugin) {
          GST_DEBUG_OBJECT (registry, "marking new plugin %p as registered",
              newplugin);
          newplugin->registered = TRUE;
          gst_object_unref (newplugin);
        }
        changed = TRUE;
      }
      gst_object_unref (plugin);

    } else {
      GST_DEBUG_OBJECT (registry, "file %s not yet in registry", filename);
      newplugin = gst_plugin_load_file (filename, NULL);
      if (newplugin) {
        newplugin->registered = TRUE;
        gst_object_unref (newplugin);
        changed = TRUE;
      }
    }

    g_free (filename);
  }
#ifdef __SYMBIAN32__          
 }  
#endif
  g_dir_close (dir);
}
  return changed;
}

/**
 * gst_registry_scan_path:
 * @registry: the registry to add the path to
 * @path: the path to add to the registry
 *
 * Add the given path to the registry. The syntax of the
 * path is specific to the registry. If the path has already been
 * added, do nothing.
 *
 * Returns: %TRUE if registry changed
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_registry_scan_path (GstRegistry * registry, const gchar * path)
{
  gboolean changed;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), FALSE);
  g_return_val_if_fail (path != NULL, FALSE);

  GST_DEBUG_OBJECT (registry, "scanning path %s", path);
  changed = gst_registry_scan_path_level (registry, path, 10);

  GST_DEBUG_OBJECT (registry, "registry changed in path %s: %d", path, changed);

  return changed;
}

/* Unref all plugins marked 'cached', to clear old plugins that no
 * longer exist. Returns TRUE if any plugins were removed */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
_priv_gst_registry_remove_cache_plugins (GstRegistry * registry)
{
  GList *g;
  GList *g_next;
  GstPlugin *plugin;
  gboolean changed = FALSE;

  g_return_val_if_fail (GST_IS_REGISTRY (registry), FALSE);

  GST_OBJECT_LOCK (registry);

  GST_DEBUG_OBJECT (registry, "removing cached plugins");
  g = registry->plugins;
  while (g) {
    g_next = g->next;
    plugin = g->data;
    if (plugin->flags & GST_PLUGIN_FLAG_CACHED) {
      GST_DEBUG_OBJECT (registry, "removing cached plugin \"%s\"",
          GST_STR_NULL (plugin->filename));
      registry->plugins = g_list_delete_link (registry->plugins, g);
      gst_registry_remove_features_for_plugin_unlocked (registry, plugin);
      gst_object_unref (plugin);
      changed = TRUE;
    }
    g = g_next;
  }

  GST_OBJECT_UNLOCK (registry);

  return changed;
}


static gboolean
_gst_plugin_feature_filter_plugin_name (GstPluginFeature * feature,
    gpointer user_data)
{
  return (strcmp (feature->plugin_name, (gchar *) user_data) == 0);
}

/**
 * gst_registry_get_feature_list_by_plugin:
 * @registry: a #GstRegistry.
 * @name: a plugin name.
 *
 * Retrieves a #GList of features of the plugin with name @name.
 *
 * Returns: a #GList of #GstPluginFeature. gst_plugin_feature_list_free() after usage.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GList *
gst_registry_get_feature_list_by_plugin (GstRegistry * registry,
    const gchar * name)
{
  g_return_val_if_fail (GST_IS_REGISTRY (registry), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  return gst_registry_feature_filter (registry,
      _gst_plugin_feature_filter_plugin_name, FALSE, (gpointer) name);
}

/* Unref and delete the default registry */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
_priv_gst_registry_cleanup ()
{
  GstRegistry *registry;

  g_static_mutex_lock (&_gst_registry_mutex);
  if ((registry = _gst_registry_default) != NULL) {
    _gst_registry_default = NULL;
  }
  g_static_mutex_unlock (&_gst_registry_mutex);

  /* unref outside of the lock because we can. */
  if (registry)
    gst_object_unref (registry);
}

/**
 * gst_default_registry_check_feature_version:
 * @feature_name: the name of the feature (e.g. "oggdemux")
 * @min_major: the minimum major version number
 * @min_minor: the minimum minor version number
 * @min_micro: the minimum micro version number
 *
 * Checks whether a plugin feature by the given name exists in the
 * default registry and whether its version is at least the
 * version required.
 *
 * Returns: #TRUE if the feature could be found and the version is
 * the same as the required version or newer, and #FALSE otherwise.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_default_registry_check_feature_version (const gchar * feature_name,
    guint min_major, guint min_minor, guint min_micro)
{
  GstPluginFeature *feature;
  GstRegistry *registry;
  gboolean ret = FALSE;

  g_return_val_if_fail (feature_name != NULL, FALSE);

  GST_DEBUG ("Looking up plugin feature '%s'", feature_name);

  registry = gst_registry_get_default ();
  feature = gst_registry_lookup_feature (registry, feature_name);
  if (feature) {
    ret = gst_plugin_feature_check_version (feature, min_major, min_minor,
        min_micro);
    gst_object_unref (feature);
  } else {
    GST_DEBUG ("Could not find plugin feature '%s'", feature_name);
  }

  return ret;
}
