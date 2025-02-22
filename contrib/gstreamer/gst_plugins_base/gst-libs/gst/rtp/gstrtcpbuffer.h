/* GStreamer
 * Copyright (C) <2007> Wim Taymans <wim@fluendo.com>
 *
 * gstrtcpbuffer.h: various helper functions to manipulate buffers
 *     with RTCP payload.
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

#ifndef __GST_RTCPBUFFER_H__
#define __GST_RTCPBUFFER_H__

#include <gst/gst.h>

G_BEGIN_DECLS

/**
 * GST_RTCP_VERSION:
 *
 * The supported RTCP version 2.
 */
#define GST_RTCP_VERSION 2

/** 
 * GstRTCPType:
 * @GST_RTCP_TYPE_INVALID: Invalid type
 * @GST_RTCP_TYPE_SR: Sender report
 * @GST_RTCP_TYPE_RR: Receiver report
 * @GST_RTCP_TYPE_SDES: Source description
 * @GST_RTCP_TYPE_BYE: Goodbye
 * @GST_RTCP_TYPE_APP: Application defined
 *
 * Different RTCP packet types.
 */
typedef enum
{
  GST_RTCP_TYPE_INVALID = 0,
  GST_RTCP_TYPE_SR      = 200,
  GST_RTCP_TYPE_RR      = 201,
  GST_RTCP_TYPE_SDES    = 202,
  GST_RTCP_TYPE_BYE     = 203,
  GST_RTCP_TYPE_APP     = 204
} GstRTCPType;

/** 
 * GstRTCPSDESType:
 * @GST_RTCP_SDES_INVALID: Invalid SDES entry
 * @GST_RTCP_SDES_END: End of SDES list
 * @GST_RTCP_SDES_CNAME: Canonical name
 * @GST_RTCP_SDES_NAME: User name
 * @GST_RTCP_SDES_EMAIL: User's electronic mail address
 * @GST_RTCP_SDES_PHONE: User's phone number
 * @GST_RTCP_SDES_LOC: Geographic user location
 * @GST_RTCP_SDES_TOOL: Name of application or tool
 * @GST_RTCP_SDES_NOTE: Notice about the source
 * @GST_RTCP_SDES_PRIV: Private extensions
 *
 * Different types of SDES content.
 */
typedef enum 
{
  GST_RTCP_SDES_INVALID  = -1,
  GST_RTCP_SDES_END      = 0,
  GST_RTCP_SDES_CNAME    = 1,
  GST_RTCP_SDES_NAME     = 2,
  GST_RTCP_SDES_EMAIL    = 3,
  GST_RTCP_SDES_PHONE    = 4,
  GST_RTCP_SDES_LOC      = 5,
  GST_RTCP_SDES_TOOL     = 6,
  GST_RTCP_SDES_NOTE     = 7,
  GST_RTCP_SDES_PRIV     = 8
} GstRTCPSDESType;

/**
 * GST_RTCP_MAX_SDES:
 *
 * The maximum text length for an SDES item.
 */
#define GST_RTCP_MAX_SDES 255

/**
 * GST_RTCP_MAX_RB_COUNT:
 *
 * The maximum amount of Receiver report blocks in RR and SR messages.
 */
#define GST_RTCP_MAX_RB_COUNT   31

/**
 * GST_RTCP_MAX_SDES_ITEM_COUNT:
 *
 * The maximum amount of SDES items.
 */
#define GST_RTCP_MAX_SDES_ITEM_COUNT   31

/**
 * GST_RTCP_MAX_BYE_SSRC_COUNT:
 *
 * The maximum amount of SSRCs in a BYE packet.
 */
#define GST_RTCP_MAX_BYE_SSRC_COUNT   31

/**
 * GST_RTCP_VALID_MASK:
 *
 * Mask for version, padding bit and packet type pair
 */
#define GST_RTCP_VALID_MASK (0xc000 | 0x2000 | 0xfe)
/**
 * GST_RTCP_VALID_VALUE:
 *
 * Valid value for the first two bytes of an RTCP packet after applying
 * #GST_RTCP_VALID_MASK to them.
 */
#define GST_RTCP_VALID_VALUE ((GST_RTCP_VERSION << 14) | GST_RTCP_TYPE_SR)

typedef struct _GstRTCPPacket GstRTCPPacket;

/**
 * GstRTCPPacket:
 * @buffer: pointer to RTCP buffer
 * @offset: offset of packet in buffer data
 *
 * Data structure that points to a packet at @offset in @buffer. 
 * The size of the structure is made public to allow stack allocations.
 */
struct _GstRTCPPacket
{ 
  GstBuffer   *buffer;
  guint        offset;
  
  /*< private >*/
  gboolean     padding;      /* padding field of current packet */
  guint8       count;        /* count field of current packet */
  GstRTCPType  type;         /* type of current packet */
  guint16      length;       /* length of current packet in 32-bits words */

  guint        item_offset;  /* current item offset for navigating SDES */
  guint        item_count;   /* current item count */
  guint        entry_offset; /* current entry offset for navigating SDES items */
};

/* creating buffers */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstBuffer*      gst_rtcp_buffer_new_take_data     (gpointer data, guint len);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstBuffer*      gst_rtcp_buffer_new_copy_data     (gpointer data, guint len);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean        gst_rtcp_buffer_validate_data     (guint8 *data, guint len);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_buffer_validate          (GstBuffer *buffer);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstBuffer*      gst_rtcp_buffer_new               (guint mtu);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void            gst_rtcp_buffer_end               (GstBuffer *buffer);

/* adding/retrieving packets */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint           gst_rtcp_buffer_get_packet_count  (GstBuffer *buffer);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_buffer_get_first_packet  (GstBuffer *buffer, GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_move_to_next      (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean        gst_rtcp_buffer_add_packet        (GstBuffer *buffer, GstRTCPType type,
		                                   GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void            gst_rtcp_packet_remove            (GstRTCPPacket *packet);

/* working with packets */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_get_padding       (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint8          gst_rtcp_packet_get_count         (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstRTCPType     gst_rtcp_packet_get_type          (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint16         gst_rtcp_packet_get_length        (GstRTCPPacket *packet);


/* sender reports */
#ifdef __SYMBIAN32__
IMPORT_C
#endif
 
void            gst_rtcp_packet_sr_get_sender_info    (GstRTCPPacket *packet, guint32 *ssrc, 
                                                       guint64 *ntptime, guint32 *rtptime, 
						       guint32 *packet_count, guint32 *octet_count);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void            gst_rtcp_packet_sr_set_sender_info    (GstRTCPPacket *packet, guint32 ssrc, 
                                                       guint64 ntptime, guint32 rtptime, 
						       guint32 packet_count, guint32 octet_count);
/* receiver reports */
#ifdef __SYMBIAN32__
IMPORT_C
#endif
 
guint32         gst_rtcp_packet_rr_get_ssrc           (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void            gst_rtcp_packet_rr_set_ssrc           (GstRTCPPacket *packet, guint32 ssrc);


/* report blocks for SR and RR */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint           gst_rtcp_packet_get_rb_count          (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void            gst_rtcp_packet_get_rb                (GstRTCPPacket *packet, guint nth, guint32 *ssrc,
		                                       guint8 *fractionlost, gint32 *packetslost,
						       guint32 *exthighestseq, guint32 *jitter,
						       guint32 *lsr, guint32 *dlsr);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_add_rb                (GstRTCPPacket *packet, guint32 ssrc,
		                                       guint8 fractionlost, gint32 packetslost,
						       guint32 exthighestseq, guint32 jitter,
						       guint32 lsr, guint32 dlsr);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void            gst_rtcp_packet_set_rb                (GstRTCPPacket *packet, guint nth, guint32 ssrc,
		                                       guint8 fractionlost, gint32 packetslost,
						       guint32 exthighestseq, guint32 jitter,
						       guint32 lsr, guint32 dlsr);

/* source description packet */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint           gst_rtcp_packet_sdes_get_item_count   (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_sdes_first_item       (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_sdes_next_item        (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint32         gst_rtcp_packet_sdes_get_ssrc         (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
 
gboolean        gst_rtcp_packet_sdes_first_entry      (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_sdes_next_entry       (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_sdes_get_entry        (GstRTCPPacket *packet, 
                                                       GstRTCPSDESType *type, guint8 *len,
						       guint8 **data);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_sdes_copy_entry       (GstRTCPPacket *packet, 
                                                       GstRTCPSDESType *type, guint8 *len,
						       guint8 **data);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


gboolean        gst_rtcp_packet_sdes_add_item         (GstRTCPPacket *packet, guint32 ssrc);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_sdes_add_entry        (GstRTCPPacket *packet, GstRTCPSDESType type, 
                                                       guint8 len, const guint8 *data);

/* bye packet */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint           gst_rtcp_packet_bye_get_ssrc_count    (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint32         gst_rtcp_packet_bye_get_nth_ssrc      (GstRTCPPacket *packet, guint nth);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_bye_add_ssrc          (GstRTCPPacket *packet, guint32 ssrc);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_bye_add_ssrcs         (GstRTCPPacket *packet, guint32 *ssrc, guint len);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint8          gst_rtcp_packet_bye_get_reason_len    (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gchar*          gst_rtcp_packet_bye_get_reason        (GstRTCPPacket *packet);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean        gst_rtcp_packet_bye_set_reason        (GstRTCPPacket *packet, const gchar *reason);

/* helper functions */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint64         gst_rtcp_ntp_to_unix                  (guint64 ntptime);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

guint64         gst_rtcp_unix_to_ntp                  (guint64 unixtime);

G_END_DECLS

#endif /* __GST_RTCPBUFFER_H__ */

