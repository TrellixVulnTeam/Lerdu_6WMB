/* GStreamer
 * Copyright (C) <2003> David A. Schleef <ds@schleef.org>
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
 * SECTION:gstcaps
 * @short_description: Structure describing sets of media formats
 * @see_also: #GstStructure
 *
 * Caps (capabilities) are lighweight refcounted objects describing media types.
 * They are composed of an array of #GstStructure.
 *
 * Caps are exposed on #GstPadTemplate to describe all possible types a
 * given pad can handle. They are also stored in the #GstRegistry along with
 * a description of the #GstElement.
 *
 * Caps are exposed on the element pads using the gst_pad_get_caps() pad
 * function. This function describes the possible types that the pad can
 * handle or produce at runtime.
 *
 * Caps are also attached to buffers to describe to content of the data
 * pointed to by the buffer with gst_buffer_set_caps(). Caps attached to
 * a #GstBuffer allow for format negotiation upstream and downstream.
 *
 * A #GstCaps can be constructed with the following code fragment:
 *
 * <example>
 *  <title>Creating caps</title>
 *  <programlisting>
 *  GstCaps *caps;
 *  caps = gst_caps_new_simple ("video/x-raw-yuv",
 *       "format", GST_TYPE_FOURCC, GST_MAKE_FOURCC ('I', '4', '2', '0'),
 *       "framerate", GST_TYPE_FRACTION, 25, 1,
 *       "pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
 *       "width", G_TYPE_INT, 320,
 *       "height", G_TYPE_INT, 240,
 *       NULL);
 *  </programlisting>
 * </example>
 *
 * A #GstCaps is fixed when it has no properties with ranges or lists. Use
 * gst_caps_is_fixed() to test for fixed caps. Only fixed caps can be
 * set on a #GstPad or #GstBuffer.
 *
 * Various methods exist to work with the media types such as subtracting
 * or intersecting.
 *
 * Last reviewed on 2007-02-13 (0.10.10)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>
#include <signal.h>

#include "gst_private.h"
#include <gst/gst.h>

#ifdef __SYMBIAN32__
#include <glib_global.h>
#endif

#define DEBUG_REFCOUNT

#define CAPS_POISON(caps) G_STMT_START{ \
  if (caps) { \
    GstCaps *_newcaps = gst_caps_copy (caps); \
    gst_caps_unref(caps); \
    caps = _newcaps; \
  } \
} G_STMT_END
#define STRUCTURE_POISON(structure) G_STMT_START{ \
  if (structure) { \
    GstStructure *_newstruct = gst_structure_copy (structure); \
    gst_structure_free(structure); \
    structure = _newstruct; \
  } \
} G_STMT_END
#define IS_WRITABLE(caps) \
  (g_atomic_int_get (&(caps)->refcount) == 1)

#if GLIB_CHECK_VERSION (2, 10, 0)
#define ALLOC_CAPS()    g_slice_new (GstCaps)
#define FREE_CAPS(caps) g_slice_free (GstCaps, caps)
#else
#define ALLOC_CAPS()    g_new (GstCaps, 1)
#define FREE_CAPS(caps) g_free (caps)
#endif

/* lock to protect multiple invocations of static caps to caps conversion */
G_LOCK_DEFINE_STATIC (static_caps_lock);

static void gst_caps_transform_to_string (const GValue * src_value,
    GValue * dest_value);
static gboolean gst_caps_from_string_inplace (GstCaps * caps,
    const gchar * string);
static GstCaps *gst_caps_copy_conditional (GstCaps * src);
#ifdef __SYMBIAN32__
EXPORT_C
#endif


GType
gst_caps_get_type (void)
{
  static GType gst_caps_type = 0;

  if (G_UNLIKELY (gst_caps_type == 0)) {
    gst_caps_type = g_boxed_type_register_static ("GstCaps",
        (GBoxedCopyFunc) gst_caps_copy_conditional,
        (GBoxedFreeFunc) gst_caps_unref);

    g_value_register_transform_func (gst_caps_type,
        G_TYPE_STRING, gst_caps_transform_to_string);
  }

  return gst_caps_type;
}

/* creation/deletion */

/**
 * gst_caps_new_empty:
 *
 * Creates a new #GstCaps that is empty.  That is, the returned
 * #GstCaps contains no media formats.
 * Caller is responsible for unreffing the returned caps.
 *
 * Returns: the new #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_new_empty (void)
{
  GstCaps *caps = ALLOC_CAPS ();

  caps->type = GST_TYPE_CAPS;
  caps->refcount = 1;
  caps->flags = 0;
  caps->structs = g_ptr_array_new ();

#ifdef DEBUG_REFCOUNT
  GST_CAT_LOG (GST_CAT_CAPS, "created caps %p", caps);
#endif

  return caps;
}

/**
 * gst_caps_new_any:
 *
 * Creates a new #GstCaps that indicates that it is compatible with
 * any media format.
 *
 * Returns: the new #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_new_any (void)
{
  GstCaps *caps = gst_caps_new_empty ();

  caps->flags = GST_CAPS_FLAGS_ANY;

  return caps;
}

/**
 * gst_caps_new_simple:
 * @media_type: the media type of the structure
 * @fieldname: first field to set
 * @...: additional arguments
 *
 * Creates a new #GstCaps that contains one #GstStructure.  The
 * structure is defined by the arguments, which have the same format
 * as gst_structure_new().
 * Caller is responsible for unreffing the returned caps.
 *
 * Returns: the new #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_new_simple (const char *media_type, const char *fieldname, ...)
{
  GstCaps *caps;
  GstStructure *structure;
  va_list var_args;

  caps = gst_caps_new_empty ();

  va_start (var_args, fieldname);
  structure = gst_structure_new_valist (media_type, fieldname, var_args);
  va_end (var_args);

  gst_caps_append_structure (caps, structure);

  return caps;
}

/**
 * gst_caps_new_full:
 * @struct1: the first structure to add
 * @...: additional structures to add
 *
 * Creates a new #GstCaps and adds all the structures listed as
 * arguments.  The list must be NULL-terminated.  The structures
 * are not copied; the returned #GstCaps owns the structures.
 *
 * Returns: the new #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_new_full (GstStructure * struct1, ...)
{
  GstCaps *caps;
  va_list var_args;

  va_start (var_args, struct1);
  caps = gst_caps_new_full_valist (struct1, var_args);
  va_end (var_args);

  return caps;
}

/**
 * gst_caps_new_full_valist:
 * @structure: the first structure to add
 * @var_args: additional structures to add
 *
 * Creates a new #GstCaps and adds all the structures listed as
 * arguments.  The list must be NULL-terminated.  The structures
 * are not copied; the returned #GstCaps owns the structures.
 *
 * Returns: the new #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_new_full_valist (GstStructure * structure, va_list var_args)
{
  GstCaps *caps;

  caps = gst_caps_new_empty ();

  while (structure) {
    gst_caps_append_structure (caps, structure);
    structure = va_arg (var_args, GstStructure *);
  }

  return caps;
}

/**
 * gst_caps_copy:
 * @caps: the #GstCaps to copy
 *
 * Creates a new #GstCaps as a copy of the old @caps. The new caps will have a
 * refcount of 1, owned by the caller. The structures are copied as well.
 *
 * Note that this function is the semantic equivalent of a gst_caps_ref()
 * followed by a gst_caps_make_writable(). If you only want to hold on to a
 * reference to the data, you should use gst_caps_ref().
 *
 * When you are finished with the caps, call gst_caps_unref() on it.
 *
 * Returns: the new #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_copy (const GstCaps * caps)
{
  GstCaps *newcaps;
  GstStructure *structure;
  guint i;

  g_return_val_if_fail (GST_IS_CAPS (caps), NULL);

  newcaps = gst_caps_new_empty ();
  newcaps->flags = caps->flags;

  for (i = 0; i < caps->structs->len; i++) {
    structure = gst_caps_get_structure (caps, i);
    gst_caps_append_structure (newcaps, gst_structure_copy (structure));
  }

  return newcaps;
}

static void
_gst_caps_free (GstCaps * caps)
{
  GstStructure *structure;
  guint i;

  /* The refcount must be 0, but since we're only called by gst_caps_unref,
   * don't bother testing. */

  for (i = 0; i < caps->structs->len; i++) {
    structure = (GstStructure *) gst_caps_get_structure (caps, i);
    gst_structure_set_parent_refcount (structure, NULL);
    gst_structure_free (structure);
  }
  g_ptr_array_free (caps->structs, TRUE);
#ifdef USE_POISONING
  memset (caps, 0xff, sizeof (GstCaps));
#endif

#ifdef DEBUG_REFCOUNT
  GST_CAT_LOG (GST_CAT_CAPS, "freeing caps %p", caps);
#endif
  FREE_CAPS (caps);
}

/**
 * gst_caps_make_writable:
 * @caps: the #GstCaps to make writable
 *
 * Returns a writable copy of @caps.
 *
 * If there is only one reference count on @caps, the caller must be the owner,
 * and so this function will return the caps object unchanged. If on the other
 * hand there is more than one reference on the object, a new caps object will
 * be returned. The caller's reference on @caps will be removed, and instead the
 * caller will own a reference to the returned object.
 *
 * In short, this function unrefs the caps in the argument and refs the caps
 * that it returns. Don't access the argument after calling this function. See
 * also: gst_caps_ref().
 *
 * Returns: the same #GstCaps object.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_make_writable (GstCaps * caps)
{
  GstCaps *copy;

  g_return_val_if_fail (caps != NULL, NULL);

  /* we are the only instance reffing this caps */
  if (g_atomic_int_get (&caps->refcount) == 1)
    return caps;

  /* else copy */
  copy = gst_caps_copy (caps);
  gst_caps_unref (caps);

  return copy;
}

/**
 * gst_caps_ref:
 * @caps: the #GstCaps to reference
 *
 * Add a reference to a #GstCaps object.
 *
 * From this point on, until the caller calls gst_caps_unref() or
 * gst_caps_make_writable(), it is guaranteed that the caps object will not
 * change. This means its structures won't change, etc. To use a #GstCaps
 * object, you must always have a refcount on it -- either the one made
 * implicitly by gst_caps_new(), or via taking one explicitly with this
 * function.
 *
 * Returns: the same #GstCaps object.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_ref (GstCaps * caps)
{
  g_return_val_if_fail (caps != NULL, NULL);

#ifdef DEBUG_REFCOUNT
  GST_CAT_LOG (GST_CAT_REFCOUNTING, "%p %d->%d", caps,
      GST_CAPS_REFCOUNT_VALUE (caps), GST_CAPS_REFCOUNT_VALUE (caps) + 1);
#endif
  g_return_val_if_fail (GST_CAPS_REFCOUNT_VALUE (caps) > 0, NULL);

  g_atomic_int_inc (&caps->refcount);

  return caps;
}

/**
 * gst_caps_unref:
 * @caps: the #GstCaps to unref
 *
 * Unref a #GstCaps and and free all its structures and the
 * structures' values when the refcount reaches 0.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_caps_unref (GstCaps * caps)
{
  g_return_if_fail (caps != NULL);

#ifdef DEBUG_REFCOUNT
  GST_CAT_LOG (GST_CAT_REFCOUNTING, "%p %d->%d", caps,
      GST_CAPS_REFCOUNT_VALUE (caps), GST_CAPS_REFCOUNT_VALUE (caps) - 1);
#endif

  g_return_if_fail (GST_CAPS_REFCOUNT_VALUE (caps) > 0);

  /* if we ended up with the refcount at zero, free the caps */
  if (G_UNLIKELY (g_atomic_int_dec_and_test (&caps->refcount)))
    _gst_caps_free (caps);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


GType
gst_static_caps_get_type (void)
{
  static GType staticcaps_type = 0;

  if (G_UNLIKELY (staticcaps_type == 0)) {
    staticcaps_type = g_pointer_type_register_static ("GstStaticCaps");
  }
  return staticcaps_type;
}


/**
 * gst_static_caps_get:
 * @static_caps: the #GstStaticCaps to convert
 *
 * Converts a #GstStaticCaps to a #GstCaps.
 *
 * Returns: A pointer to the #GstCaps. Unref after usage. Since the
 * core holds an additional ref to the returned caps,
 * use gst_caps_make_writable() on the returned caps to modify it.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_static_caps_get (GstStaticCaps * static_caps)
{
  GstCaps *caps;

  g_return_val_if_fail (static_caps != NULL, NULL);

  caps = (GstCaps *) static_caps;

  /* refcount is 0 when we need to convert */
  if (G_UNLIKELY (g_atomic_int_get (&caps->refcount) == 0)) {
    const char *string;
    GstCaps temp;

    G_LOCK (static_caps_lock);
    /* check if other thread already updated */
    if (G_UNLIKELY (g_atomic_int_get (&caps->refcount) > 0))
      goto done;

    string = static_caps->string;

    if (G_UNLIKELY (string == NULL))
      goto no_string;

    GST_CAT_LOG (GST_CAT_CAPS, "creating %p", static_caps);

    /* we construct the caps on the stack, then copy over the struct into our
     * real caps, refcount last. We do this because we must leave the refcount
     * of the result caps to 0 so that other threads don't run away with the
     * caps while we are constructing it. */
    temp.type = GST_TYPE_CAPS;
    temp.flags = 0;
    temp.structs = g_ptr_array_new ();

    /* initialize the caps to a refcount of 1 so the caps can be writable for
     * the next statement */
    gst_atomic_int_set (&temp.refcount, 1);

    /* convert to string */
    if (G_UNLIKELY (!gst_caps_from_string_inplace (&temp, string)))
      g_critical ("Could not convert static caps \"%s\"", string);

    /* now copy stuff over to the real caps. */
    caps->type = temp.type;
    caps->flags = temp.flags;
    caps->structs = temp.structs;
    /* and bump the refcount so other threads can now read */
    gst_atomic_int_set (&caps->refcount, 1);

    GST_CAT_LOG (GST_CAT_CAPS, "created %p", static_caps);
  done:
    G_UNLOCK (static_caps_lock);
  }
  /* ref the caps, makes it not writable */
  gst_caps_ref (caps);

  return caps;

  /* ERRORS */
no_string:
  {
    G_UNLOCK (static_caps_lock);
    g_warning ("static caps %p string is NULL", static_caps);
    return NULL;
  }
}

/* manipulation */
static GstStructure *
gst_caps_remove_and_get_structure (GstCaps * caps, guint idx)
{
  /* don't use index_fast, gst_caps_do_simplify relies on the order */
  GstStructure *s = g_ptr_array_remove_index (caps->structs, idx);

  gst_structure_set_parent_refcount (s, NULL);
  return s;
}

static gboolean
gst_structure_is_equal_foreach (GQuark field_id, const GValue * val2,
    gpointer data)
{
  GstStructure *struct1 = (GstStructure *) data;
  const GValue *val1 = gst_structure_id_get_value (struct1, field_id);

  if (val1 == NULL)
    return FALSE;
  if (gst_value_compare (val1, val2) == GST_VALUE_EQUAL) {
    return TRUE;
  }

  return FALSE;
}

static gboolean
gst_caps_structure_is_subset_field (GQuark field_id, const GValue * value,
    gpointer user_data)
{
  GstStructure *subtract_from = user_data;
  GValue subtraction = { 0, };
  const GValue *other;
  gint res;

  other = gst_structure_id_get_value (subtract_from, field_id);
  if (!other) {
    /* field is missing in one set */
    return FALSE;
  }
  /*
   * [1,2] - 1 = 2
   * 1 - [1,2] = ???
   */
  if (!gst_value_subtract (&subtraction, other, value)) {
    /* empty result -> values are the same, or first was a value and
     * second was a list
     * verify that result is empty by swapping args */
    if (!gst_value_subtract (&subtraction, value, other)) {
      return TRUE;
    }
    g_value_unset (&subtraction);
    return FALSE;
  }

  res = gst_value_compare (&subtraction, other);
  g_value_unset (&subtraction);

  if (res == GST_VALUE_EQUAL) {
    /* value was empty ? */
    return FALSE;
  } else {
    return TRUE;
  }
}

static gboolean
gst_caps_structure_is_subset (const GstStructure * minuend,
    const GstStructure * subtrahend)
{
  if ((minuend->name != subtrahend->name) ||
      (gst_structure_n_fields (minuend) !=
          gst_structure_n_fields (subtrahend))) {
    return FALSE;
  }

  return gst_structure_foreach ((GstStructure *) subtrahend,
      gst_caps_structure_is_subset_field, (gpointer) minuend);
}

/**
 * gst_caps_append:
 * @caps1: the #GstCaps that will be appended to
 * @caps2: the #GstCaps to append
 *
 * Appends the structures contained in @caps2 to @caps1. The structures in
 * @caps2 are not copied -- they are transferred to @caps1, and then @caps2 is
 * freed. If either caps is ANY, the resulting caps will be ANY.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_caps_append (GstCaps * caps1, GstCaps * caps2)
{
  GstStructure *structure;
  int i;

  g_return_if_fail (GST_IS_CAPS (caps1));
  g_return_if_fail (GST_IS_CAPS (caps2));
  g_return_if_fail (IS_WRITABLE (caps1));
  g_return_if_fail (IS_WRITABLE (caps2));

#ifdef USE_POISONING
  CAPS_POISON (caps2);
#endif
  if (gst_caps_is_any (caps1) || gst_caps_is_any (caps2)) {
    /* FIXME: this leaks */
    caps1->flags |= GST_CAPS_FLAGS_ANY;
    for (i = caps2->structs->len - 1; i >= 0; i--) {
      structure = gst_caps_remove_and_get_structure (caps2, i);
      gst_structure_free (structure);
    }
  } else {
    int len = caps2->structs->len;

    for (i = 0; i < len; i++) {
      structure = gst_caps_remove_and_get_structure (caps2, 0);
      gst_caps_append_structure (caps1, structure);
    }
  }
  gst_caps_unref (caps2);       /* guaranteed to free it */
}

/**
 * gst_caps_merge:
 * @caps1: the #GstCaps that will take the new entries
 * @caps2: the #GstCaps to merge in
 *
 * Appends the structures contained in @caps2 to @caps1 if they are not yet
 * expressed by @caps1. The structures in @caps2 are not copied -- they are
 * transferred to @caps1, and then @caps2 is freed.
 * If either caps is ANY, the resulting caps will be ANY.
 *
 * Since: 0.10.10
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_caps_merge (GstCaps * caps1, GstCaps * caps2)
{
  GstStructure *structure;
  int i;

  g_return_if_fail (GST_IS_CAPS (caps1));
  g_return_if_fail (GST_IS_CAPS (caps2));
  g_return_if_fail (IS_WRITABLE (caps1));
  g_return_if_fail (IS_WRITABLE (caps2));

#ifdef USE_POISONING
  CAPS_POISON (caps2);
#endif
  if (gst_caps_is_any (caps1)) {
    for (i = caps2->structs->len - 1; i >= 0; i--) {
      structure = gst_caps_remove_and_get_structure (caps2, i);
      gst_structure_free (structure);
    }
  } else if (gst_caps_is_any (caps2)) {
    caps1->flags |= GST_CAPS_FLAGS_ANY;
    for (i = caps1->structs->len - 1; i >= 0; i--) {
      structure = gst_caps_remove_and_get_structure (caps1, i);
      gst_structure_free (structure);
    }
  } else {
    int len = caps2->structs->len;

    for (i = 0; i < len; i++) {
      structure = gst_caps_remove_and_get_structure (caps2, 0);
      gst_caps_merge_structure (caps1, structure);
    }
    /* this is too naive
       GstCaps *com = gst_caps_intersect (caps1, caps2);
       GstCaps *add = gst_caps_subtract (caps2, com);

       GST_DEBUG ("common : %d", gst_caps_get_size (com));
       GST_DEBUG ("adding : %d", gst_caps_get_size (add));
       gst_caps_append (caps1, add);
       gst_caps_unref (com);
     */
  }
  gst_caps_unref (caps2);       /* guaranteed to free it */
}

/**
 * gst_caps_append_structure:
 * @caps: the #GstCaps that will be appended to
 * @structure: the #GstStructure to append
 *
 * Appends @structure to @caps.  The structure is not copied; @caps
 * becomes the owner of @structure.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_caps_append_structure (GstCaps * caps, GstStructure * structure)
{
  g_return_if_fail (GST_IS_CAPS (caps));
  g_return_if_fail (IS_WRITABLE (caps));

  if (G_LIKELY (structure)) {
    g_return_if_fail (structure->parent_refcount == NULL);
#if 0
#ifdef USE_POISONING
    STRUCTURE_POISON (structure);
#endif
#endif
    gst_structure_set_parent_refcount (structure, &caps->refcount);
    g_ptr_array_add (caps->structs, structure);
  }
}

/**
 * gst_caps_remove_structure:
 * @caps: the #GstCaps to remove from
 * @idx: Index of the structure to remove
 *
 * removes the stucture with the given index from the list of structures
 * contained in @caps.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_caps_remove_structure (GstCaps * caps, guint idx)
{
  GstStructure *structure;

  g_return_if_fail (caps != NULL);
  g_return_if_fail (idx <= gst_caps_get_size (caps));
  g_return_if_fail (IS_WRITABLE (caps));

  structure = gst_caps_remove_and_get_structure (caps, idx);
  gst_structure_free (structure);
}

/**
 * gst_caps_merge_structure:
 * @caps: the #GstCaps that will the the new structure
 * @structure: the #GstStructure to merge
 *
 * Appends @structure to @caps if its not already expressed by @caps.  The
 * structure is not copied; @caps becomes the owner of @structure.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_caps_merge_structure (GstCaps * caps, GstStructure * structure)
{
  g_return_if_fail (GST_IS_CAPS (caps));
  g_return_if_fail (IS_WRITABLE (caps));

  if (G_LIKELY (structure)) {
    GstStructure *structure1;
    int i;
    gboolean unique = TRUE;

    g_return_if_fail (structure->parent_refcount == NULL);
#if 0
#ifdef USE_POISONING
    STRUCTURE_POISON (structure);
#endif
#endif
    /* check each structure */
    for (i = caps->structs->len - 1; i >= 0; i--) {
      structure1 = gst_caps_get_structure (caps, i);
      /* if structure is a subset of structure1, then skip it */
      if (gst_caps_structure_is_subset (structure1, structure)) {
        unique = FALSE;
        break;
      }
    }
    if (unique) {
      gst_structure_set_parent_refcount (structure, &caps->refcount);
      g_ptr_array_add (caps->structs, structure);
    } else {
      gst_structure_free (structure);
    }
  }
}


/**
 * gst_caps_get_size:
 * @caps: a #GstCaps
 *
 * Gets the number of structures contained in @caps.
 *
 * Returns: the number of structures that @caps contains
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

guint
gst_caps_get_size (const GstCaps * caps)
{
  g_return_val_if_fail (GST_IS_CAPS (caps), 0);

  return caps->structs->len;
}

/**
 * gst_caps_get_structure:
 * @caps: a #GstCaps
 * @index: the index of the structure
 *
 * Finds the structure in @caps that has the index @index, and
 * returns it.
 *
 * WARNING: This function takes a const GstCaps *, but returns a
 * non-const GstStructure *.  This is for programming convenience --
 * the caller should be aware that structures inside a constant
 * #GstCaps should not be modified.
 *
 * Returns: a pointer to the #GstStructure corresponding to @index
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstStructure *
gst_caps_get_structure (const GstCaps * caps, guint index)
{
  g_return_val_if_fail (GST_IS_CAPS (caps), NULL);
  g_return_val_if_fail (index < caps->structs->len, NULL);

  return g_ptr_array_index (caps->structs, index);
}

/**
 * gst_caps_copy_nth:
 * @caps: the #GstCaps to copy
 * @nth: the nth structure to copy
 *
 * Creates a new #GstCaps and appends a copy of the nth structure
 * contained in @caps.
 *
 * Returns: the new #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_copy_nth (const GstCaps * caps, guint nth)
{
  GstCaps *newcaps;
  GstStructure *structure;

  g_return_val_if_fail (GST_IS_CAPS (caps), NULL);

  newcaps = gst_caps_new_empty ();
  newcaps->flags = caps->flags;

  if (caps->structs->len > nth) {
    structure = gst_caps_get_structure (caps, nth);
    gst_caps_append_structure (newcaps, gst_structure_copy (structure));
  }

  return newcaps;
}

/**
 * gst_caps_truncate:
 * @caps: the #GstCaps to truncate
 *
 * Destructively discard all but the first structure from @caps. Useful when
 * fixating. @caps must be writable.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_caps_truncate (GstCaps * caps)
{
  gint i;

  g_return_if_fail (GST_IS_CAPS (caps));
  g_return_if_fail (IS_WRITABLE (caps));

  i = caps->structs->len - 1;

  while (i > 0)
    gst_caps_remove_structure (caps, i--);
}

/**
 * gst_caps_set_simple:
 * @caps: the #GstCaps to set
 * @field: first field to set
 * @...: additional parameters
 *
 * Sets fields in a simple #GstCaps.  A simple #GstCaps is one that
 * only has one structure.  The arguments must be passed in the same
 * manner as gst_structure_set(), and be NULL-terminated.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_caps_set_simple (GstCaps * caps, char *field, ...)
{
  GstStructure *structure;
  va_list var_args;

  g_return_if_fail (GST_IS_CAPS (caps));
  g_return_if_fail (caps->structs->len == 1);
  g_return_if_fail (IS_WRITABLE (caps));

  structure = gst_caps_get_structure (caps, 0);

  va_start (var_args, field);
  gst_structure_set_valist (structure, field, var_args);
  va_end (var_args);
}

/**
 * gst_caps_set_simple_valist:
 * @caps: the #GstCaps to copy
 * @field: first field to set
 * @varargs: additional parameters
 *
 * Sets fields in a simple #GstCaps.  A simple #GstCaps is one that
 * only has one structure.  The arguments must be passed in the same
 * manner as gst_structure_set(), and be NULL-terminated.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_caps_set_simple_valist (GstCaps * caps, char *field, va_list varargs)
{
  GstStructure *structure;

  g_return_if_fail (GST_IS_CAPS (caps));
  g_return_if_fail (caps->structs->len == 1);
  g_return_if_fail (IS_WRITABLE (caps));

  structure = gst_caps_get_structure (caps, 0);

  gst_structure_set_valist (structure, field, varargs);
}

/* tests */

/**
 * gst_caps_is_any:
 * @caps: the #GstCaps to test
 *
 * Determines if @caps represents any media format.
 *
 * Returns: TRUE if @caps represents any format.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_caps_is_any (const GstCaps * caps)
{
  g_return_val_if_fail (GST_IS_CAPS (caps), FALSE);

  return (caps->flags & GST_CAPS_FLAGS_ANY);
}

/**
 * gst_caps_is_empty:
 * @caps: the #GstCaps to test
 *
 * Determines if @caps represents no media formats.
 *
 * Returns: TRUE if @caps represents no formats.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_caps_is_empty (const GstCaps * caps)
{
  g_return_val_if_fail (GST_IS_CAPS (caps), FALSE);

  if (caps->flags & GST_CAPS_FLAGS_ANY)
    return FALSE;

  return (caps->structs == NULL) || (caps->structs->len == 0);
}

static gboolean
gst_caps_is_fixed_foreach (GQuark field_id, const GValue * value,
    gpointer unused)
{
  return gst_value_is_fixed (value);
}

/**
 * gst_caps_is_fixed:
 * @caps: the #GstCaps to test
 *
 * Fixed #GstCaps describe exactly one format, that is, they have exactly
 * one structure, and each field in the structure describes a fixed type.
 * Examples of non-fixed types are GST_TYPE_INT_RANGE and GST_TYPE_LIST.
 *
 * Returns: TRUE if @caps is fixed
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_caps_is_fixed (const GstCaps * caps)
{
  GstStructure *structure;

  g_return_val_if_fail (GST_IS_CAPS (caps), FALSE);

  if (caps->structs->len != 1)
    return FALSE;

  structure = gst_caps_get_structure (caps, 0);

  return gst_structure_foreach (structure, gst_caps_is_fixed_foreach, NULL);
}

/**
 * gst_caps_is_equal_fixed:
 * @caps1: the #GstCaps to test
 * @caps2: the #GstCaps to test
 *
 * Tests if two #GstCaps are equal.  This function only works on fixed
 * #GstCaps.
 *
 * Returns: TRUE if the arguments represent the same format
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_caps_is_equal_fixed (const GstCaps * caps1, const GstCaps * caps2)
{
  GstStructure *struct1, *struct2;

  g_return_val_if_fail (gst_caps_is_fixed (caps1), FALSE);
  g_return_val_if_fail (gst_caps_is_fixed (caps2), FALSE);

  struct1 = gst_caps_get_structure (caps1, 0);
  struct2 = gst_caps_get_structure (caps2, 0);

  if (struct1->name != struct2->name) {
    return FALSE;
  }
  if (struct1->fields->len != struct2->fields->len) {
    return FALSE;
  }

  return gst_structure_foreach (struct1, gst_structure_is_equal_foreach,
      struct2);
}

/**
 * gst_caps_is_always_compatible:
 * @caps1: the #GstCaps to test
 * @caps2: the #GstCaps to test
 *
 * A given #GstCaps structure is always compatible with another if
 * every media format that is in the first is also contained in the
 * second.  That is, @caps1 is a subset of @caps2.
 *
 * Returns: TRUE if @caps1 is a subset of @caps2.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_caps_is_always_compatible (const GstCaps * caps1, const GstCaps * caps2)
{
  g_return_val_if_fail (GST_IS_CAPS (caps1), FALSE);
  g_return_val_if_fail (GST_IS_CAPS (caps2), FALSE);

  return gst_caps_is_subset (caps1, caps2);
}

/**
 * gst_caps_is_subset:
 * @subset: a #GstCaps
 * @superset: a potentially greater #GstCaps
 *
 * Checks if all caps represented by @subset are also represented by @superset.
 * <note>This function does not work reliably if optional properties for caps
 * are included on one caps and omitted on the other.</note>
 *
 * Returns: %TRUE if @subset is a subset of @superset
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_caps_is_subset (const GstCaps * subset, const GstCaps * superset)
{
  GstCaps *caps;
  gboolean ret;

  g_return_val_if_fail (subset != NULL, FALSE);
  g_return_val_if_fail (superset != NULL, FALSE);

  if (gst_caps_is_empty (subset) || gst_caps_is_any (superset))
    return TRUE;
  if (gst_caps_is_any (subset) || gst_caps_is_empty (superset))
    return FALSE;

  caps = gst_caps_subtract (subset, superset);
  ret = gst_caps_is_empty (caps);
  gst_caps_unref (caps);
  return ret;
}

/**
 * gst_caps_is_equal:
 * @caps1: a #GstCaps
 * @caps2: another #GstCaps
 *
 * Checks if the given caps represent the same set of caps.
 * <note>This function does not work reliably if optional properties for caps
 * are included on one caps and omitted on the other.</note>
 *
 * This function deals correctly with passing NULL for any of the caps.
 *
 * Returns: TRUE if both caps are equal.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_caps_is_equal (const GstCaps * caps1, const GstCaps * caps2)
{
  /* NULL <-> NULL is allowed here */
  if (caps1 == caps2)
    return TRUE;

  /* one of them NULL => they are different (can't be both NULL because
   * we checked that above) */
  if (caps1 == NULL || caps2 == NULL)
    return FALSE;

  if (gst_caps_is_fixed (caps1) && gst_caps_is_fixed (caps2))
    return gst_caps_is_equal_fixed (caps1, caps2);

  return gst_caps_is_subset (caps1, caps2) && gst_caps_is_subset (caps2, caps1);
}

typedef struct
{
  GstStructure *dest;
  const GstStructure *intersect;
  gboolean first_run;
}
IntersectData;

static gboolean
gst_caps_structure_intersect_field (GQuark id, const GValue * val1,
    gpointer data)
{
  IntersectData *idata = (IntersectData *) data;
  GValue dest_value = { 0 };
  const GValue *val2 = gst_structure_id_get_value (idata->intersect, id);

  if (val2 == NULL) {
    gst_structure_id_set_value (idata->dest, id, val1);
  } else if (idata->first_run) {
    if (gst_value_intersect (&dest_value, val1, val2)) {
      gst_structure_id_set_value (idata->dest, id, &dest_value);
      g_value_unset (&dest_value);
    } else {
      return FALSE;
    }
  }

  return TRUE;
}

static GstStructure *
gst_caps_structure_intersect (const GstStructure * struct1,
    const GstStructure * struct2)
{
  IntersectData data;

  g_return_val_if_fail (struct1 != NULL, NULL);
  g_return_val_if_fail (struct2 != NULL, NULL);

  if (struct1->name != struct2->name)
    return NULL;

  data.dest = gst_structure_id_empty_new (struct1->name);
  data.intersect = struct2;
  data.first_run = TRUE;
  if (!gst_structure_foreach ((GstStructure *) struct1,
          gst_caps_structure_intersect_field, &data))
    goto error;

  data.intersect = struct1;
  data.first_run = FALSE;
  if (!gst_structure_foreach ((GstStructure *) struct2,
          gst_caps_structure_intersect_field, &data))
    goto error;

  return data.dest;

error:
  gst_structure_free (data.dest);
  return NULL;
}

#if 0
static GstStructure *
gst_caps_structure_union (const GstStructure * struct1,
    const GstStructure * struct2)
{
  int i;
  GstStructure *dest;
  const GstStructureField *field1;
  const GstStructureField *field2;
  int ret;

  /* FIXME this doesn't actually work */

  if (struct1->name != struct2->name)
    return NULL;

  dest = gst_structure_id_empty_new (struct1->name);

  for (i = 0; i < struct1->fields->len; i++) {
    GValue dest_value = { 0 };

    field1 = GST_STRUCTURE_FIELD (struct1, i);
    field2 = gst_structure_id_get_field (struct2, field1->name);

    if (field2 == NULL) {
      continue;
    } else {
      if (gst_value_union (&dest_value, &field1->value, &field2->value)) {
        gst_structure_set_value (dest, g_quark_to_string (field1->name),
            &dest_value);
      } else {
        ret = gst_value_compare (&field1->value, &field2->value);
      }
    }
  }

  return dest;
}
#endif

/* operations */

/**
 * gst_caps_intersect:
 * @caps1: a #GstCaps to intersect
 * @caps2: a #GstCaps to intersect
 *
 * Creates a new #GstCaps that contains all the formats that are common
 * to both @caps1 and @caps2.
 *
 * Returns: the new #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_intersect (const GstCaps * caps1, const GstCaps * caps2)
{
  guint64 i;                    /* index can be up to 2 * G_MAX_UINT */
  guint j, k;

  GstStructure *struct1;
  GstStructure *struct2;
  GstCaps *dest;
  GstStructure *istruct;

  g_return_val_if_fail (GST_IS_CAPS (caps1), NULL);
  g_return_val_if_fail (GST_IS_CAPS (caps2), NULL);

  /* caps are exactly the same pointers, just copy one caps */
  if (caps1 == caps2)
    return gst_caps_copy (caps1);

  /* empty caps on either side, return empty */
  if (gst_caps_is_empty (caps1) || gst_caps_is_empty (caps2))
    return gst_caps_new_empty ();

  /* one of the caps is any, just copy the other caps */
  if (gst_caps_is_any (caps1))
    return gst_caps_copy (caps2);
  if (gst_caps_is_any (caps2))
    return gst_caps_copy (caps1);

  dest = gst_caps_new_empty ();

  /* run zigzag on top line then right line, this preserves the caps order
   * much better than a simple loop.
   *
   * This algorithm zigzags over the caps structures as demonstrated in
   * the folowing matrix:
   *
   *          caps1
   *       +-------------
   *       | 1  2  4  7
   * caps2 | 3  5  8 10
   *       | 6  9 11 12
   *
   * First we iterate over the caps1 structures (top line) intersecting
   * the structures diagonally down, then we iterate over the caps2
   * structures.
   */
  for (i = 0; i < caps1->structs->len + caps2->structs->len - 1; i++) {
    /* caps1 index goes from 0 to caps1->structs->len-1 */
    j = MIN (i, caps1->structs->len - 1);
    /* caps2 index stays 0 until i reaches caps1->structs->len, then it counts
     * up from 1 to caps2->structs->len - 1 */
    k = MAX (0, i - j);

    /* now run the diagonal line, end condition is the left or bottom
     * border */
    while (k < caps2->structs->len) {
      struct1 = gst_caps_get_structure (caps1, j);
      struct2 = gst_caps_get_structure (caps2, k);

      istruct = gst_caps_structure_intersect (struct1, struct2);

      gst_caps_append_structure (dest, istruct);
      /* move down left */
      k++;
      if (j == 0)
        break;                  /* so we don't roll back to G_MAXUINT */
      j--;
    }
  }
  return dest;
}

typedef struct
{
  const GstStructure *subtract_from;
  GSList *put_into;
}
SubtractionEntry;


static gboolean
gst_caps_structure_subtract_field (GQuark field_id, const GValue * value,
    gpointer user_data)
{
  SubtractionEntry *e = user_data;
  GValue subtraction = { 0, };
  const GValue *other;
  GstStructure *structure;

  other = gst_structure_id_get_value (e->subtract_from, field_id);
  if (!other) {
    return FALSE;
  }
  if (!gst_value_subtract (&subtraction, other, value))
    return TRUE;
  if (gst_value_compare (&subtraction, other) == GST_VALUE_EQUAL) {
    g_value_unset (&subtraction);
    return FALSE;
  } else {
    structure = gst_structure_copy (e->subtract_from);
    gst_structure_id_set_value (structure, field_id, &subtraction);
    g_value_unset (&subtraction);
    e->put_into = g_slist_prepend (e->put_into, structure);
    return TRUE;
  }
}

static gboolean
gst_caps_structure_subtract (GSList ** into, const GstStructure * minuend,
    const GstStructure * subtrahend)
{
  SubtractionEntry e;
  gboolean ret;

  e.subtract_from = minuend;
  e.put_into = NULL;

  ret = gst_structure_foreach ((GstStructure *) subtrahend,
      gst_caps_structure_subtract_field, &e);
  if (ret) {
    *into = e.put_into;
  } else {
    GSList *walk;

    for (walk = e.put_into; walk; walk = g_slist_next (walk)) {
      gst_structure_free (walk->data);
    }
    g_slist_free (e.put_into);
  }
  return ret;
}

/**
 * gst_caps_subtract:
 * @minuend: #GstCaps to substract from
 * @subtrahend: #GstCaps to substract
 *
 * Subtracts the @subtrahend from the @minuend.
 * <note>This function does not work reliably if optional properties for caps
 * are included on one caps and omitted on the other.</note>
 *
 * Returns: the resulting caps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_subtract (const GstCaps * minuend, const GstCaps * subtrahend)
{
  guint i, j;
  GstStructure *min;
  GstStructure *sub;
  GstCaps *dest = NULL, *src;

  g_return_val_if_fail (minuend != NULL, NULL);
  g_return_val_if_fail (subtrahend != NULL, NULL);

  if (gst_caps_is_empty (minuend) || gst_caps_is_any (subtrahend)) {
    return gst_caps_new_empty ();
  }
  if (gst_caps_is_empty (subtrahend))
    return gst_caps_copy (minuend);

  /* FIXME: Do we want this here or above?
     The reason we need this is that there is no definition about what
     ANY means for specific types, so it's not possible to reduce ANY partially
     You can only remove everything or nothing and that is done above.
     Note: there's a test that checks this behaviour. */
  g_return_val_if_fail (!gst_caps_is_any (minuend), NULL);
  g_assert (subtrahend->structs->len > 0);

  src = gst_caps_copy (minuend);
  for (i = 0; i < subtrahend->structs->len; i++) {
    sub = gst_caps_get_structure (subtrahend, i);
    if (dest) {
      gst_caps_unref (src);
      src = dest;
    }
    dest = gst_caps_new_empty ();
    for (j = 0; j < src->structs->len; j++) {
      min = gst_caps_get_structure (src, j);
      if (gst_structure_get_name_id (min) == gst_structure_get_name_id (sub)) {
        GSList *list;

        if (gst_caps_structure_subtract (&list, min, sub)) {
          GSList *walk;

          for (walk = list; walk; walk = g_slist_next (walk)) {
            gst_caps_append_structure (dest, (GstStructure *) walk->data);
          }
          g_slist_free (list);
        } else {
          gst_caps_append_structure (dest, gst_structure_copy (min));
        }
      } else {
        gst_caps_append_structure (dest, gst_structure_copy (min));
      }
    }
    if (gst_caps_is_empty (dest)) {
      gst_caps_unref (src);
      return dest;
    }
  }

  gst_caps_unref (src);
  gst_caps_do_simplify (dest);
  return dest;
}

/**
 * gst_caps_union:
 * @caps1: a #GstCaps to union
 * @caps2: a #GstCaps to union
 *
 * Creates a new #GstCaps that contains all the formats that are in
 * either @caps1 and @caps2.
 *
 * Returns: the new #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_union (const GstCaps * caps1, const GstCaps * caps2)
{
  GstCaps *dest1;
  GstCaps *dest2;

  if (gst_caps_is_any (caps1) || gst_caps_is_any (caps2))
    return gst_caps_new_any ();

  dest1 = gst_caps_copy (caps1);
  dest2 = gst_caps_copy (caps2);
  gst_caps_append (dest1, dest2);

  gst_caps_do_simplify (dest1);
  return dest1;
}

typedef struct _NormalizeForeach
{
  GstCaps *caps;
  GstStructure *structure;
}
NormalizeForeach;

static gboolean
gst_caps_normalize_foreach (GQuark field_id, const GValue * value, gpointer ptr)
{
  NormalizeForeach *nf = (NormalizeForeach *) ptr;
  GValue val = { 0 };
  guint i;

  if (G_VALUE_TYPE (value) == GST_TYPE_LIST) {
    for (i = 1; i < gst_value_list_get_size (value); i++) {
      const GValue *v = gst_value_list_get_value (value, i);
      GstStructure *structure = gst_structure_copy (nf->structure);

      gst_structure_id_set_value (structure, field_id, v);
      gst_caps_append_structure (nf->caps, structure);
    }

    gst_value_init_and_copy (&val, gst_value_list_get_value (value, 0));
    gst_structure_id_set_value (nf->structure, field_id, &val);
    g_value_unset (&val);

    return FALSE;
  }
  return TRUE;
}

/**
 * gst_caps_normalize:
 * @caps: a #GstCaps to normalize
 *
 * Creates a new #GstCaps that represents the same set of formats as
 * @caps, but contains no lists.  Each list is expanded into separate
 * @GstStructures.
 *
 * Returns: the new #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_normalize (const GstCaps * caps)
{
  NormalizeForeach nf;
  GstCaps *newcaps;
  guint i;

  g_return_val_if_fail (GST_IS_CAPS (caps), NULL);

  newcaps = gst_caps_copy (caps);
  nf.caps = newcaps;

  for (i = 0; i < newcaps->structs->len; i++) {
    nf.structure = gst_caps_get_structure (newcaps, i);

    while (!gst_structure_foreach (nf.structure,
            gst_caps_normalize_foreach, &nf));
  }

  return newcaps;
}

static gint
gst_caps_compare_structures (gconstpointer one, gconstpointer two)
{
  gint ret;
  const GstStructure *struct1 = *((const GstStructure **) one);
  const GstStructure *struct2 = *((const GstStructure **) two);

  /* FIXME: this orders alphabetically, but ordering the quarks might be faster
     So what's the best way? */
  ret = strcmp (gst_structure_get_name (struct1),
      gst_structure_get_name (struct2));
  if (ret)
    return ret;

  return gst_structure_n_fields (struct2) - gst_structure_n_fields (struct1);
}

typedef struct
{
  GQuark name;
  GValue value;
  GstStructure *compare;
}
UnionField;

static gboolean
gst_caps_structure_figure_out_union (GQuark field_id, const GValue * value,
    gpointer user_data)
{
  UnionField *u = user_data;
  const GValue *val = gst_structure_id_get_value (u->compare, field_id);

  if (!val) {
    if (u->name)
      g_value_unset (&u->value);
    return FALSE;
  }
  if (gst_value_compare (val, value) == GST_VALUE_EQUAL)
    return TRUE;
  if (u->name) {
    g_value_unset (&u->value);
    return FALSE;
  }
  u->name = field_id;
  gst_value_union (&u->value, val, value);
  return TRUE;
}

static gboolean
gst_caps_structure_simplify (GstStructure ** result,
    const GstStructure * simplify, GstStructure * compare)
{
  GSList *list;
  UnionField field = { 0, {0,}, NULL };

  /* try to subtract to get a real subset */
  if (gst_caps_structure_subtract (&list, simplify, compare)) {
    switch (g_slist_length (list)) {
      case 0:
        *result = NULL;
        return TRUE;
      case 1:
        *result = list->data;
        g_slist_free (list);
        return TRUE;
      default:
      {
        GSList *walk;

        for (walk = list; walk; walk = g_slist_next (walk)) {
          gst_structure_free (walk->data);
        }
        g_slist_free (list);
        break;
      }
    }
  }

  /* try to union both structs */
  field.compare = compare;
  if (gst_structure_foreach ((GstStructure *) simplify,
          gst_caps_structure_figure_out_union, &field)) {
    gboolean ret = FALSE;

    /* now we know all of simplify's fields are the same in compare
     * but at most one field: field.name */
    if (G_IS_VALUE (&field.value)) {
      if (gst_structure_n_fields (simplify) == gst_structure_n_fields (compare)) {
        gst_structure_id_set_value (compare, field.name, &field.value);
        *result = NULL;
        ret = TRUE;
      }
      g_value_unset (&field.value);
    } else if (gst_structure_n_fields (simplify) <=
        gst_structure_n_fields (compare)) {
      /* compare is just more specific, will be optimized away later */
      /* FIXME: do this here? */
      GST_LOG ("found a case that will be optimized later.");
    } else {
      gchar *one = gst_structure_to_string (simplify);
      gchar *two = gst_structure_to_string (compare);

      GST_ERROR
          ("caps mismatch: structures %s and %s claim to be possible to unify, but aren't",
          one, two);
      g_free (one);
      g_free (two);
    }
    return ret;
  }

  return FALSE;
}

static void
gst_caps_switch_structures (GstCaps * caps, GstStructure * old,
    GstStructure * new, gint i)
{
  gst_structure_set_parent_refcount (old, NULL);
  gst_structure_free (old);
  gst_structure_set_parent_refcount (new, &caps->refcount);
  g_ptr_array_index (caps->structs, i) = new;
}

/**
 * gst_caps_do_simplify:
 * @caps: a #GstCaps to simplify
 *
 * Modifies the given @caps inplace into a representation that represents the
 * same set of formats, but in a simpler form.  Component structures that are
 * identical are merged.  Component structures that have values that can be
 * merged are also merged.
 *
 * Returns: TRUE, if the caps could be simplified
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gboolean
gst_caps_do_simplify (GstCaps * caps)
{
  GstStructure *simplify, *compare, *result = NULL;
  gint i, j, start;
  gboolean changed = FALSE;

  g_return_val_if_fail (caps != NULL, FALSE);
  g_return_val_if_fail (IS_WRITABLE (caps), FALSE);

  if (gst_caps_get_size (caps) < 2)
    return FALSE;

  g_ptr_array_sort (caps->structs, gst_caps_compare_structures);

  start = caps->structs->len - 1;
  for (i = caps->structs->len - 1; i >= 0; i--) {
    simplify = gst_caps_get_structure (caps, i);
    if (gst_structure_get_name_id (simplify) !=
        gst_structure_get_name_id (gst_caps_get_structure (caps, start)))
      start = i;
    for (j = start; j >= 0; j--) {
      if (j == i)
        continue;
      compare = gst_caps_get_structure (caps, j);
      if (gst_structure_get_name_id (simplify) !=
          gst_structure_get_name_id (compare)) {
        break;
      }
      if (gst_caps_structure_simplify (&result, simplify, compare)) {
        if (result) {
          gst_caps_switch_structures (caps, simplify, result, i);
          simplify = result;
        } else {
          gst_caps_remove_structure (caps, i);
          start--;
          break;
        }
        changed = TRUE;
      }
    }
  }

  if (!changed)
    return FALSE;

  /* gst_caps_do_simplify (caps); */
  return TRUE;
}

#ifndef GST_DISABLE_LOADSAVE
/**
 * gst_caps_save_thyself:
 * @caps: a #GstCaps structure
 * @parent: a XML parent node
 *
 * Serializes a #GstCaps to XML and adds it as a child node of @parent.
 *
 * Returns: a XML node pointer
 */
xmlNodePtr
gst_caps_save_thyself (const GstCaps * caps, xmlNodePtr parent)
{
  char *s = gst_caps_to_string (caps);

  xmlNewChild (parent, NULL, (xmlChar *) "caps", (xmlChar *) s);
  g_free (s);
  return parent;
}

/**
 * gst_caps_load_thyself:
 * @parent: a XML node
 *
 * Creates a #GstCaps from its XML serialization.
 *
 * Returns: a new #GstCaps structure
 */
GstCaps *
gst_caps_load_thyself (xmlNodePtr parent)
{
  if (strcmp ("caps", (char *) parent->name) == 0) {
    return gst_caps_from_string ((gchar *) xmlNodeGetContent (parent));
  }

  return NULL;
}
#endif

/* utility */

/**
 * gst_caps_replace:
 * @caps: a pointer to #GstCaps
 * @newcaps: a #GstCaps to replace *caps
 *
 * Replaces *caps with @newcaps.  Unrefs the #GstCaps in the location
 * pointed to by @caps, if applicable, then modifies @caps to point to
 * @newcaps. An additional ref on @newcaps is taken.
 *
 * This function does not take any locks so you might want to lock
 * the object owning @caps pointer.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_caps_replace (GstCaps ** caps, GstCaps * newcaps)
{
  GstCaps *oldcaps;

  g_return_if_fail (caps != NULL);

  oldcaps = *caps;

  if (newcaps != oldcaps) {
    if (newcaps)
      gst_caps_ref (newcaps);

    *caps = newcaps;

    if (oldcaps)
      gst_caps_unref (oldcaps);
  }
}

/**
 * gst_caps_to_string:
 * @caps: a #GstCaps
 *
 * Converts @caps to a string representation.  This string representation
 * can be converted back to a #GstCaps by gst_caps_from_string().
 *
 * For debugging purposes its easier to do something like this:
 * <programlisting>
 *  GST_LOG ("caps are %" GST_PTR_FORMAT, caps);
 * </programlisting>
 * This prints the caps in human readble form.
 *
 * Returns: a newly allocated string representing @caps.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

gchar *
gst_caps_to_string (const GstCaps * caps)
{
  guint i, slen;
  GString *s;

  /* NOTE:  This function is potentially called by the debug system,
   * so any calls to gst_log() (and GST_DEBUG(), GST_LOG(), etc.)
   * should be careful to avoid recursion.  This includes any functions
   * called by gst_caps_to_string.  In particular, calls should
   * not use the GST_PTR_FORMAT extension.  */

  if (caps == NULL) {
    return g_strdup ("NULL");
  }
  if (gst_caps_is_any (caps)) {
    return g_strdup ("ANY");
  }
  if (gst_caps_is_empty (caps)) {
    return g_strdup ("EMPTY");
  }

  /* estimate a rough string length to avoid unnecessary reallocs in GString */
  slen = 0;
  for (i = 0; i < caps->structs->len; i++) {
    slen += STRUCTURE_ESTIMATED_STRING_LEN (gst_caps_get_structure (caps, i));
  }

  s = g_string_sized_new (slen);
  for (i = 0; i < caps->structs->len; i++) {
    GstStructure *structure;

    if (i > 0) {
      /* ';' is now added by gst_structure_to_string */
      g_string_append_c (s, ' ');
    }

    structure = gst_caps_get_structure (caps, i);
    priv_gst_structure_append_to_gstring (structure, s);
  }
  if (s->len && s->str[s->len - 1] == ';') {
    /* remove latest ';' */
    s->str[--s->len] = '\0';
  }
  return g_string_free (s, FALSE);
}

static gboolean
gst_caps_from_string_inplace (GstCaps * caps, const gchar * string)
{
  GstStructure *structure;
  gchar *s;

  g_return_val_if_fail (string, FALSE);
  if (strcmp ("ANY", string) == 0) {
    caps->flags = GST_CAPS_FLAGS_ANY;
    return TRUE;
  }
  if (strcmp ("EMPTY", string) == 0) {
    return TRUE;
  }

  structure = gst_structure_from_string (string, &s);
  if (structure == NULL) {
    return FALSE;
  }
  gst_caps_append_structure (caps, structure);

  do {

    while (g_ascii_isspace (*s))
      s++;
    if (*s == '\0') {
      break;
    }
    structure = gst_structure_from_string (s, &s);
    if (structure == NULL) {
      return FALSE;
    }
    gst_caps_append_structure (caps, structure);

  } while (TRUE);

  return TRUE;
}

/**
 * gst_caps_from_string:
 * @string: a string to convert to #GstCaps
 *
 * Converts @caps from a string representation.
 *
 * Returns: a newly allocated #GstCaps
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstCaps *
gst_caps_from_string (const gchar * string)
{
  GstCaps *caps;

  caps = gst_caps_new_empty ();
  if (gst_caps_from_string_inplace (caps, string)) {
    return caps;
  } else {
    gst_caps_unref (caps);
    return NULL;
  }
}

static void
gst_caps_transform_to_string (const GValue * src_value, GValue * dest_value)
{
  g_return_if_fail (G_IS_VALUE (src_value));
  g_return_if_fail (G_IS_VALUE (dest_value));
  g_return_if_fail (G_VALUE_HOLDS (src_value, GST_TYPE_CAPS));
  g_return_if_fail (G_VALUE_HOLDS (dest_value, G_TYPE_STRING)
      || G_VALUE_HOLDS (dest_value, G_TYPE_POINTER));

  dest_value->data[0].v_pointer =
      gst_caps_to_string (src_value->data[0].v_pointer);
}

static GstCaps *
gst_caps_copy_conditional (GstCaps * src)
{
  if (src) {
    return gst_caps_ref (src);
  } else {
    return NULL;
  }
}
