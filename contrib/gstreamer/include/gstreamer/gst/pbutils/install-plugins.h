/* GStreamer base utils library plugin install support for applications
 * Copyright (C) 2007 Tim-Philipp Müller <tim centricular net>
 * Copyright (C) 2006 Ryan Lortie <desrt desrt ca>
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

#ifndef __GST_PB_UTILS_INSTALL_PLUGINS_H__
#define __GST_PB_UTILS_INSTALL_PLUGINS_H__

#include <glib-object.h>

G_BEGIN_DECLS

/*
 * functions for use by applications to initiate installation of missing plugins
 */

/**
 * GstInstallPluginsReturn:
 * @GST_INSTALL_PLUGINS_SUCCESS: all of the requested plugins could be
 *     installed
 * @GST_INSTALL_PLUGINS_NOT_FOUND: no appropriate installation candidate for
 *     any of the requested plugins could be found. Only return this if nothing
 *     has been installed. Return #GST_INSTALL_PLUGINS_PARTIAL_SUCCESS if
 *     some (but not all) of the requested plugins could be installed.
 * @GST_INSTALL_PLUGINS_ERROR: an error occured during the installation. If
 *     this happens, the  user has already seen an error message and another
 *     one should not be displayed
 * @GST_INSTALL_PLUGINS_CRASHED: the installer had an unclean exit code
 *     (ie. death by signal)
 * @GST_INSTALL_PLUGINS_PARTIAL_SUCCESS: some of the requested plugins could
 *     be installed, but not all
 * @GST_INSTALL_PLUGINS_USER_ABORT: the user has aborted the installation
 * @GST_INSTALL_PLUGINS_INVALID: the helper returned an invalid status code
 * @GST_INSTALL_PLUGINS_STARTED_OK: returned by gst_install_plugins_async() to
 *     indicate that everything went fine so far and the provided callback
 *     will be called with the result of the installation later
 * @GST_INSTALL_PLUGINS_INTERNAL_FAILURE: some internal failure has
 *     occured when trying to start the installer
 * @GST_INSTALL_PLUGINS_HELPER_MISSING: the helper script to call the
 *     actual installer is not installed
 * @GST_INSTALL_PLUGINS_INSTALL_IN_PROGRESS: a previously-started plugin
 *     installation is still in progress, try again later
 *
 * Result codes returned by gst_install_plugins_async() and
 * gst_install_plugins_sync(), and also the result code passed to the
 * #GstInstallPluginsResultFunc specified with gst_install_plugin_async().
 *
 * These codes indicate success or failure of starting an external installer
 * program and to what extent the requested plugins could be installed.
 *
 * Since: 0.10.12
 */
typedef enum {
  /* Return codes from the installer. Returned by gst_install_plugins_sync(),
   * or passed as result code to your #GstInstallPluginsResultFunc */
  GST_INSTALL_PLUGINS_SUCCESS = 0,
  GST_INSTALL_PLUGINS_NOT_FOUND = 1,
  GST_INSTALL_PLUGINS_ERROR = 2,
  GST_INSTALL_PLUGINS_PARTIAL_SUCCESS = 3,
  GST_INSTALL_PLUGINS_USER_ABORT = 4,

  /* Returned by gst_install_plugins_sync(), or passed as result code to your
   * #GstInstallPluginsResultFunc */
  GST_INSTALL_PLUGINS_CRASHED = 100,
  GST_INSTALL_PLUGINS_INVALID,

  /* Return codes from starting the external helper, may be returned by both
   * gst_install_plugins_sync() and gst_install_plugins_async(), but should
   * never be seen by a #GstInstallPluginsResultFunc */
  GST_INSTALL_PLUGINS_STARTED_OK = 200,
  GST_INSTALL_PLUGINS_INTERNAL_FAILURE,
  GST_INSTALL_PLUGINS_HELPER_MISSING,
  GST_INSTALL_PLUGINS_INSTALL_IN_PROGRESS
} GstInstallPluginsReturn;

/**
 * GstInstallPluginsContext:
 *
 * Opaque context structure for the plugin installation. Use the provided
 * API to set details on it.
 *
 * Since: 0.10.12
 */

#define GST_TYPE_INSTALL_PLUGINS_CONTEXT	(gst_install_plugins_context_get_type())

typedef struct _GstInstallPluginsContext GstInstallPluginsContext;
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstInstallPluginsContext * gst_install_plugins_context_new (void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void   gst_install_plugins_context_free    (GstInstallPluginsContext * ctx);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void   gst_install_plugins_context_set_xid (GstInstallPluginsContext * ctx,
                                            guint                      xid);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GType  gst_install_plugins_context_get_type (void);

/**
 * GstInstallPluginsResultFunc:
 * @result: whether the installation of the requested plugins succeeded or not
 * @user_data: the user data passed to gst_install_plugins_async()
 *
 * The prototype of the callback function that will be called once the
 * external plugin installer program has returned. You only need to provide
 * a callback function if you are using the asynchronous interface.
 *
 * Since: 0.10.12
 */
typedef void (*GstInstallPluginsResultFunc) (GstInstallPluginsReturn  result,
                                             gpointer                 user_data);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstInstallPluginsReturn  gst_install_plugins_async (gchar                    ** details,
                                                    GstInstallPluginsContext  * ctx,
                                                    GstInstallPluginsResultFunc func,
                                                    gpointer                    user_data);

GstInstallPluginsReturn  gst_install_plugins_sync  (gchar                    ** details,
                                                    GstInstallPluginsContext  * ctx);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


const gchar * gst_install_plugins_return_get_name (GstInstallPluginsReturn ret);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean      gst_install_plugins_installation_in_progress (void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean      gst_install_plugins_supported (void);

G_END_DECLS

#endif /* __GST_PB_UTILS_INSTALL_PLUGINS_H__ */

