/* GStreamer
 * Copyright (C) 2003 Benjamin Otte <in7y118@public.uni-hamburg.de>
 *
 * gstvorbistagsetter.c: plugin for reading / modifying vorbis tags
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
 * SECTION:gsttagvorbis
 * @short_description: tag mappings and support functions for plugins
 *                     dealing with vorbiscomments
 * @see_also: #GstTagList
 *
 * <refsect2>
 * <para>
 * Contains various utility functions for plugins to parse or create
 * vorbiscomments and map them to and from #GstTagList<!-- -->s.
 * </para>
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gst/gsttagsetter.h>
#include "gsttageditingprivate.h"
#include <stdlib.h>
#include <string.h>

#ifdef __SYMBIAN32__
#include <glib_global.h>
#endif

/*
 * see http://xiph.org/ogg/vorbis/doc/v-comment.html
 */
static const GstTagEntryMatch tag_matches[] = {
  {GST_TAG_TITLE, "TITLE"},
  {GST_TAG_VERSION, "VERSION"},
  {GST_TAG_ALBUM, "ALBUM"},
  {GST_TAG_TRACK_NUMBER, "TRACKNUMBER"},
  {GST_TAG_ALBUM_VOLUME_NUMBER, "DISCNUMBER"},
  {GST_TAG_TRACK_COUNT, "TRACKTOTAL"},
  {GST_TAG_ALBUM_VOLUME_COUNT, "DISCTOTAL"},
  {GST_TAG_ARTIST, "ARTIST"},
  {GST_TAG_PERFORMER, "PERFORMER"},
  {GST_TAG_COMPOSER, "COMPOSER"},
  {GST_TAG_COPYRIGHT, "COPYRIGHT"},
  {GST_TAG_LICENSE, "LICENSE"},
  {GST_TAG_LICENSE_URI, "LICENSE"},
  {GST_TAG_LOCATION, "LOCATION"},
  {GST_TAG_ORGANIZATION, "ORGANIZATION"},
  {GST_TAG_DESCRIPTION, "DESCRIPTION"},
  {GST_TAG_GENRE, "GENRE"},
  {GST_TAG_DATE, "DATE"},
  {GST_TAG_CONTACT, "CONTACT"},
  {GST_TAG_ISRC, "ISRC"},
  {GST_TAG_COMMENT, "COMMENT"},
  {GST_TAG_TRACK_GAIN, "REPLAYGAIN_TRACK_GAIN"},
  {GST_TAG_TRACK_PEAK, "REPLAYGAIN_TRACK_PEAK"},
  {GST_TAG_ALBUM_GAIN, "REPLAYGAIN_ALBUM_GAIN"},
  {GST_TAG_ALBUM_PEAK, "REPLAYGAIN_ALBUM_PEAK"},
  {GST_TAG_REFERENCE_LEVEL, "REPLAYGAIN_REFERENCE_LOUDNESS"},
  {GST_TAG_MUSICBRAINZ_TRACKID, "MUSICBRAINZ_TRACKID"},
  {GST_TAG_MUSICBRAINZ_ARTISTID, "MUSICBRAINZ_ARTISTID"},
  {GST_TAG_MUSICBRAINZ_ALBUMID, "MUSICBRAINZ_ALBUMID"},
  {GST_TAG_MUSICBRAINZ_ALBUMARTISTID, "MUSICBRAINZ_ALBUMARTISTID"},
  {GST_TAG_MUSICBRAINZ_TRMID, "MUSICBRAINZ_TRMID"},
  {GST_TAG_ARTIST_SORTNAME, "ARTISTSORT"},
  {GST_TAG_ARTIST_SORTNAME, "ARTISTSORTORDER"},
  {GST_TAG_ARTIST_SORTNAME, "MUSICBRAINZ_SORTNAME"},
  {GST_TAG_ALBUM_SORTNAME, "ALBUMSORT"},
  {GST_TAG_ALBUM_SORTNAME, "ALBUMSORTORDER"},
  {GST_TAG_TITLE_SORTNAME, "TITLESORT"},
  {GST_TAG_TITLE_SORTNAME, "TITLESORTORDER"},
  {GST_TAG_LANGUAGE_CODE, "LANGUAGE"},
  {GST_TAG_CDDA_MUSICBRAINZ_DISCID, "MUSICBRAINZ_DISCID"},
  {GST_TAG_CDDA_CDDB_DISCID, "DISCID"},
  /* some incidence that this makes sense:
   * http://mail.kde.org/pipermail/amarok/2006-May/000090.html
   */
  {GST_TAG_BEATS_PER_MINUTE, "BPM"},
  {NULL, NULL}
};

/**
 * gst_tag_from_vorbis_tag:
 * @vorbis_tag: vorbiscomment tag to convert to GStreamer tag
 *
 * Looks up the GStreamer tag for a vorbiscomment tag.
 *
 * Returns: The corresponding GStreamer tag or NULL if none exists.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

G_CONST_RETURN gchar *
gst_tag_from_vorbis_tag (const gchar * vorbis_tag)
{
  int i = 0;
  gchar *real_vorbis_tag;

  g_return_val_if_fail (vorbis_tag != NULL, NULL);

  gst_tag_register_musicbrainz_tags ();

  real_vorbis_tag = g_ascii_strup (vorbis_tag, -1);
  while (tag_matches[i].gstreamer_tag != NULL) {
    if (strcmp (real_vorbis_tag, tag_matches[i].original_tag) == 0) {
      break;
    }
    i++;
  }
  g_free (real_vorbis_tag);
  return tag_matches[i].gstreamer_tag;
}

/**
 * gst_tag_to_vorbis_tag:
 * @gst_tag: GStreamer tag to convert to vorbiscomment tag
 *
 * Looks up the vorbiscomment tag for a GStreamer tag.
 *
 * Returns: The corresponding vorbiscomment tag or NULL if none exists.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

G_CONST_RETURN gchar *
gst_tag_to_vorbis_tag (const gchar * gst_tag)
{
  int i = 0;

  g_return_val_if_fail (gst_tag != NULL, NULL);

  gst_tag_register_musicbrainz_tags ();

  while (tag_matches[i].gstreamer_tag != NULL) {
    if (strcmp (gst_tag, tag_matches[i].gstreamer_tag) == 0) {
      return tag_matches[i].original_tag;
    }
    i++;
  }
  return NULL;
}


/**
 * gst_vorbis_tag_add:
 * @list: a #GstTagList
 * @tag: a vorbiscomment tag string (key in key=value), must be valid UTF-8
 * @value: a vorbiscomment value string (value in key=value), must be valid UTF-8
 *
 * Convenience function using gst_tag_from_vorbis_tag(), parsing
 * a vorbis comment string into the right type and adding it to the
 * given taglist @list.
 *
 * Unknown vorbiscomment tags will be added to the tag list in form
 * of a #GST_TAG_EXTENDED_COMMENT (since 0.10.10 at least).
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_vorbis_tag_add (GstTagList * list, const gchar * tag, const gchar * value)
{
  const gchar *gst_tag;
  GType tag_type;

  g_return_if_fail (list != NULL);
  g_return_if_fail (tag != NULL);
  g_return_if_fail (value != NULL);

  g_return_if_fail (g_utf8_validate (tag, -1, NULL));
  g_return_if_fail (g_utf8_validate (value, -1, NULL));
  g_return_if_fail (strchr (tag, '=') == NULL);

  gst_tag = gst_tag_from_vorbis_tag (tag);
  if (gst_tag == NULL) {
    gchar *ext_comment;

    ext_comment = g_strdup_printf ("%s=%s", tag, value);
    gst_tag_list_add (list, GST_TAG_MERGE_APPEND, GST_TAG_EXTENDED_COMMENT,
        ext_comment, NULL);
    g_free (ext_comment);
    return;
  }

  tag_type = gst_tag_get_type (gst_tag);
  switch (tag_type) {
    case G_TYPE_UINT:{
      guint tmp;
      gchar *check;
      gboolean is_track_number_tag;
      gboolean is_disc_number_tag;

      is_track_number_tag = (strcmp (gst_tag, GST_TAG_TRACK_NUMBER) == 0);
      is_disc_number_tag = (strcmp (gst_tag, GST_TAG_ALBUM_VOLUME_NUMBER) == 0);
      tmp = strtoul (value, &check, 10);
      if (*check == '/' && (is_track_number_tag || is_disc_number_tag)) {
        guint count;

        check++;
        count = strtoul (check, &check, 10);
        if (*check != '\0' || count == 0)
          break;
        if (is_track_number_tag) {
          gst_tag_list_add (list, GST_TAG_MERGE_APPEND, GST_TAG_TRACK_COUNT,
              count, NULL);
        } else {
          gst_tag_list_add (list, GST_TAG_MERGE_APPEND,
              GST_TAG_ALBUM_VOLUME_COUNT, count, NULL);
        }
      }
      if (*check == '\0') {
        gst_tag_list_add (list, GST_TAG_MERGE_APPEND, gst_tag, tmp, NULL);
      }
      break;
    }
    case G_TYPE_STRING:{
      gchar *valid = NULL;

      /* specialcase for language code */
      if (strcmp (tag, "LANGUAGE") == 0) {
        const gchar *s = strchr (value, '[');

        /* Accept both ISO-639-1 and ISO-639-2 codes */
        if (s && strchr (s, ']') == s + 4) {
          valid = g_strndup (s + 1, 3);
        } else if (s && strchr (s, ']') == s + 3) {
          valid = g_strndup (s + 1, 2);
        } else if (strlen (value) != 2 && strlen (value) != 3) {
          GST_WARNING ("doesn't contain an ISO-639 language code: %s", value);
        }
      } else if (strcmp (tag, "LICENSE") == 0) {
        /* license tags in vorbis comments must contain an URI representing
         * the license and nothing more, at least according to:
         * http://wiki.xiph.org/index.php/LICENSE_and_COPYRIGHT_tags_on_Vorbis_Comments */
        if (value && gst_uri_is_valid (value))
          gst_tag = GST_TAG_LICENSE_URI;
      }

      if (!valid) {
        valid = g_strdup (value);
      }
      gst_tag_list_add (list, GST_TAG_MERGE_APPEND, gst_tag, valid, NULL);
      g_free (valid);
      break;
    }
    case G_TYPE_DOUBLE:{
      gchar *c;

      c = g_strdup (value);
      g_strdelimit (c, ",", '.');
      gst_tag_list_add (list, GST_TAG_MERGE_APPEND, gst_tag,
          g_strtod (c, NULL), NULL);
      g_free (c);
      break;
    }
    default:{
      if (tag_type == GST_TYPE_DATE) {
        guint y, d = 1, m = 1;
        gchar *check = (gchar *) value;

        y = strtoul (check, &check, 10);
        if (*check == '-') {
          check++;
          m = strtoul (check, &check, 10);
          if (*check == '-') {
            check++;
            d = strtoul (check, &check, 10);
          }
        }

        /* accept dates like 2007-00-00 and 2007-05-00 */
        if (y != 0) {
          if (m == 0 && d == 0)
            m = d = 1;
          else if (m != 0 && d == 0)
            d = 1;
        }

        /* date might be followed by a time */
        if ((*check == '\0' || g_ascii_isspace (*check)) && y != 0 &&
            g_date_valid_dmy (d, m, y)) {
          GDate *date;

          date = g_date_new_dmy (d, m, y);
          gst_tag_list_add (list, GST_TAG_MERGE_APPEND, gst_tag, date, NULL);
          g_date_free (date);
        } else {
          GST_DEBUG ("skipping invalid date '%s' (%u,%u,%u)", value, y, m, d);
        }
      } else {
        GST_WARNING ("Unhandled tag of type '%s' (%d)",
            g_type_name (tag_type), (gint) tag_type);
      }
      break;
    }
  }
}

/**
 * gst_tag_list_from_vorbiscomment_buffer:
 * @buffer: buffer to convert
 * @id_data: identification data at start of stream
 * @id_data_length: length of identification data
 * @vendor_string: pointer to a string that should take the vendor string
 *                 of this vorbis comment or NULL if you don't need it.
 *
 * Creates a new tag list that contains the information parsed out of a
 * vorbiscomment packet.
 *
 * Returns: A new #GstTagList with all tags that could be extracted from the
 *          given vorbiscomment buffer or NULL on error.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstTagList *
gst_tag_list_from_vorbiscomment_buffer (const GstBuffer * buffer,
    const guint8 * id_data, const guint id_data_length, gchar ** vendor_string)
{
#define ADVANCE(x) G_STMT_START{                                                \
  data += x;                                                                    \
  size -= x;                                                                    \
  if (size < 4) goto error;                                                     \
  cur_size = GST_READ_UINT32_LE (data);                                         \
  data += 4;                                                                    \
  size -= 4;                                                                    \
  if (cur_size > size) goto error;                                              \
  cur = (gchar*)data;                                                                   \
}G_STMT_END
  gchar *cur, *value;
  guint cur_size;
  guint iterations;
  guint8 *data;
  guint size;
  GstTagList *list;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);
  g_return_val_if_fail (id_data != NULL || id_data_length == 0, NULL);

  data = GST_BUFFER_DATA (buffer);
  size = GST_BUFFER_SIZE (buffer);
  list = gst_tag_list_new ();

  if (size < 11 || size <= id_data_length + 4)
    goto error;
  if (id_data_length > 0 && memcmp (data, id_data, id_data_length) != 0)
    goto error;
  ADVANCE (id_data_length);
  if (vendor_string)
    *vendor_string = g_strndup (cur, cur_size);
  ADVANCE (cur_size);
  iterations = cur_size;
  cur_size = 0;
  while (iterations) {
    ADVANCE (cur_size);
    iterations--;
    cur = g_strndup (cur, cur_size);
    value = strchr (cur, '=');
    if (value == NULL) {
      g_free (cur);
      continue;
    }
    *value = '\0';
    value++;
    if (!g_utf8_validate (value, -1, NULL)) {
      g_free (cur);
      continue;
    }
    gst_vorbis_tag_add (list, cur, value);
    g_free (cur);
  }

  return list;

error:
  gst_tag_list_free (list);
  return NULL;
#undef ADVANCE
}
typedef struct
{
  guint count;
  guint data_count;
  GList *entries;
}
MyForEach;

/**
 * gst_tag_to_vorbis_comments:
 * @list: a #GstTagList
 * @tag: a GStreamer tag identifier, such as #GST_TAG_ARTIST
 *
 * Creates a new tag list that contains the information parsed out of a
 * vorbiscomment packet.
 *
 * Returns: A #GList of newly-allowcated key=value strings. Free with
 *          g_list_foreach (list, (GFunc) g_free, NULL) plus g_list_free (list)
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GList *
gst_tag_to_vorbis_comments (const GstTagList * list, const gchar * tag)
{
  const gchar *vorbis_tag = NULL;
  GList *l = NULL;
  guint i;

  g_return_val_if_fail (list != NULL, NULL);
  g_return_val_if_fail (tag != NULL, NULL);

  if (strcmp (tag, GST_TAG_EXTENDED_COMMENT) != 0) {
    vorbis_tag = gst_tag_to_vorbis_tag (tag);
    if (!vorbis_tag)
      return NULL;
  }

  /* FIXME: for tags that can map to multiple vorbis comment keys, add all
   * of the possible keys */
  for (i = 0; i < gst_tag_list_get_tag_size (list, tag); i++) {
    GType tag_type = gst_tag_get_type (tag);
    gchar *result = NULL;

    switch (tag_type) {
      case G_TYPE_UINT:{
        guint u;

        if (!gst_tag_list_get_uint_index (list, tag, i, &u))
          g_return_val_if_reached (NULL);
        result = g_strdup_printf ("%s=%u", vorbis_tag, u);
        break;
      }
      case G_TYPE_STRING:{
        gchar *str = NULL;

        if (!gst_tag_list_get_string_index (list, tag, i, &str))
          g_return_val_if_reached (NULL);

        /* special case: GST_TAG_EXTENDED_COMMENT */
        if (vorbis_tag == NULL) {
          gchar *key = NULL, *val = NULL;

          if (gst_tag_parse_extended_comment (str, &key, NULL, &val, TRUE)) {
            result = g_strdup_printf ("%s=%s", key, val);
            g_free (key);
            g_free (val);
          } else {
            GST_WARNING ("Not a valid extended comment string: %s", str);
          }
        } else {
          result = g_strdup_printf ("%s=%s", vorbis_tag, str);
        }
        g_free (str);
        break;
      }
      case G_TYPE_DOUBLE:{
        gdouble value;
        gchar buf[G_ASCII_DTOSTR_BUF_SIZE];

        if (!gst_tag_list_get_double_index (list, tag, i, &value))
          g_return_val_if_reached (NULL);
        g_ascii_formatd (buf, G_ASCII_DTOSTR_BUF_SIZE, "%f", value);
        result = g_strconcat (vorbis_tag, "=", buf, NULL);
        break;
      }
      default:{
        if (tag_type == GST_TYPE_DATE) {
          GDate *date;

          if (gst_tag_list_get_date_index (list, tag, i, &date)) {
            /* vorbis suggests using ISO date formats */
            result =
                g_strdup_printf ("%s=%04d-%02d-%02d", vorbis_tag,
                (gint) g_date_get_year (date), (gint) g_date_get_month (date),
                (gint) g_date_get_day (date));
            g_date_free (date);
          }
        } else {
          GST_DEBUG ("Couldn't write tag %s", tag);
          continue;
        }
        break;
      }
    }
    l = g_list_prepend (l, result);
  }

  return g_list_reverse (l);
}

static void
write_one_tag (const GstTagList * list, const gchar * tag, gpointer user_data)
{
  MyForEach *data = (MyForEach *) user_data;
  GList *comments;
  GList *it;

  comments = gst_tag_to_vorbis_comments (list, tag);

  for (it = comments; it != NULL; it = it->next) {
    gchar *result = it->data;

    data->count++;
    data->data_count += strlen (result);
    data->entries = g_list_prepend (data->entries, result);
  }

  g_list_free (comments);
}

/**
 * gst_tag_list_to_vorbiscomment_buffer:
 * @list: tag list to convert
 * @id_data: identification data at start of stream
 * @id_data_length: length of identification data, may be 0 if @id_data is NULL
 * @vendor_string: string that describes the vendor string or NULL
 *
 * Creates a new vorbiscomment buffer from a tag list.
 *
 * Returns: A new #GstBuffer containing a vorbiscomment buffer with all tags
 *          that could be converted from the given tag list.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstBuffer *
gst_tag_list_to_vorbiscomment_buffer (const GstTagList * list,
    const guint8 * id_data, const guint id_data_length,
    const gchar * vendor_string)
{
  GstBuffer *buffer;
  guint8 *data;
  guint i;
  GList *l;
  MyForEach my_data = { 0, 0, NULL };
  guint vendor_len;
  int required_size;

  g_return_val_if_fail (GST_IS_TAG_LIST (list), NULL);
  g_return_val_if_fail (id_data != NULL || id_data_length == 0, NULL);

  if (vendor_string == NULL)
    vendor_string = "GStreamer encoded vorbiscomment";
  vendor_len = strlen (vendor_string);
  required_size = id_data_length + 4 + vendor_len + 4 + 1;
  gst_tag_list_foreach ((GstTagList *) list, write_one_tag, &my_data);
  required_size += 4 * my_data.count + my_data.data_count;
  buffer = gst_buffer_new_and_alloc (required_size);
  data = GST_BUFFER_DATA (buffer);
  if (id_data_length > 0) {
    memcpy (data, id_data, id_data_length);
    data += id_data_length;
  }
  GST_WRITE_UINT32_LE (data, vendor_len);
  data += 4;
  memcpy (data, vendor_string, vendor_len);
  data += vendor_len;
  l = my_data.entries = g_list_reverse (my_data.entries);
  GST_WRITE_UINT32_LE (data, my_data.count);
  data += 4;
  for (i = 0; i < my_data.count; i++) {
    guint size;
    gchar *cur;

    g_assert (l != NULL);
    cur = l->data;
    l = g_list_next (l);
    size = strlen (cur);
    GST_WRITE_UINT32_LE (data, size);
    data += 4;
    memcpy (data, cur, size);
    data += size;
  }
  g_list_foreach (my_data.entries, (GFunc) g_free, NULL);
  g_list_free (my_data.entries);
  *data = 1;

  return buffer;
}
