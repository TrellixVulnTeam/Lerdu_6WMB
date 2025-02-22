/* GStreamer PCM/A-Law conversions
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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
#include "mulaw-encode.h"
#include "mulaw-decode.h"

GstStaticPadTemplate mulaw_dec_src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw-int, "
        "rate = (int) [ 8000, 192000 ], "
        "channels = (int) [ 1, 2 ], "
        "endianness = (int) BYTE_ORDER, "
        "width = (int) 16, " "width = (int) 16, " "signed = (boolean) True")
    );

GstStaticPadTemplate mulaw_dec_sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-mulaw, "
        "rate = [ 8000 , 192000 ], " "channels = [ 1 , 2 ]")
    );

GstStaticPadTemplate mulaw_enc_sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw-int, "
        "rate = (int) [ 8000, 192000 ], "
        "channels = (int) [ 1, 2 ], "
        "endianness = (int) BYTE_ORDER, "
        "width = (int) 16, " "width = (int) 16, " "signed = (boolean) True")
    );

GstStaticPadTemplate mulaw_enc_src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-mulaw, "
        "rate = [ 8000 , 192000 ], " "channels = [ 1 , 2 ]")
    );

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_element_register (plugin, "mulawenc",
          GST_RANK_NONE, GST_TYPE_MULAWENC) ||
      !gst_element_register (plugin, "mulawdec",
          GST_RANK_PRIMARY, GST_TYPE_MULAWDEC))
    return FALSE;

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "mulaw",
    "MuLaw audio conversion routines",
    plugin_init, VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)


EXPORT_C GstPluginDesc* _GST_PLUGIN_DESC()
{
	return &gst_plugin_desc;
}
