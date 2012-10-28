/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2004> Thomas Vander Stichele <thomas at apestaart dot org>
 *
 * gsttcp.h: helper functions
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

#ifndef __GST_TCP_HELP_H__
#define __GST_TCP_HELP_H__

#include "gsttcp-enumtypes.h"
#include <gst/gst.h>
#undef GST_DISABLE_DEPRECATED
#include <gst/dataprotocol/dataprotocol.h>

#define TCP_HIGHEST_PORT        65535
#define TCP_DEFAULT_HOST        "localhost"
#define TCP_DEFAULT_PORT        4953

G_BEGIN_DECLS

typedef enum
{
  GST_TCP_PROTOCOL_NONE,
  GST_TCP_PROTOCOL_GDP
} GstTCPProtocol;
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gchar * gst_tcp_host_to_ip (GstElement *element, const gchar *host);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gint gst_tcp_socket_write (int socket, const void *buf, size_t count);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void gst_tcp_socket_close (GstPollFD *socket);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstFlowReturn gst_tcp_read_buffer (GstElement * this, int socket, GstPoll * fdset, GstBuffer **buf);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstFlowReturn gst_tcp_gdp_read_buffer (GstElement * this, int socket, GstPoll * fdset, GstBuffer **buf);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstFlowReturn gst_tcp_gdp_read_caps (GstElement * this, int socket, GstPoll * fdset, GstCaps **caps);

GstEvent * gst_tcp_gdp_read_event (GstElement *elem, int socket, GstPoll * fdset);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean gst_tcp_gdp_write_buffer (GstElement *elem, int socket, GstBuffer *buffer, gboolean fatal, const gchar *host, int port);
gboolean gst_tcp_gdp_write_event (GstElement *elem, int socket, GstEvent *event, gboolean fatal, const gchar *host, int port);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean gst_tcp_gdp_write_caps (GstElement *elem, int socket, const GstCaps *caps, gboolean fatal, const gchar *host, int port);

G_END_DECLS

#endif /* __GST_TCP_HELP_H__ */
