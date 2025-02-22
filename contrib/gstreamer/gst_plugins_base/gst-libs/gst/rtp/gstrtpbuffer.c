/* GStreamer
 * Copyright (C) <2005> Philippe Khalaf <burger@speedy.org>
 * Copyright (C) <2006> Wim Taymans <wim@fluendo.com>
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
 * SECTION:gstrtpbuffer
 * @short_description: Helper methods for dealing with RTP buffers
 * @see_also: #GstBaseRTPPayload, #GstBaseRTPDepayload, gstrtcpbuffer
 *
 * <refsect2>
 * <para>
 * The GstRTPBuffer helper functions makes it easy to parse and create regular 
 * #GstBuffer objects that contain RTP payloads. These buffers are typically of
 * 'application/x-rtp' #GstCaps.
 * </para>
 * </refsect2>
 *
 * Last reviewed on 2006-07-17 (0.10.10)
 */

#include "gstrtpbuffer.h"

#include <stdlib.h>
#include <string.h>

#define GST_RTP_HEADER_LEN 12

/* Note: we use bitfields here to make sure the compiler doesn't add padding
 * between fields on certain architectures; can't assume aligned access either
 */
typedef struct _GstRTPHeader
{
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
  unsigned int csrc_count:4;    /* CSRC count */
  unsigned int extension:1;     /* header extension flag */
  unsigned int padding:1;       /* padding flag */
  unsigned int version:2;       /* protocol version */
  unsigned int payload_type:7;  /* payload type */
  unsigned int marker:1;        /* marker bit */
#elif G_BYTE_ORDER == G_BIG_ENDIAN
  unsigned int version:2;       /* protocol version */
  unsigned int padding:1;       /* padding flag */
  unsigned int extension:1;     /* header extension flag */
  unsigned int csrc_count:4;    /* CSRC count */
  unsigned int marker:1;        /* marker bit */
  unsigned int payload_type:7;  /* payload type */
#else
#error "G_BYTE_ORDER should be big or little endian."
#endif
  unsigned int seq:16;          /* sequence number */
  unsigned int timestamp:32;    /* timestamp */
  unsigned int ssrc:32;         /* synchronization source */
  guint8 csrclist[4];           /* optional CSRC list, 32 bits each */
} GstRTPHeader;

#define GST_RTP_HEADER_VERSION(buf)     (((GstRTPHeader *)(GST_BUFFER_DATA (buf)))->version)
#define GST_RTP_HEADER_PADDING(buf)     (((GstRTPHeader *)(GST_BUFFER_DATA (buf)))->padding)
#define GST_RTP_HEADER_EXTENSION(buf)   (((GstRTPHeader *)(GST_BUFFER_DATA (buf)))->extension)
#define GST_RTP_HEADER_CSRC_COUNT(buf)  (((GstRTPHeader *)(GST_BUFFER_DATA (buf)))->csrc_count)
#define GST_RTP_HEADER_MARKER(buf)      (((GstRTPHeader *)(GST_BUFFER_DATA (buf)))->marker)
#define GST_RTP_HEADER_PAYLOAD_TYPE(buf)(((GstRTPHeader *)(GST_BUFFER_DATA (buf)))->payload_type)
#define GST_RTP_HEADER_SEQ(buf)         (((GstRTPHeader *)(GST_BUFFER_DATA (buf)))->seq)
#define GST_RTP_HEADER_TIMESTAMP(buf)   (((GstRTPHeader *)(GST_BUFFER_DATA (buf)))->timestamp)
#define GST_RTP_HEADER_SSRC(buf)        (((GstRTPHeader *)(GST_BUFFER_DATA (buf)))->ssrc)
#define GST_RTP_HEADER_CSRC_LIST_OFFSET(buf,i)  \
    GST_BUFFER_DATA (buf) +                     \
    G_STRUCT_OFFSET(GstRTPHeader, csrclist) +   \
    ((i) * sizeof(guint32))
#define GST_RTP_HEADER_CSRC_SIZE(buf)   (GST_RTP_HEADER_CSRC_COUNT(buf) * sizeof (guint32))

/**
 * gst_rtp_buffer_allocate_data:
 * @buffer: a #GstBuffer
 * @payload_len: the length of the payload
 * @pad_len: the amount of padding
 * @csrc_count: the number of CSRC entries
 *
 * Allocate enough data in @buffer to hold an RTP packet with @csrc_count CSRCs,
 * a payload length of @payload_len and padding of @pad_len.
 * MALLOCDATA of @buffer will be overwritten and will not be freed. 
 * All other RTP header fields will be set to 0/FALSE.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_allocate_data (GstBuffer * buffer, guint payload_len,
    guint8 pad_len, guint8 csrc_count)
{
  guint len;

  g_return_if_fail (csrc_count <= 15);
  g_return_if_fail (GST_IS_BUFFER (buffer));

  len = GST_RTP_HEADER_LEN + csrc_count * sizeof (guint32)
      + payload_len + pad_len;

  GST_BUFFER_MALLOCDATA (buffer) = g_malloc (len);
  GST_BUFFER_DATA (buffer) = GST_BUFFER_MALLOCDATA (buffer);
  GST_BUFFER_SIZE (buffer) = len;

  /* fill in defaults */
  GST_RTP_HEADER_VERSION (buffer) = GST_RTP_VERSION;
  GST_RTP_HEADER_PADDING (buffer) = FALSE;
  GST_RTP_HEADER_EXTENSION (buffer) = FALSE;
  GST_RTP_HEADER_CSRC_COUNT (buffer) = csrc_count;
  memset (GST_RTP_HEADER_CSRC_LIST_OFFSET (buffer, 0), 0,
      csrc_count * sizeof (guint32));
  GST_RTP_HEADER_MARKER (buffer) = FALSE;
  GST_RTP_HEADER_PAYLOAD_TYPE (buffer) = 0;
  GST_RTP_HEADER_SEQ (buffer) = 0;
  GST_RTP_HEADER_TIMESTAMP (buffer) = 0;
  GST_RTP_HEADER_SSRC (buffer) = 0;
}

/**
 * gst_rtp_buffer_new_take_data:
 * @data: data for the new buffer
 * @len: the length of data
 *
 * Create a new buffer and set the data and size of the buffer to @data and @len
 * respectively. @data will be freed when the buffer is unreffed, so this
 * function transfers ownership of @data to the new buffer.
 *
 * Returns: A newly allocated buffer with @data and of size @len.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstBuffer *
gst_rtp_buffer_new_take_data (gpointer data, guint len)
{
  GstBuffer *result;

  g_return_val_if_fail (data != NULL, NULL);
  g_return_val_if_fail (len > 0, NULL);

  result = gst_buffer_new ();

  GST_BUFFER_MALLOCDATA (result) = data;
  GST_BUFFER_DATA (result) = data;
  GST_BUFFER_SIZE (result) = len;

  return result;
}

/**
 * gst_rtp_buffer_new_copy_data:
 * @data: data for the new buffer
 * @len: the length of data
 *
 * Create a new buffer and set the data to a copy of @len
 * bytes of @data and the size to @len. The data will be freed when the buffer
 * is freed.
 *
 * Returns: A newly allocated buffer with a copy of @data and of size @len.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstBuffer *
gst_rtp_buffer_new_copy_data (gpointer data, guint len)
{
  return gst_rtp_buffer_new_take_data (g_memdup (data, len), len);
}

/**
 * gst_rtp_buffer_new_allocate:
 * @payload_len: the length of the payload
 * @pad_len: the amount of padding
 * @csrc_count: the number of CSRC entries
 *
 * Allocate a new #Gstbuffer with enough data to hold an RTP packet with @csrc_count CSRCs,
 * a payload length of @payload_len and padding of @pad_len.
 * All other RTP header fields will be set to 0/FALSE.
 *
 * Returns: A newly allocated buffer that can hold an RTP packet with given
 * parameters.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstBuffer *
gst_rtp_buffer_new_allocate (guint payload_len, guint8 pad_len,
    guint8 csrc_count)
{
  GstBuffer *result;

  g_return_val_if_fail (csrc_count <= 15, NULL);

  result = gst_buffer_new ();
  gst_rtp_buffer_allocate_data (result, payload_len, pad_len, csrc_count);

  return result;
}

/**
 * gst_rtp_buffer_new_allocate_len:
 * @packet_len: the total length of the packet
 * @pad_len: the amount of padding
 * @csrc_count: the number of CSRC entries
 *
 * Create a new #GstBuffer that can hold an RTP packet that is exactly
 * @packet_len long. The length of the payload depends on @pad_len and
 * @csrc_count and can be calculated with gst_rtp_buffer_calc_payload_len().
 * All RTP header fields will be set to 0/FALSE.
 *
 * Returns: A newly allocated buffer that can hold an RTP packet of @packet_len.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstBuffer *
gst_rtp_buffer_new_allocate_len (guint packet_len, guint8 pad_len,
    guint8 csrc_count)
{
  guint len;

  g_return_val_if_fail (csrc_count <= 15, NULL);

  len = gst_rtp_buffer_calc_payload_len (packet_len, pad_len, csrc_count);

  return gst_rtp_buffer_new_allocate (len, pad_len, csrc_count);
}

/**
 * gst_rtp_buffer_calc_header_len:
 * @csrc_count: the number of CSRC entries
 *
 * Calculate the header length of an RTP packet with @csrc_count CSRC entries.
 * An RTP packet can have at most 15 CSRC entries.
 *
 * Returns: The length of an RTP header with @csrc_count CSRC entries.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint
gst_rtp_buffer_calc_header_len (guint8 csrc_count)
{
  g_return_val_if_fail (csrc_count <= 15, 0);

  return GST_RTP_HEADER_LEN + (csrc_count * sizeof (guint32));
}

/**
 * gst_rtp_buffer_calc_packet_len:
 * @payload_len: the length of the payload
 * @pad_len: the amount of padding
 * @csrc_count: the number of CSRC entries
 *
 * Calculate the total length of an RTP packet with a payload size of @payload_len,
 * a padding of @pad_len and a @csrc_count CSRC entries.
 *
 * Returns: The total length of an RTP header with given parameters.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint
gst_rtp_buffer_calc_packet_len (guint payload_len, guint8 pad_len,
    guint8 csrc_count)
{
  g_return_val_if_fail (csrc_count <= 15, 0);

  return payload_len + GST_RTP_HEADER_LEN + (csrc_count * sizeof (guint32))
      + pad_len;
}

/**
 * gst_rtp_buffer_calc_payload_len:
 * @packet_len: the length of the total RTP packet
 * @pad_len: the amount of padding
 * @csrc_count: the number of CSRC entries
 *
 * Calculate the length of the payload of an RTP packet with size @packet_len,
 * a padding of @pad_len and a @csrc_count CSRC entries.
 *
 * Returns: The length of the payload of an RTP packet  with given parameters.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint
gst_rtp_buffer_calc_payload_len (guint packet_len, guint8 pad_len,
    guint8 csrc_count)
{
  g_return_val_if_fail (csrc_count <= 15, 0);

  return packet_len - GST_RTP_HEADER_LEN - (csrc_count * sizeof (guint32))
      - pad_len;
}

/**
 * gst_rtp_buffer_validate_data:
 * @data: the data to validate
 * @len: the length of @data to validate
 *
 * Check if the @data and @size point to the data of a valid RTP packet.
 * This function checks the length, version and padding of the packet data.
 * Use this function to validate a packet before using the other functions in
 * this module.
 *
 * Returns: TRUE if the data points to a valid RTP packet.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_rtp_buffer_validate_data (guint8 * data, guint len)
{
  guint8 padding;
  guint8 csrc_count;
  guint header_len;
  guint8 version;

  g_return_val_if_fail (data != NULL, FALSE);

  header_len = GST_RTP_HEADER_LEN;
  if (G_UNLIKELY (len < header_len))
    goto wrong_length;

  /* check version */
  version = (data[0] & 0xc0) >> 6;
  if (G_UNLIKELY (version != GST_RTP_VERSION))
    goto wrong_version;

  /* calc header length with csrc */
  csrc_count = (data[0] & 0x0f);
  header_len += csrc_count * sizeof (guint32);

  /* calc extension length when present. */
  if (data[0] & 0x10) {
    guint8 *extpos;
    guint16 extlen;

    /* this points to the extenstion bits and header length */
    extpos = &data[header_len];

    /* skip the header and check that we have enough space */
    header_len += 4;
    if (G_UNLIKELY (len < header_len))
      goto wrong_length;

    /* skip id */
    extpos += 2;
    /* read length as the number of 32 bits words */
    extlen = GST_READ_UINT16_BE (extpos);

    header_len += extlen * sizeof (guint32);
  }

  /* check for padding */
  if (data[0] & 0x20)
    padding = data[len - 1];
  else
    padding = 0;

  /* check if padding not bigger than packet and header */
  if (G_UNLIKELY (len - header_len < padding))
    goto wrong_padding;

  return TRUE;

  /* ERRORS */
wrong_length:
  {
    GST_DEBUG ("len < header_len check failed (%d < %d)", len, header_len);
    return FALSE;
  }
wrong_version:
  {
    GST_DEBUG ("version check failed (%d != %d)", version, GST_RTP_VERSION);
    return FALSE;
  }
wrong_padding:
  {
    GST_DEBUG ("padding check failed (%d - %d < %d)", len, header_len, padding);
    return FALSE;
  }
}

/**
 * gst_rtp_buffer_validate:
 * @buffer: the buffer to validate
 *
 * Check if the data pointed to by @buffer is a valid RTP packet using
 * gst_rtp_buffer_validate_data().
 *
 * Returns: TRUE if @buffer is a valid RTP packet.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_rtp_buffer_validate (GstBuffer * buffer)
{
  guint8 *data;
  guint len;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);

  data = GST_BUFFER_DATA (buffer);
  len = GST_BUFFER_SIZE (buffer);

  return gst_rtp_buffer_validate_data (data, len);
}

/**
 * gst_rtp_buffer_set_packet_len:
 * @buffer: the buffer
 * @len: the new packet length
 *
 * Set the total @buffer size to @len. The data in the buffer will be made
 * larger if needed. Any padding will be removed from the packet. 
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_set_packet_len (GstBuffer * buffer, guint len)
{
  guint oldlen;

  g_return_if_fail (GST_IS_BUFFER (buffer));

  oldlen = GST_BUFFER_SIZE (buffer);

  if (oldlen < len) {
    guint8 *newdata;

    newdata = g_realloc (GST_BUFFER_MALLOCDATA (buffer), len);
    GST_BUFFER_MALLOCDATA (buffer) = newdata;
    GST_BUFFER_DATA (buffer) = newdata;
  }
  GST_BUFFER_SIZE (buffer) = len;

  /* remove any padding */
  GST_RTP_HEADER_PADDING (buffer) = FALSE;
}

/**
 * gst_rtp_buffer_get_packet_len:
 * @buffer: the buffer
 *
 * Return the total length of the packet in @buffer.
 *
 * Returns: The total length of the packet in @buffer.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint
gst_rtp_buffer_get_packet_len (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);

  return GST_BUFFER_SIZE (buffer);
}

/**
 * gst_rtp_buffer_get_header_len:
 * @buffer: the buffer
 *
 * Return the total length of the header in @buffer. This include the length of
 * the fixed header, the CSRC list and the extension header.
 *
 * Returns: The total length of the header in @buffer.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint
gst_rtp_buffer_get_header_len (GstBuffer * buffer)
{
  guint len;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);

  len = GST_RTP_HEADER_LEN + GST_RTP_HEADER_CSRC_SIZE (buffer);
  if (GST_RTP_HEADER_EXTENSION (buffer))
    len += GST_READ_UINT16_BE (GST_BUFFER_DATA (buffer) + len + 2) * 4 + 4;

  return len;
}

/**
 * gst_rtp_buffer_get_version:
 * @buffer: the buffer
 *
 * Get the version number of the RTP packet in @buffer.
 *
 * Returns: The version of @buffer.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint8
gst_rtp_buffer_get_version (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, 0);

  return GST_RTP_HEADER_VERSION (buffer);
}

/**
 * gst_rtp_buffer_set_version:
 * @buffer: the buffer
 * @version: the new version
 *
 * Set the version of the RTP packet in @buffer to @version.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_set_version (GstBuffer * buffer, guint8 version)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (version < 0x04);
  g_return_if_fail (GST_BUFFER_DATA (buffer) != NULL);

  GST_RTP_HEADER_VERSION (buffer) = version;
}

/**
 * gst_rtp_buffer_get_padding:
 * @buffer: the buffer
 *
 * Check if the padding bit is set on the RTP packet in @buffer.
 *
 * Returns: TRUE if @buffer has the padding bit set.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_rtp_buffer_get_padding (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, FALSE);

  return GST_RTP_HEADER_PADDING (buffer);
}

/**
 * gst_rtp_buffer_set_padding:
 * @buffer: the buffer
 * @padding: the new padding
 *
 * Set the padding bit on the RTP packet in @buffer to @padding.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_set_padding (GstBuffer * buffer, gboolean padding)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (GST_BUFFER_DATA (buffer) != NULL);

  GST_RTP_HEADER_PADDING (buffer) = padding;
}

/**
 * gst_rtp_buffer_pad_to:
 * @buffer: the buffer
 * @len: the new amount of padding
 *
 * Set the amount of padding in the RTP packet in @buffer to
 * @len. If @len is 0, the padding is removed.
 *
 * NOTE: This function does not work correctly.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_pad_to (GstBuffer * buffer, guint len)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (GST_BUFFER_DATA (buffer) != NULL);

  if (len > 0)
    GST_RTP_HEADER_PADDING (buffer) = TRUE;
  else
    GST_RTP_HEADER_PADDING (buffer) = FALSE;

  /* FIXME, set the padding byte at the end of the payload data */
}

/**
 * gst_rtp_buffer_get_extension:
 * @buffer: the buffer
 *
 * Check if the extension bit is set on the RTP packet in @buffer.
 * 
 * Returns: TRUE if @buffer has the extension bit set.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_rtp_buffer_get_extension (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, FALSE);

  return GST_RTP_HEADER_EXTENSION (buffer);
}

/**
 * gst_rtp_buffer_set_extension:
 * @buffer: the buffer
 * @extension: the new extension
 *
 * Set the extension bit on the RTP packet in @buffer to @extension.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_set_extension (GstBuffer * buffer, gboolean extension)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (GST_BUFFER_DATA (buffer) != NULL);

  GST_RTP_HEADER_EXTENSION (buffer) = extension;
}

/**
 * gst_rtp_buffer_get_extension_data:
 * @buffer: the buffer
 * @bits: location for result bits
 * @data: location for data
 * @wordlen: location for length of @data in 32 bits words
 *
 * Get the extension data. @bits will contain the extension 16 bits of custom
 * data. @data will point to the data in the extension and @wordlen will contain
 * the length of @data in 32 bits words.
 *
 * If @buffer did not contain an extension, this function will return %FALSE
 * with @bits, @data and @wordlen unchanged.
 * 
 * Returns: TRUE if @buffer had the extension bit set.
 *
 * Since: 0.10.15
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_rtp_buffer_get_extension_data (GstBuffer * buffer, guint16 * bits,
    gpointer * data, guint * wordlen)
{
  guint len;
  guint8 *pdata;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, FALSE);

  if (!GST_RTP_HEADER_EXTENSION (buffer))
    return FALSE;

  /* move to the extension */
  len = GST_RTP_HEADER_LEN + GST_RTP_HEADER_CSRC_SIZE (buffer);
  pdata = GST_BUFFER_DATA (buffer) + len;

  if (bits)
    *bits = GST_READ_UINT16_BE (pdata);
  if (wordlen)
    *wordlen = GST_READ_UINT16_BE (pdata + 2);
  if (data)
    *data = pdata + 4;

  return TRUE;
}

/**
 * gst_rtp_buffer_set_extension_data:
 * @buffer: the buffer
 * @bits: the bits specific for the extension
 * @length: the length that counts the number of 32-bit words in
 * the extension, excluding the extension header ( therefore zero is a valid length)
 *
 * Set the extension bit of the rtp buffer and fill in the @bits and @length of the
 * extension header. It will refuse to set the extension data if the buffer is not
 * large enough.
 *
 * Returns: True if done.
 *
 * Since : 0.10.18
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_rtp_buffer_set_extension_data (GstBuffer * buffer, guint16 bits,
    guint16 length)
{
  guint32 min_size = 0;
  guint8 *data;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, FALSE);

  gst_rtp_buffer_set_extension (buffer, TRUE);
  min_size =
      GST_RTP_HEADER_LEN + GST_RTP_HEADER_CSRC_SIZE (buffer) + 4 +
      length * sizeof (guint32);

  if (min_size > GST_BUFFER_SIZE (buffer)) {
    GST_WARNING_OBJECT (buffer,
        "rtp buffer too small: need more than %d bytes but only have %d bytes",
        min_size, GST_BUFFER_SIZE (buffer));
    return FALSE;
  }

  data =
      GST_BUFFER_DATA (buffer) + GST_RTP_HEADER_LEN +
      GST_RTP_HEADER_CSRC_SIZE (buffer);
  GST_WRITE_UINT16_BE (data, bits);
  GST_WRITE_UINT16_BE (data + 2, length);
  return TRUE;
}

/**
 * gst_rtp_buffer_get_ssrc:
 * @buffer: the buffer
 *
 * Get the SSRC of the RTP packet in @buffer.
 * 
 * Returns: the SSRC of @buffer in host order.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint32
gst_rtp_buffer_get_ssrc (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, 0);

  return g_ntohl (GST_RTP_HEADER_SSRC (buffer));
}

/**
 * gst_rtp_buffer_set_ssrc:
 * @buffer: the buffer
 * @ssrc: the new SSRC
 *
 * Set the SSRC on the RTP packet in @buffer to @ssrc.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_set_ssrc (GstBuffer * buffer, guint32 ssrc)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (GST_BUFFER_DATA (buffer) != NULL);

  GST_RTP_HEADER_SSRC (buffer) = g_htonl (ssrc);
}

/**
 * gst_rtp_buffer_get_csrc_count:
 * @buffer: the buffer
 *
 * Get the CSRC count of the RTP packet in @buffer.
 * 
 * Returns: the CSRC count of @buffer.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint8
gst_rtp_buffer_get_csrc_count (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, 0);

  return GST_RTP_HEADER_CSRC_COUNT (buffer);
}

/**
 * gst_rtp_buffer_get_csrc:
 * @buffer: the buffer
 * @idx: the index of the CSRC to get
 *
 * Get the CSRC at index @idx in @buffer.
 * 
 * Returns: the CSRC at index @idx in host order.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint32
gst_rtp_buffer_get_csrc (GstBuffer * buffer, guint8 idx)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, 0);
  g_return_val_if_fail (idx < GST_RTP_HEADER_CSRC_COUNT (buffer), 0);

  return GST_READ_UINT32_BE (GST_RTP_HEADER_CSRC_LIST_OFFSET (buffer, idx));
}

/**
 * gst_rtp_buffer_set_csrc:
 * @buffer: the buffer
 * @idx: the CSRC index to set
 * @csrc: the CSRC in host order to set at @idx
 *
 * Modify the CSRC at index @idx in @buffer to @csrc.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_set_csrc (GstBuffer * buffer, guint8 idx, guint32 csrc)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (GST_BUFFER_DATA (buffer) != NULL);
  g_return_if_fail (idx < GST_RTP_HEADER_CSRC_COUNT (buffer));

  GST_WRITE_UINT32_BE (GST_RTP_HEADER_CSRC_LIST_OFFSET (buffer, idx), csrc);
}

/**
 * gst_rtp_buffer_get_marker:
 * @buffer: the buffer
 *
 * Check if the marker bit is set on the RTP packet in @buffer.
 *
 * Returns: TRUE if @buffer has the marker bit set.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_rtp_buffer_get_marker (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, FALSE);

  return GST_RTP_HEADER_MARKER (buffer);
}

/**
 * gst_rtp_buffer_set_marker:
 * @buffer: the buffer
 * @marker: the new marker
 *
 * Set the marker bit on the RTP packet in @buffer to @marker.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_set_marker (GstBuffer * buffer, gboolean marker)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (GST_BUFFER_DATA (buffer) != NULL);

  GST_RTP_HEADER_MARKER (buffer) = marker;
}

/**
 * gst_rtp_buffer_get_payload_type:
 * @buffer: the buffer
 *
 * Get the payload type of the RTP packet in @buffer.
 *
 * Returns: The payload type.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint8
gst_rtp_buffer_get_payload_type (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, 0);

  return GST_RTP_HEADER_PAYLOAD_TYPE (buffer);
}

/**
 * gst_rtp_buffer_set_payload_type:
 * @buffer: the buffer
 * @payload_type: the new type
 *
 * Set the payload type of the RTP packet in @buffer to @payload_type.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_set_payload_type (GstBuffer * buffer, guint8 payload_type)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (GST_BUFFER_DATA (buffer) != NULL);
  g_return_if_fail (payload_type < 0x80);

  GST_RTP_HEADER_PAYLOAD_TYPE (buffer) = payload_type;
}

/**
 * gst_rtp_buffer_get_seq:
 * @buffer: the buffer
 *
 * Get the sequence number of the RTP packet in @buffer.
 *
 * Returns: The sequence number in host order.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint16
gst_rtp_buffer_get_seq (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, 0);

  return g_ntohs (GST_RTP_HEADER_SEQ (buffer));
}

/**
 * gst_rtp_buffer_set_seq:
 * @buffer: the buffer
 * @seq: the new sequence number
 *
 * Set the sequence number of the RTP packet in @buffer to @seq.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_set_seq (GstBuffer * buffer, guint16 seq)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (GST_BUFFER_DATA (buffer) != NULL);

  GST_RTP_HEADER_SEQ (buffer) = g_htons (seq);
}

/**
 * gst_rtp_buffer_get_timestamp:
 * @buffer: the buffer
 *
 * Get the timestamp of the RTP packet in @buffer.
 *
 * Returns: The timestamp in host order.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint32
gst_rtp_buffer_get_timestamp (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, 0);

  return g_ntohl (GST_RTP_HEADER_TIMESTAMP (buffer));
}

/**
 * gst_rtp_buffer_set_timestamp:
 * @buffer: the buffer
 * @timestamp: the new timestamp
 *
 * Set the timestamp of the RTP packet in @buffer to @timestamp.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_rtp_buffer_set_timestamp (GstBuffer * buffer, guint32 timestamp)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (GST_BUFFER_DATA (buffer) != NULL);

  GST_RTP_HEADER_TIMESTAMP (buffer) = g_htonl (timestamp);
}

/**
 * gst_rtp_buffer_get_payload_subbuffer:
 * @buffer: the buffer
 * @offset: the offset in the payload
 * @len: the length in the payload
 *
 * Create a subbuffer of the payload of the RTP packet in @buffer. @offset bytes
 * are skipped in the payload and the subbuffer will be of size @len.
 * If @len is -1 the total payload starting from @offset if subbuffered.
 *
 * Returns: A new buffer with the specified data of the payload.
 *
 * Since: 0.10.10
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstBuffer *
gst_rtp_buffer_get_payload_subbuffer (GstBuffer * buffer, guint offset,
    guint len)
{
  guint poffset, plen;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, NULL);

  plen = gst_rtp_buffer_get_payload_len (buffer);
  /* we can't go past the length */
  if (G_UNLIKELY (offset >= plen)) {
    GST_WARNING ("offset=%u should be less then plen=%u", offset, plen);
    return (NULL);
  }

  /* apply offset */
  poffset = gst_rtp_buffer_get_header_len (buffer) + offset;
  plen -= offset;

  /* see if we need to shrink the buffer based on @len */
  if (len != -1 && len < plen)
    plen = len;

  return gst_buffer_create_sub (buffer, poffset, plen);
}

/**
 * gst_rtp_buffer_get_payload_buffer:
 * @buffer: the buffer
 *
 * Create a buffer of the payload of the RTP packet in @buffer. This function
 * will internally create a subbuffer of @buffer so that a memcpy can be
 * avoided.
 *
 * Returns: A new buffer with the data of the payload.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstBuffer *
gst_rtp_buffer_get_payload_buffer (GstBuffer * buffer)
{
  return gst_rtp_buffer_get_payload_subbuffer (buffer, 0, -1);
}

/**
 * gst_rtp_buffer_get_payload_len:
 * @buffer: the buffer
 *
 * Get the length of the payload of the RTP packet in @buffer.
 *
 * Returns: The length of the payload in @buffer.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint
gst_rtp_buffer_get_payload_len (GstBuffer * buffer)
{
  guint len, size;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, 0);

  size = GST_BUFFER_SIZE (buffer);

  len = size - gst_rtp_buffer_get_header_len (buffer);

  if (GST_RTP_HEADER_PADDING (buffer))
    len -= GST_BUFFER_DATA (buffer)[size - 1];

  return len;
}

/**
 * gst_rtp_buffer_get_payload:
 * @buffer: the buffer
 *
 * Get a pointer to the payload data in @buffer. This pointer is valid as long
 * as a reference to @buffer is held.
 *
 * Returns: A pointer to the payload data in @buffer.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gpointer
gst_rtp_buffer_get_payload (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);
  g_return_val_if_fail (GST_BUFFER_DATA (buffer) != NULL, NULL);

  return GST_BUFFER_DATA (buffer) + gst_rtp_buffer_get_header_len (buffer);
}

/**
 * gst_rtp_buffer_default_clock_rate:
 * @payload_type: the static payload type
 *
 * Get the default clock-rate for the static payload type @payload_type.
 *
 * Returns: the default clock rate or -1 if the payload type is not static or
 * the clock-rate is undefined.
 *
 * Since: 0.10.13
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint32
gst_rtp_buffer_default_clock_rate (guint8 payload_type)
{
  const GstRTPPayloadInfo *info;
  guint32 res;

  info = gst_rtp_payload_info_for_pt (payload_type);
  if (!info)
    return -1;

  res = info->clock_rate;
  /* 0 means unknown so we have to return -1 from this function */
  if (res == 0)
    res = -1;

  return res;
}

/**
 * gst_rtp_buffer_compare_seqnum:
 * @seqnum1: a sequence number
 * @seqnum2: a sequence number
 *
 * Compare two sequence numbers, taking care of wraparounds.
 *
 * Returns: -1 if @seqnum1 is before @seqnum2, 0 if they are equal or 1 if
 * @seqnum1 is bigger than @segnum2.
 *
 * Since: 0.10.15
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gint
gst_rtp_buffer_compare_seqnum (guint16 seqnum1, guint16 seqnum2)
{
  /* check if diff more than half of the 16bit range */
  if (abs (seqnum2 - seqnum1) > (1 << 15)) {
    /* one of a/b has wrapped */
    return seqnum1 - seqnum2;
  } else {
    return seqnum2 - seqnum1;
  }
}

/**
 * gst_rtp_buffer_ext_timestamp:
 * @exttimestamp: a previous extended timestamp
 * @timestamp: a new timestamp
 *
 * Update the @exttimestamp field with @timestamp. For the first call of the
 * method, @exttimestamp should point to a location with a value of -1.
 *
 * This function makes sure that the returned value is a constantly increasing
 * value even in the case where there is a timestamp wraparound.
 *
 * Returns: The extended timestamp of @timestamp.
 *
 * Since: 0.10.15
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint64
gst_rtp_buffer_ext_timestamp (guint64 * exttimestamp, guint32 timestamp)
{
  guint64 result, diff, ext;

  g_return_val_if_fail (exttimestamp != NULL, -1);

  ext = *exttimestamp;

  if (ext == -1) {
    result = timestamp;
  } else {
    /* pick wraparound counter from previous timestamp and add to new timestamp */
    result = timestamp + (ext & ~(G_GINT64_CONSTANT (0xffffffff)));

    /* check for timestamp wraparound */
    if (result < ext)
      diff = ext - result;
    else
      diff = result - ext;

    if (diff > G_MAXINT32) {
      /* timestamp went backwards more than allowed, we wrap around and get
       * updated extended timestamp. */
      result += (G_GINT64_CONSTANT (1) << 32);
    }
  }
  *exttimestamp = result;

  return result;
}
