/* GStreamer
 * Copyright (C) 1999 Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) 2004 Wim Taymans <wim.taymans@gmail.com>
 * Copyright (C) 2007 Peter Kjellerstedt <pkj@axis.com>
 *
 * gstpoll.h: File descriptor set
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

#ifndef __GST_POLL_H__
#define __GST_POLL_H__

#include <glib.h>
#include <glib-object.h>

#include <gst/gstclock.h>

G_BEGIN_DECLS

/**
 * GstPoll:
 *
 * A set of file/network descriptors.
 */
typedef struct _GstPoll GstPoll;

/**
 * GstPollFD:
 * @fd: a file descriptor
 *
 * A file descriptor object.
 */
typedef struct {
  int fd;

  /*< private >*/
  gint idx;
} GstPollFD;

/**
 * GST_POLL_FD_INIT:
 *
 * A #GstPollFD must be initialized with this macro, before it can be
 * used. This macro can used be to initialize a variable, but it cannot
 * be assigned to a variable. In that case you have to use
 * gst_poll_fd_init().
 *
 * Since: 0.10.18
 */
#define GST_POLL_FD_INIT  { -1, -1 }
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstPoll*        gst_poll_new              (gboolean controllable);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void            gst_poll_free             (GstPoll *set);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void            gst_poll_fd_init          (GstPollFD *fd);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean        gst_poll_add_fd           (GstPoll *set, GstPollFD *fd);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_poll_remove_fd        (GstPoll *set, GstPollFD *fd);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean        gst_poll_fd_ctl_write     (GstPoll *set, GstPollFD *fd, gboolean active);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_poll_fd_ctl_read      (GstPoll *set, GstPollFD *fd, gboolean active);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void            gst_poll_fd_ignored       (GstPoll *set, GstPollFD *fd);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean        gst_poll_fd_has_closed    (const GstPoll *set, GstPollFD *fd);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_poll_fd_has_error     (const GstPoll *set, GstPollFD *fd);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_poll_fd_can_read      (const GstPoll *set, GstPollFD *fd);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_poll_fd_can_write     (const GstPoll *set, GstPollFD *fd);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gint            gst_poll_wait             (GstPoll *set, GstClockTime timeout);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean        gst_poll_set_controllable (GstPoll *set, gboolean controllable);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void            gst_poll_restart          (GstPoll *set);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void            gst_poll_set_flushing     (GstPoll *set, gboolean flushing);

G_END_DECLS

#endif /* __GST_POLL_H__ */
