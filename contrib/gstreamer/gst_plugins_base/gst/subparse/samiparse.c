/* GStreamer SAMI subtitle parser
 * Copyright (c) 2006 Young-Ho Cha <ganadist at chollian net>
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

#include "samiparse.h"

/* FIXME: use Makefile stuff */
#ifndef GST_DISABLE_XML
#include <libxml/HTMLparser.h>
#include <string.h>

#define ITALIC_TAG 'i'
#define SPAN_TAG   's'
#define RUBY_TAG   'r'
#define RT_TAG     't'
#define CLEAR_TAG  '0'

typedef struct _GstSamiContext GstSamiContext;

struct _GstSamiContext
{
  GString *buf;                 /* buffer to collect content */
  GString *rubybuf;             /* buffer to collect ruby content */
  GString *resultbuf;           /* when opening the next 'sync' tag, move
                                 * from 'buf' to avoid to append following
                                 * content */
  GString *state;               /* in many sami files there are tags that
                                 * are not closed, so for each open tag the
                                 * parser will append a tag flag here so
                                 * that tags can be closed properly on
                                 * 'sync' tags. See _context_push_state()
                                 * and _context_pop_state(). */
  htmlParserCtxtPtr htmlctxt;   /* html parser context */
  gboolean has_result;          /* set when ready to push out result */
  gboolean in_title;            /* flag to avoid appending the title content
                                 * to buf */
  guint64 time1;                /* previous start attribute in sync tag */
  guint64 time2;                /* current start attribute in sync tag  */
};

static gchar *
has_tag (GString * str, const gchar tag)
{
  return strrchr (str->str, tag);
}

static void
sami_context_push_state (GstSamiContext * sctx, char state)
{
  g_string_append_c (sctx->state, state);
}

static void
sami_context_pop_state (GstSamiContext * sctx, char state)
{
  GString *str = g_string_new ("");
  GString *context_state = sctx->state;
  int i;

  for (i = context_state->len - 1; i >= 0; i--) {
    switch (context_state->str[i]) {
      case ITALIC_TAG:         /* <i> */
      {
        g_string_append (str, "</i>");
        break;
      }
      case SPAN_TAG:           /* <span foreground= > */
      {
        g_string_append (str, "</span>");
        break;
      }
      case RUBY_TAG:           /* <span size= >  -- ruby */
      {
        break;
      }
      case RT_TAG:             /*  ruby */
      {
        /* FIXME: support for furigana/ruby once implemented in pango */
        g_string_append (sctx->rubybuf, "</span>");
        if (has_tag (context_state, ITALIC_TAG)) {
          g_string_append (sctx->rubybuf, "</i>");
        }

        break;
      }
      default:
        break;
    }
    if (context_state->str[i] == state) {
      g_string_append (sctx->buf, str->str);
      g_string_free (str, TRUE);
      g_string_truncate (context_state, i);
      return;
    }
  }
  if (state == CLEAR_TAG) {
    g_string_append (sctx->buf, str->str);
    g_string_truncate (context_state, 0);
  }
  g_string_free (str, TRUE);
}

static void
handle_start_sync (GstSamiContext * sctx, const xmlChar ** atts)
{
  int i;

  sami_context_pop_state (sctx, CLEAR_TAG);
  if (atts != NULL) {
    for (i = 0; (atts[i] != NULL); i += 2) {
      const xmlChar *key, *value;

      key = atts[i];
      value = atts[i + 1];

      if (!value)
        continue;
      if (!xmlStrncmp ((const xmlChar *) "start", key, 5)) {
        sctx->time1 = sctx->time2;
        sctx->time2 = atoi ((const char *) value) * GST_MSECOND;
        sctx->has_result = TRUE;
        g_string_append (sctx->resultbuf, sctx->buf->str);
        g_string_truncate (sctx->buf, 0);
      }
    }
  }
}

static void
handle_start_font (GstSamiContext * sctx, const xmlChar ** atts)
{
  int i;

  sami_context_pop_state (sctx, SPAN_TAG);
  if (atts != NULL) {
    g_string_append (sctx->buf, "<span");
    for (i = 0; (atts[i] != NULL); i += 2) {
      const xmlChar *key, *value;

      key = atts[i];
      value = atts[i + 1];

      if (!value)
        continue;
      if (!xmlStrncmp ((const xmlChar *) "color", key, 5)) {
        /*
         * There are invalid color value in many
         * sami files.
         * It will fix hex color value that start without '#'
         */
        gchar *sharp = "";
        int len = xmlStrlen (value);

        if (!(*value == '#' && len == 7)) {
          gchar *r;

          /* check if it looks like hex */
          if (strtol ((const char *) value, &r, 16) >= 0 &&
              ((xmlChar *) r == (value + 6) && len == 6)) {
            sharp = "#";
          }
        }
        /* some colours can be found in many sami files, but X RGB database
         * doesn't contain a colour by this name, so map explicitly */
        if (!xmlStrncasecmp (value, (const xmlChar *) "aqua", len)) {
          value = (const xmlChar *) "#00ffff";
        } else if (!xmlStrncasecmp (value, (const xmlChar *) "crimson", len)) {
          value = (const xmlChar *) "#dc143c";
        } else if (!xmlStrncasecmp (value, (const xmlChar *) "fuchsia", len)) {
          value = (const xmlChar *) "#ff00ff";
        } else if (!xmlStrncasecmp (value, (const xmlChar *) "indigo", len)) {
          value = (const xmlChar *) "#4b0082";
        } else if (!xmlStrncasecmp (value, (const xmlChar *) "lime", len)) {
          value = (const xmlChar *) "#00ff00";
        } else if (!xmlStrncasecmp (value, (const xmlChar *) "olive", len)) {
          value = (const xmlChar *) "#808000";
        } else if (!xmlStrncasecmp (value, (const xmlChar *) "silver", len)) {
          value = (const xmlChar *) "#c0c0c0";
        } else if (!xmlStrncasecmp (value, (const xmlChar *) "teal", len)) {
          value = (const xmlChar *) "#008080";
        }
        g_string_append_printf (sctx->buf, " foreground=\"%s%s\"", sharp,
            value);
      } else if (!xmlStrncasecmp ((const xmlChar *) "face", key, 4)) {
        g_string_append_printf (sctx->buf, " font_family=\"%s\"", value);
      }
    }
    g_string_append_c (sctx->buf, '>');
    sami_context_push_state (sctx, SPAN_TAG);
  }
}

static void
start_sami_element (void *ctx, const xmlChar * name, const xmlChar ** atts)
{
  GstSamiContext *sctx = (GstSamiContext *) ctx;

  if (!xmlStrncmp ((const xmlChar *) "title", name, 5)) {
    sctx->in_title = TRUE;
  } else if (!xmlStrncmp ((const xmlChar *) "sync", name, 4)) {
    handle_start_sync (sctx, atts);
  } else if (!xmlStrncmp ((const xmlChar *) "font", name, 4)) {
    handle_start_font (sctx, atts);
  } else if (!xmlStrncmp ((const xmlChar *) "ruby", name, 4)) {
    sami_context_push_state (sctx, RUBY_TAG);
  } else if (!xmlStrncmp ((const xmlChar *) "br", name, 2)) {
    g_string_append_c (sctx->buf, '\n');
    /* FIXME: support for furigana/ruby once implemented in pango */
  } else if (!xmlStrncmp ((const xmlChar *) "rt", name, 2)) {
    if (has_tag (sctx->state, ITALIC_TAG)) {
      g_string_append (sctx->rubybuf, "<i>");
    }
    g_string_append (sctx->rubybuf, "<span size='xx-small' rise='-100'>");
    sami_context_push_state (sctx, RT_TAG);
  } else if (!xmlStrncmp ((const xmlChar *) "p", name, 1)) {
  } else if (!xmlStrncmp ((const xmlChar *) "i", name, 1)) {
    g_string_append (sctx->buf, "<i>");
    sami_context_push_state (sctx, ITALIC_TAG);
  }
}

static void
end_sami_element (void *ctx, const xmlChar * name)
{
  GstSamiContext *sctx = (GstSamiContext *) ctx;

  if (!xmlStrncmp ((const xmlChar *) "title", name, 5)) {
    sctx->in_title = FALSE;
  } else if (!xmlStrncmp ((const xmlChar *) "font", name, 4)) {
    sami_context_pop_state (sctx, SPAN_TAG);
  } else if (!xmlStrncmp ((const xmlChar *) "ruby", name, 4)) {
    sami_context_pop_state (sctx, RUBY_TAG);
  } else if (!xmlStrncmp ((const xmlChar *) "i", name, 1)) {
    sami_context_pop_state (sctx, ITALIC_TAG);
  }
}

static void
characters_sami (void *ctx, const xmlChar * ch, int len)
{
  GstSamiContext *sctx = (GstSamiContext *) ctx;
  gchar *escaped;

  /* skip title */
  if (sctx->in_title)
    return;

  escaped = g_markup_escape_text ((const gchar *) ch, len);
  if (has_tag (sctx->state, RT_TAG)) {
    g_string_append_c (sctx->rubybuf, ' ');
    g_string_append (sctx->rubybuf, escaped);
    g_string_append_c (sctx->rubybuf, ' ');
  } else {
    g_string_append (sctx->buf, escaped);
  }
  g_free (escaped);
}

static xmlSAXHandler samiSAXHandlerStruct = {
  NULL,                         /* internalSubset */
  NULL,                         /* isStandalone */
  NULL,                         /* hasInternalSubset */
  NULL,                         /* hasExternalSubset */
  NULL,                         /* resolveEntity */
  NULL,                         /* getEntity */
  NULL,                         /* entityDecl */
  NULL,                         /* notationDecl */
  NULL,                         /* attributeDecl */
  NULL,                         /* elementDecl */
  NULL,                         /* unparsedEntityDecl */
  NULL,                         /* setDocumentLocator */
  NULL,                         /* startDocument */
  NULL,                         /* endDocument */
  start_sami_element,           /* startElement */
  end_sami_element,             /* endElement */
  NULL,                         /* reference */
  characters_sami,              /* characters */
  NULL,                         /* ignorableWhitespace */
  NULL,                         /* processingInstruction */
  NULL,                         /* comment */
  NULL,                         /* xmlParserWarning */
  NULL,                         /* xmlParserError */
  NULL,                         /* xmlParserError */
  NULL,                         /* getParameterEntity */
  NULL,                         /* cdataBlock */
  NULL,                         /* externalSubset */
  1,                            /* initialized */
  NULL,                         /* private */
  NULL,                         /* startElementNsSAX2Func */
  NULL,                         /* endElementNsSAX2Func */
  NULL                          /* xmlStructuredErrorFunc */
};
static xmlSAXHandlerPtr samiSAXHandler = &samiSAXHandlerStruct;
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
sami_context_init (ParserState * state)
{
  GstSamiContext *context;

  g_assert (state->user_data == NULL);
  state->user_data = (gpointer) g_new0 (GstSamiContext, 1);
  context = (GstSamiContext *) state->user_data;

  context->htmlctxt = htmlCreatePushParserCtxt (samiSAXHandler, context,
      "", 0, NULL, XML_CHAR_ENCODING_UTF8);
  context->buf = g_string_new ("");
  context->rubybuf = g_string_new ("");
  context->resultbuf = g_string_new ("");
  context->state = g_string_new ("");
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
sami_context_deinit (ParserState * state)
{
  GstSamiContext *context = (GstSamiContext *) state->user_data;

  if (context) {
    htmlParserCtxtPtr htmlctxt = context->htmlctxt;

    /* destroy sax context */
    htmlDocPtr doc;

    htmlParseChunk (htmlctxt, "", 0, 1);
    doc = htmlctxt->myDoc;
    htmlFreeParserCtxt (htmlctxt);
    context->htmlctxt = NULL;
    if (doc)
      xmlFreeDoc (doc);
    g_string_free (context->buf, TRUE);
    g_string_free (context->rubybuf, TRUE);
    g_string_free (context->resultbuf, TRUE);
    g_string_free (context->state, TRUE);
    g_free (context);
    state->user_data = NULL;
  }
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


void
sami_context_reset (ParserState * state)
{
  GstSamiContext *context = (GstSamiContext *) state->user_data;

  if (context) {
    g_string_truncate (context->buf, 0);
    g_string_truncate (context->rubybuf, 0);
    g_string_truncate (context->resultbuf, 0);
    g_string_truncate (context->state, 0);
    context->has_result = FALSE;
    context->in_title = FALSE;
    context->time1 = 0;
    context->time2 = 0;
  }
}

static gchar *
fix_invalid_entities (const gchar * line)
{
  const gchar *cp, *pp;         /* current pointer, previous pointer */
  gssize size;
  GString *ret = g_string_new (NULL);

  pp = line;
  cp = strchr (line, '&');
  while (cp) {
    size = cp - pp;
    ret = g_string_append_len (ret, pp, size);
    cp++;
    if (g_ascii_strncasecmp (cp, "nbsp;", 5)
        && (!g_ascii_strncasecmp (cp, "nbsp", 4))) {
      /* translate "&nbsp" to "&nbsp;" */
      ret = g_string_append_len (ret, "&nbsp;", 6);
      cp += 4;
    } else if (g_ascii_strncasecmp (cp, "quot;", 5)
        && g_ascii_strncasecmp (cp, "amp;", 4)
        && g_ascii_strncasecmp (cp, "apos;", 5)
        && g_ascii_strncasecmp (cp, "lt;", 3)
        && g_ascii_strncasecmp (cp, "gt;", 3)
        && g_ascii_strncasecmp (cp, "nbsp;", 5)
        && cp[0] != '#') {
      /* translate "&" to "&amp;" */
      ret = g_string_append_len (ret, "&amp;", 5);
    } else {
      /* do not translate */
      ret = g_string_append_c (ret, '&');
    }

    pp = cp;
    cp = strchr (pp, '&');
  }
  ret = g_string_append (ret, pp);
  return g_string_free (ret, FALSE);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


gchar *
parse_sami (ParserState * state, const gchar * line)
{
  gchar *fixed_line;
  GstSamiContext *context = (GstSamiContext *) state->user_data;

  fixed_line = fix_invalid_entities (line);
  htmlParseChunk (context->htmlctxt, fixed_line, strlen (fixed_line), 0);
  g_free (fixed_line);

  if (context->has_result) {
    gchar *r;

    if (context->rubybuf->len) {
      context->rubybuf = g_string_append_c (context->rubybuf, '\n');
      g_string_prepend (context->resultbuf, context->rubybuf->str);
      context->rubybuf = g_string_truncate (context->rubybuf, 0);
    }

    r = g_string_free (context->resultbuf, FALSE);
    context->resultbuf = g_string_new ("");
    state->start_time = context->time1;
    state->duration = context->time2 - context->time1;
    context->has_result = FALSE;
    return r;
  }
  return NULL;
}

#else /* GST_DISABLE_XML */
#ifdef __SYMBIAN32__
EXPORT_C
#endif
gchar *
parse_sami (ParserState * state, const gchar * line)
{
  /* our template caps should not include sami in this case */
  g_assert_not_reached ();
}

#ifdef __SYMBIAN32__
EXPORT_C
#endif
void
sami_context_init (ParserState * state)
{
  return;
}

#ifdef __SYMBIAN32__
EXPORT_C
#endif
void
sami_context_deinit (ParserState * state)
{
  return;
}

#ifdef __SYMBIAN32__
EXPORT_C
#endif
void
sami_context_reset (ParserState * state)
{
  return;
}

#endif /* GST_DISABLE_XML */
