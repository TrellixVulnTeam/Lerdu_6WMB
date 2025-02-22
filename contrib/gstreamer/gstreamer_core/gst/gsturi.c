/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *
 * gsturi.c: register URI handlers
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
 * SECTION:gsturihandler
 * @short_description: Interface to ease URI handling in plugins.
 *
 * The URIHandler is an interface that is implemented by Source and Sink 
 * #GstElement to simplify then handling of URI.
 *
 * An application can use the following functions to quickly get an element
 * that handles the given URI for reading or writing
 * (gst_element_make_from_uri()).
 *
 * Source and Sink plugins should implement this interface when possible.
 *
 * Last reviewed on 2005-11-09 (0.9.4)
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "gst_private.h"
#include "gsturi.h"
#include "gstinfo.h"
#include "gstmarshal.h"
#include "gstregistry.h"

#include <string.h>

#ifdef __SYMBIAN32__
#include <glib_global.h>
#endif

GST_DEBUG_CATEGORY_STATIC (gst_uri_handler_debug);
#define GST_CAT_DEFAULT gst_uri_handler_debug

static void gst_uri_handler_base_init (gpointer g_class);
#ifdef __SYMBIAN32__
EXPORT_C
#endif


GType
gst_uri_handler_get_type (void)
{
  static GType urihandler_type = 0;

  if (G_UNLIKELY (urihandler_type == 0)) {
    static const GTypeInfo urihandler_info = {
      sizeof (GstURIHandlerInterface),
      gst_uri_handler_base_init,
      NULL,
      NULL,
      NULL,
      NULL,
      0,
      0,
      NULL,
      NULL
    };

    urihandler_type = g_type_register_static (G_TYPE_INTERFACE,
        "GstURIHandler", &urihandler_info, 0);

    GST_DEBUG_CATEGORY_INIT (gst_uri_handler_debug, "GST_URI", GST_DEBUG_BOLD,
        "handling of URIs");
  }
  return urihandler_type;
}
static void
gst_uri_handler_base_init (gpointer g_class)
{
  static gboolean initialized = FALSE;

  if (G_UNLIKELY (!initialized)) {

    /**
     * GstURIHandler::new-uri:
     * @handler: The #GstURIHandler which emitted the signal
     * @uri: The new URI, or NULL if the URI was removed
     *
     * The URI of the given @handler has changed.
     */

    g_signal_new ("new-uri", GST_TYPE_URI_HANDLER, G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET (GstURIHandlerInterface, new_uri), NULL, NULL,
        gst_marshal_VOID__STRING, G_TYPE_NONE, 1, G_TYPE_STRING);
    initialized = TRUE;
  }
}

static const guchar acceptable[96] = {  /* X0   X1   X2   X3   X4   X5   X6   X7   X8   X9   XA   XB   XC   XD   XE   XF */
  0x00, 0x3F, 0x20, 0x20, 0x20, 0x00, 0x2C, 0x3F, 0x3F, 0x3F, 0x3F, 0x22, 0x20, 0x3F, 0x3F, 0x1C,       /* 2X  !"#$%&'()*+,-./   */
  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x38, 0x20, 0x20, 0x2C, 0x20, 0x2C,       /* 3X 0123456789:;<=>?   */
  0x30, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,       /* 4X @ABCDEFGHIJKLMNO   */
  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x20, 0x20, 0x20, 0x20, 0x3F,       /* 5X PQRSTUVWXYZ[\]^_   */
  0x20, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,       /* 6X `abcdefghijklmno   */
  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x20, 0x20, 0x20, 0x3F, 0x20        /* 7X pqrstuvwxyz{|}~DEL */
};

typedef enum
{
  UNSAFE_ALL = 0x1,             /* Escape all unsafe characters   */
  UNSAFE_ALLOW_PLUS = 0x2,      /* Allows '+'  */
  UNSAFE_PATH = 0x4,            /* Allows '/' and '?' and '&' and '='  */
  UNSAFE_DOS_PATH = 0x8,        /* Allows '/' and '?' and '&' and '=' and ':' */
  UNSAFE_HOST = 0x10,           /* Allows '/' and ':' and '@' */
  UNSAFE_SLASHES = 0x20         /* Allows all characters except for '/' and '%' */
} UnsafeCharacterSet;

#define HEX_ESCAPE '%'

/*  Escape undesirable characters using %
 *  -------------------------------------
 *
 * This function takes a pointer to a string in which
 * some characters may be unacceptable unescaped.
 * It returns a string which has these characters
 * represented by a '%' character followed by two hex digits.
 *
 * This routine returns a g_malloced string.
 */

static const gchar hex[16] = "0123456789ABCDEF";

static gchar *
escape_string_internal (const gchar * string, UnsafeCharacterSet mask)
{
#define ACCEPTABLE_CHAR(a) ((a)>=32 && (a)<128 && (acceptable[(a)-32] & use_mask))

  const gchar *p;
  gchar *q;
  gchar *result;
  guchar c;
  gint unacceptable;
  UnsafeCharacterSet use_mask;

  g_return_val_if_fail (mask == UNSAFE_ALL
      || mask == UNSAFE_ALLOW_PLUS
      || mask == UNSAFE_PATH
      || mask == UNSAFE_DOS_PATH
      || mask == UNSAFE_HOST || mask == UNSAFE_SLASHES, NULL);

  if (string == NULL) {
    return NULL;
  }

  unacceptable = 0;
  use_mask = mask;
  for (p = string; *p != '\0'; p++) {
    c = *p;
    if (!ACCEPTABLE_CHAR (c)) {
      unacceptable++;
    }
    if ((use_mask == UNSAFE_HOST) && (unacceptable || (c == '/'))) {
      /* when escaping a host, if we hit something that needs to be escaped, or we finally
       * hit a path separator, revert to path mode (the host segment of the url is over).
       */
      use_mask = UNSAFE_PATH;
    }
  }

  result = g_malloc (p - string + unacceptable * 2 + 1);

  use_mask = mask;
  for (q = result, p = string; *p != '\0'; p++) {
    c = *p;

    if (!ACCEPTABLE_CHAR (c)) {
      *q++ = HEX_ESCAPE;        /* means hex coming */
      *q++ = hex[c >> 4];
      *q++ = hex[c & 15];
    } else {
      *q++ = c;
    }
    if ((use_mask == UNSAFE_HOST) && (!ACCEPTABLE_CHAR (c) || (c == '/'))) {
      use_mask = UNSAFE_PATH;
    }
  }

  *q = '\0';

  return result;
}

/**
 * escape_string:
 * @string: string to be escaped
 *
 * Escapes @string, replacing any and all special characters
 * with equivalent escape sequences.
 *
 * Return value: a newly allocated string equivalent to @string
 * but with all special characters escaped
 **/
static gchar *
escape_string (const gchar * string)
{
  return escape_string_internal (string, UNSAFE_ALL);
}

static int
hex_to_int (gchar c)
{
  return c >= '0' && c <= '9' ? c - '0'
      : c >= 'A' && c <= 'F' ? c - 'A' + 10
      : c >= 'a' && c <= 'f' ? c - 'a' + 10 : -1;
}

static int
unescape_character (const char *scanner)
{
  int first_digit;
  int second_digit;

  first_digit = hex_to_int (*scanner++);
  if (first_digit < 0) {
    return -1;
  }

  second_digit = hex_to_int (*scanner++);
  if (second_digit < 0) {
    return -1;
  }

  return (first_digit << 4) | second_digit;
}

/**
 * unescape_string:
 * @escaped_string: an escaped URI, path, or other string
 * @illegal_characters: a string containing a sequence of characters
 * considered "illegal", '\0' is automatically in this list.
 *
 * Decodes escaped characters (i.e. PERCENTxx sequences) in @escaped_string.
 * Characters are encoded in PERCENTxy form, where xy is the ASCII hex code
 * for character 16x+y.
 *
 * Return value: a newly allocated string with the unescaped equivalents,
 * or %NULL if @escaped_string contained one of the characters
 * in @illegal_characters.
 **/
static char *
unescape_string (const gchar * escaped_string, const gchar * illegal_characters)
{
  const gchar *in;
  gchar *out, *result;
  gint character;

  if (escaped_string == NULL) {
    return NULL;
  }

  result = g_malloc (strlen (escaped_string) + 1);

  out = result;
  for (in = escaped_string; *in != '\0'; in++) {
    character = *in;
    if (*in == HEX_ESCAPE) {
      character = unescape_character (in + 1);

      /* Check for an illegal character. We consider '\0' illegal here. */
      if (character <= 0
          || (illegal_characters != NULL
              && strchr (illegal_characters, (char) character) != NULL)) {
        g_free (result);
        return NULL;
      }
      in += 2;
    }
    *out++ = (char) character;
  }

  *out = '\0';
  g_assert ((gsize) (out - result) <= strlen (escaped_string));
  return result;

}


static void
gst_uri_protocol_check_internal (const gchar * uri, gchar ** endptr)
{
  gchar *check = (gchar *) uri;

  g_assert (uri != NULL);
  g_assert (endptr != NULL);

  if (g_ascii_isalpha (*check)) {
    check++;
    while (g_ascii_isalnum (*check))
      check++;
  }

  *endptr = check;
}

/**
 * gst_uri_protocol_is_valid:
 * @protocol: A string
 *
 * Tests if the given string is a valid protocol identifier. Protocols
 * must consist of alphanumeric characters and not start with a number.
 *
 * Returns: TRUE if the string is a valid protocol identifier, FALSE otherwise.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_uri_protocol_is_valid (const gchar * protocol)
{
  gchar *endptr;

  g_return_val_if_fail (protocol != NULL, FALSE);

  gst_uri_protocol_check_internal (protocol, &endptr);

  return *endptr == '\0' && endptr != protocol;
}

/**
 * gst_uri_is_valid:
 * @uri: A URI string
 *
 * Tests if the given string is a valid URI identifier. URIs start with a valid
 * protocol followed by "://" and maybe a string identifying the location.
 *
 * Returns: TRUE if the string is a valid URI
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_uri_is_valid (const gchar * uri)
{
  gchar *endptr;

  g_return_val_if_fail (uri != NULL, FALSE);

  gst_uri_protocol_check_internal (uri, &endptr);

  return (*endptr == ':' && *(endptr + 1) == '/' && *(endptr + 2) == '/');
}

/**
 * gst_uri_get_protocol:
 * @uri: A URI string
 *
 * Extracts the protocol out of a given valid URI. The returned string must be
 * freed using g_free().
 *
 * Returns: The protocol for this URI.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gchar *
gst_uri_get_protocol (const gchar * uri)
{
  gchar *colon;

  g_return_val_if_fail (uri != NULL, NULL);
  g_return_val_if_fail (gst_uri_is_valid (uri), NULL);

  colon = strstr (uri, "://");

  return g_strdown (g_strndup (uri, colon - uri));
}

/**
 * gst_uri_has_protocol:
 * @uri: an URI string
 * @protocol: a protocol string (e.g. "http")
 *
 * Checks if the protocol of a given valid URI matches @protocol.
 *
 * Returns: %TRUE if the protocol matches.
 *
 * Since: 0.10.4
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_uri_has_protocol (const gchar * uri, const gchar * protocol)
{
  gchar *colon;

  g_return_val_if_fail (uri != NULL, FALSE);
  g_return_val_if_fail (protocol != NULL, FALSE);
  g_return_val_if_fail (gst_uri_is_valid (uri), FALSE);

  colon = strstr (uri, "://");

  if (colon == NULL)
    return FALSE;

  return (strncmp (uri, protocol, (gsize) (colon - uri)) == 0);
}

/**
 * gst_uri_get_location:
 * @uri: A URI string
 *
 * Extracts the location out of a given valid URI, ie. the protocol and "://"
 * are stripped from the URI, which means that the location returned includes
 * the hostname if one is specified. The returned string must be freed using
 * g_free().
 *
 * Returns: The location for this URI. Returns NULL if the URI isn't valid. If
 * the URI does not contain a location, an empty string is returned.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gchar *
gst_uri_get_location (const gchar * uri)
{
  const gchar *colon;
  gchar *unescaped = NULL;

  g_return_val_if_fail (uri != NULL, NULL);
  g_return_val_if_fail (gst_uri_is_valid (uri), NULL);

  colon = strstr (uri, "://");

  unescaped = unescape_string (colon + 3, "/");

  /* On Windows an URI might look like file:///c:/foo/bar.txt or
   * file:///c|/foo/bar.txt (some Netscape versions) and we want to
   * return c:/foo/bar.txt as location rather than /c:/foo/bar.txt.
   * Can't use g_filename_from_uri() here because it will only handle the
   * file:// protocol */
#ifdef G_OS_WIN32
  if (unescaped != NULL && unescaped[0] == '/' &&
      g_ascii_isalpha (unescaped[1]) &&
      (unescaped[2] == ':' || unescaped[2] == '|')) {
    unescaped[2] = ':';
    g_memmove (unescaped, unescaped + 1, strlen (unescaped + 1) + 1);
  }
#endif

  GST_LOG ("extracted location '%s' from URI '%s'", GST_STR_NULL (unescaped),
      uri);;
  return unescaped;
}

/**
 * gst_uri_construct:
 * @protocol: Protocol for URI
 * @location: Location for URI
 *
 * Constructs a URI for a given valid protocol and location.
 *
 * Returns: a new string for this URI. Returns NULL if the given URI protocol
 * is not valid, or the given location is NULL.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gchar *
gst_uri_construct (const gchar * protocol, const gchar * location)
{
  char *escaped;
  char *retval;

  g_return_val_if_fail (gst_uri_protocol_is_valid (protocol), NULL);
  g_return_val_if_fail (location != NULL, NULL);

  escaped = escape_string (location);
  retval = g_strdup_printf ("%s://%s", protocol, escaped);
  g_free (escaped);

  return retval;
}

typedef struct
{
  GstURIType type;
  const gchar *protocol;
}
SearchEntry;

static gboolean
search_by_entry (GstPluginFeature * feature, gpointer search_entry)
{
  gchar **protocols;
  GstElementFactory *factory;
  SearchEntry *entry = (SearchEntry *) search_entry;

  if (!GST_IS_ELEMENT_FACTORY (feature))
    return FALSE;
  factory = GST_ELEMENT_FACTORY (feature);

  if (gst_element_factory_get_uri_type (factory) != entry->type)
    return FALSE;

  protocols = gst_element_factory_get_uri_protocols (factory);

  if (protocols == NULL) {
    g_warning ("Factory '%s' implements GstUriHandler interface but returned "
        "no supported protocols!", gst_plugin_feature_get_name (feature));
    return FALSE;
  }

  while (*protocols != NULL) {
    if (g_ascii_strcasecmp (*protocols, entry->protocol) == 0)
      return TRUE;
    protocols++;
  }
  return FALSE;
}

static gint
sort_by_rank (gconstpointer a, gconstpointer b)
{
  GstPluginFeature *first = GST_PLUGIN_FEATURE (a);
  GstPluginFeature *second = GST_PLUGIN_FEATURE (b);

  return gst_plugin_feature_get_rank (second) -
      gst_plugin_feature_get_rank (first);
}

static GList *
get_element_factories_from_uri_protocol (const GstURIType type,
    const gchar * protocol)
{
  GList *possibilities;
  SearchEntry entry;

  g_return_val_if_fail (protocol, NULL);

  entry.type = type;
  entry.protocol = protocol;
  possibilities = gst_registry_feature_filter (gst_registry_get_default (),
      search_by_entry, FALSE, &entry);

  return possibilities;
}

/**
 * gst_uri_protocol_is_supported:
 * @type: Whether to check for a source or a sink
 * @protocol: Protocol that should be checked for (e.g. "http" or "smb")
 *
 * Checks if an element exists that supports the given URI protocol. Note
 * that a positive return value does not imply that a subsequent call to
 * gst_element_make_from_uri() is guaranteed to work.
 *
 * Returns: TRUE
 *
 * Since: 0.10.13
*/
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_uri_protocol_is_supported (const GstURIType type, const gchar * protocol)
{
  GList *possibilities;

  g_return_val_if_fail (protocol, FALSE);

  possibilities = get_element_factories_from_uri_protocol (type, protocol);

  if (possibilities) {
    g_list_free (possibilities);
    return TRUE;
  } else
    return FALSE;
}

/**
 * gst_element_make_from_uri:
 * @type: Whether to create a source or a sink
 * @uri: URI to create an element for
 * @elementname: Name of created element, can be NULL.
 *
 * Creates an element for handling the given URI.
 *
 * Returns: a new element or NULL if none could be created
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstElement *
gst_element_make_from_uri (const GstURIType type, const gchar * uri,
    const gchar * elementname)
{
  GList *possibilities, *walk;
  gchar *protocol;
  GstElement *ret = NULL;

  g_return_val_if_fail (GST_URI_TYPE_IS_VALID (type), NULL);
  g_return_val_if_fail (gst_uri_is_valid (uri), NULL);

  protocol = gst_uri_get_protocol (uri);
  possibilities = get_element_factories_from_uri_protocol (type, protocol);
  g_free (protocol);

  if (!possibilities) {
    GST_DEBUG ("No %s for URI '%s'", type == GST_URI_SINK ? "sink" : "source",
        uri);
    return NULL;
  }

  possibilities = g_list_sort (possibilities, sort_by_rank);
  walk = possibilities;
  while (walk) {
    if ((ret = gst_element_factory_create (GST_ELEMENT_FACTORY (walk->data),
                elementname)) != NULL) {
      GstURIHandler *handler = GST_URI_HANDLER (ret);

      if (gst_uri_handler_set_uri (handler, uri))
        break;
      gst_object_unref (ret);
      ret = NULL;
    }
    walk = walk->next;
  }
  gst_plugin_feature_list_free (possibilities);

  GST_LOG_OBJECT (ret, "created %s for URL '%s'",
      type == GST_URI_SINK ? "sink" : "source", uri);
  return ret;
}

/**
 * gst_uri_handler_get_uri_type:
 * @handler: A #GstURIHandler.
 *
 * Gets the type of the given URI handler
 *
 * Returns: the #GstURIType of the URI handler.
 * Returns #GST_URI_UNKNOWN if the @handler isn't implemented correctly.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint
gst_uri_handler_get_uri_type (GstURIHandler * handler)
{
  GstURIHandlerInterface *iface;
  guint ret;

  g_return_val_if_fail (GST_IS_URI_HANDLER (handler), GST_URI_UNKNOWN);

  iface = GST_URI_HANDLER_GET_INTERFACE (handler);
  g_return_val_if_fail (iface != NULL, GST_URI_UNKNOWN);
  g_return_val_if_fail (iface->get_type != NULL, GST_URI_UNKNOWN);
  ret = iface->get_type ();
  g_return_val_if_fail (GST_URI_TYPE_IS_VALID (ret), GST_URI_UNKNOWN);

  return ret;
}

/**
 * gst_uri_handler_get_protocols:
 * @handler: A #GstURIHandler.
 *
 * Gets the list of protocols supported by @handler. This list may not be
 * modified.
 *
 * Returns: the supported protocols.
 * Returns NULL if the @handler isn't implemented properly, or the @handler
 * doesn't support any protocols.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gchar **
gst_uri_handler_get_protocols (GstURIHandler * handler)
{
  GstURIHandlerInterface *iface;
  gchar **ret;

  g_return_val_if_fail (GST_IS_URI_HANDLER (handler), NULL);

  iface = GST_URI_HANDLER_GET_INTERFACE (handler);
  g_return_val_if_fail (iface != NULL, NULL);
  g_return_val_if_fail (iface->get_protocols != NULL ||
      iface->get_protocols_full != NULL, NULL);

  if (iface->get_protocols != NULL) {
    ret = iface->get_protocols ();
  } else {
    ret = iface->get_protocols_full (G_OBJECT_TYPE (handler));
  }
  g_return_val_if_fail (ret != NULL, NULL);

  return ret;
}

/**
 * gst_uri_handler_get_uri:
 * @handler: A #GstURIHandler
 *
 * Gets the currently handled URI.
 *
 * Returns: the URI currently handled by the @handler.
 * Returns NULL if there are no URI currently handled. The returned
 * string must not be modified or freed.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

G_CONST_RETURN gchar *
gst_uri_handler_get_uri (GstURIHandler * handler)
{
  GstURIHandlerInterface *iface;
  const gchar *ret;

  g_return_val_if_fail (GST_IS_URI_HANDLER (handler), NULL);

  iface = GST_URI_HANDLER_GET_INTERFACE (handler);
  g_return_val_if_fail (iface != NULL, NULL);
  g_return_val_if_fail (iface->get_uri != NULL, NULL);
  ret = iface->get_uri (handler);
  if (ret != NULL)
    g_return_val_if_fail (gst_uri_is_valid (ret), NULL);

  return ret;
}

/**
 * gst_uri_handler_set_uri:
 * @handler: A #GstURIHandler
 * @uri: URI to set
 *
 * Tries to set the URI of the given handler.
 *
 * Returns: TRUE if the URI was set successfully, else FALSE.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_uri_handler_set_uri (GstURIHandler * handler, const gchar * uri)
{
  GstURIHandlerInterface *iface;

  g_return_val_if_fail (GST_IS_URI_HANDLER (handler), FALSE);
  g_return_val_if_fail (gst_uri_is_valid (uri), FALSE);

  iface = GST_URI_HANDLER_GET_INTERFACE (handler);
  g_return_val_if_fail (iface != NULL, FALSE);
  g_return_val_if_fail (iface->set_uri != NULL, FALSE);
  return iface->set_uri (handler, uri);
}

/**
 * gst_uri_handler_new_uri:
 * @handler: A #GstURIHandler
 * @uri: new URI or NULL if it was unset
 *
 * Emits the new-uri signal for a given handler, when that handler has a new URI.
 * This function should only be called by URI handlers themselves.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_uri_handler_new_uri (GstURIHandler * handler, const gchar * uri)
{
  g_return_if_fail (GST_IS_URI_HANDLER (handler));

  g_signal_emit_by_name (handler, "new-uri", uri);
}
