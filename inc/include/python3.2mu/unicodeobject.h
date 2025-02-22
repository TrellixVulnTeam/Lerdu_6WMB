#ifndef Py_UNICODEOBJECT_H
#define Py_UNICODEOBJECT_H

#include <stdarg.h>

/*

Unicode implementation based on original code by Fredrik Lundh,
modified by Marc-Andre Lemburg (mal@lemburg.com) according to the
Unicode Integration Proposal. (See
http://www.egenix.com/files/python/unicode-proposal.txt).

Copyright (c) Corporation for National Research Initiatives.


 Original header:
 --------------------------------------------------------------------

 * Yet another Unicode string type for Python.  This type supports the
 * 16-bit Basic Multilingual Plane (BMP) only.
 *
 * Written by Fredrik Lundh, January 1999.
 *
 * Copyright (c) 1999 by Secret Labs AB.
 * Copyright (c) 1999 by Fredrik Lundh.
 *
 * fredrik@pythonware.com
 * http://www.pythonware.com
 *
 * --------------------------------------------------------------------
 * This Unicode String Type is
 *
 * Copyright (c) 1999 by Secret Labs AB
 * Copyright (c) 1999 by Fredrik Lundh
 *
 * By obtaining, using, and/or copying this software and/or its
 * associated documentation, you agree that you have read, understood,
 * and will comply with the following terms and conditions:
 *
 * Permission to use, copy, modify, and distribute this software and its
 * associated documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appears in all
 * copies, and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Secret Labs
 * AB or the author not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 *
 * SECRET LABS AB AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS.  IN NO EVENT SHALL SECRET LABS AB OR THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * -------------------------------------------------------------------- */

#include <ctype.h>

/* === Internal API ======================================================= */

/* --- Internal Unicode Format -------------------------------------------- */

/* Python 3.x requires unicode */
#define Py_USING_UNICODE

/* FIXME: MvL's new implementation assumes that Py_UNICODE_SIZE is
   properly set, but the default rules below doesn't set it.  I'll
   sort this out some other day -- fredrik@pythonware.com */

#ifndef Py_UNICODE_SIZE
#error Must define Py_UNICODE_SIZE
#endif

/* Setting Py_UNICODE_WIDE enables UCS-4 storage.  Otherwise, Unicode
   strings are stored as UCS-2 (with limited support for UTF-16) */

#if Py_UNICODE_SIZE >= 4
#define Py_UNICODE_WIDE
#endif

/* Set these flags if the platform has "wchar.h" and the
   wchar_t type is a 16-bit unsigned type */
/* #define HAVE_WCHAR_H */
/* #define HAVE_USABLE_WCHAR_T */

/* Defaults for various platforms */
#ifndef PY_UNICODE_TYPE

/* Windows has a usable wchar_t type (unless we're using UCS-4) */
# if defined(MS_WIN32) && Py_UNICODE_SIZE == 2
#  define HAVE_USABLE_WCHAR_T
#  define PY_UNICODE_TYPE wchar_t
# endif

# if defined(Py_UNICODE_WIDE)
#  define PY_UNICODE_TYPE Py_UCS4
# endif

#endif

/* If the compiler provides a wchar_t type we try to support it
   through the interface functions PyUnicode_FromWideChar(),
   PyUnicode_AsWideChar() and PyUnicode_AsWideCharString(). */

#ifdef HAVE_USABLE_WCHAR_T
# ifndef HAVE_WCHAR_H
#  define HAVE_WCHAR_H
# endif
#endif

#ifdef HAVE_WCHAR_H
/* Work around a cosmetic bug in BSDI 4.x wchar.h; thanks to Thomas Wouters */
# ifdef _HAVE_BSDI
#  include <time.h>
# endif
#  include <wchar.h>
#endif

/*
 * Use this typedef when you need to represent a UTF-16 surrogate pair
 * as single unsigned integer.
 */
#if SIZEOF_INT >= 4
typedef unsigned int Py_UCS4;
#elif SIZEOF_LONG >= 4
typedef unsigned long Py_UCS4;
#endif

/* Py_UNICODE is the native Unicode storage format (code unit) used by
   Python and represents a single Unicode element in the Unicode
   type. */

#ifndef Py_LIMITED_API
typedef PY_UNICODE_TYPE Py_UNICODE;
#endif

/* --- UCS-2/UCS-4 Name Mangling ------------------------------------------ */

/* Unicode API names are mangled to assure that UCS-2 and UCS-4 builds
   produce different external names and thus cause import errors in
   case Python interpreters and extensions with mixed compiled in
   Unicode width assumptions are combined. */

#ifndef Py_UNICODE_WIDE

# define PyUnicode_AsASCIIString PyUnicodeUCS2_AsASCIIString
# define PyUnicode_AsCharmapString PyUnicodeUCS2_AsCharmapString
# define PyUnicode_AsDecodedObject PyUnicodeUCS2_AsDecodedObject
# define PyUnicode_AsDecodedUnicode PyUnicodeUCS2_AsDecodedUnicode
# define PyUnicode_AsEncodedObject PyUnicodeUCS2_AsEncodedObject
# define PyUnicode_AsEncodedString PyUnicodeUCS2_AsEncodedString
# define PyUnicode_AsEncodedUnicode PyUnicodeUCS2_AsEncodedUnicode
# define PyUnicode_AsLatin1String PyUnicodeUCS2_AsLatin1String
# define PyUnicode_AsRawUnicodeEscapeString PyUnicodeUCS2_AsRawUnicodeEscapeString
# define PyUnicode_AsUTF32String PyUnicodeUCS2_AsUTF32String
# define PyUnicode_AsUTF16String PyUnicodeUCS2_AsUTF16String
# define PyUnicode_AsUTF8String PyUnicodeUCS2_AsUTF8String
# define PyUnicode_AsUnicode PyUnicodeUCS2_AsUnicode
# define PyUnicode_AsUnicodeEscapeString PyUnicodeUCS2_AsUnicodeEscapeString
# define PyUnicode_AsWideChar PyUnicodeUCS2_AsWideChar
# define PyUnicode_AsWideCharString PyUnicodeUCS2_AsWideCharString
# define PyUnicode_ClearFreeList PyUnicodeUCS2_ClearFreelist
# define PyUnicode_Compare PyUnicodeUCS2_Compare
# define PyUnicode_CompareWithASCIIString PyUnicodeUCS2_CompareWithASCIIString
# define PyUnicode_Concat PyUnicodeUCS2_Concat
# define PyUnicode_Append PyUnicodeUCS2_Append
# define PyUnicode_AppendAndDel PyUnicodeUCS2_AppendAndDel
# define PyUnicode_Contains PyUnicodeUCS2_Contains
# define PyUnicode_Count PyUnicodeUCS2_Count
# define PyUnicode_Decode PyUnicodeUCS2_Decode
# define PyUnicode_DecodeASCII PyUnicodeUCS2_DecodeASCII
# define PyUnicode_DecodeCharmap PyUnicodeUCS2_DecodeCharmap
# define PyUnicode_DecodeLatin1 PyUnicodeUCS2_DecodeLatin1
# define PyUnicode_DecodeFSDefault PyUnicodeUCS2_DecodeFSDefault
# define PyUnicode_DecodeFSDefaultAndSize PyUnicodeUCS2_DecodeFSDefaultAndSize
# define PyUnicode_DecodeRawUnicodeEscape PyUnicodeUCS2_DecodeRawUnicodeEscape
# define PyUnicode_DecodeUTF32 PyUnicodeUCS2_DecodeUTF32
# define PyUnicode_DecodeUTF32Stateful PyUnicodeUCS2_DecodeUTF32Stateful
# define PyUnicode_DecodeUTF16 PyUnicodeUCS2_DecodeUTF16
# define PyUnicode_DecodeUTF16Stateful PyUnicodeUCS2_DecodeUTF16Stateful
# define PyUnicode_DecodeUTF8 PyUnicodeUCS2_DecodeUTF8
# define PyUnicode_DecodeUTF8Stateful PyUnicodeUCS2_DecodeUTF8Stateful
# define PyUnicode_DecodeUnicodeEscape PyUnicodeUCS2_DecodeUnicodeEscape
# define PyUnicode_Encode PyUnicodeUCS2_Encode
# define PyUnicode_EncodeASCII PyUnicodeUCS2_EncodeASCII
# define PyUnicode_EncodeCharmap PyUnicodeUCS2_EncodeCharmap
# define PyUnicode_EncodeDecimal PyUnicodeUCS2_EncodeDecimal
# define PyUnicode_EncodeLatin1 PyUnicodeUCS2_EncodeLatin1
# define PyUnicode_EncodeRawUnicodeEscape PyUnicodeUCS2_EncodeRawUnicodeEscape
# define PyUnicode_EncodeUTF32 PyUnicodeUCS2_EncodeUTF32
# define PyUnicode_EncodeUTF16 PyUnicodeUCS2_EncodeUTF16
# define PyUnicode_EncodeUTF8 PyUnicodeUCS2_EncodeUTF8
# define PyUnicode_EncodeUnicodeEscape PyUnicodeUCS2_EncodeUnicodeEscape
# define PyUnicode_Find PyUnicodeUCS2_Find
# define PyUnicode_Format PyUnicodeUCS2_Format
# define PyUnicode_FromEncodedObject PyUnicodeUCS2_FromEncodedObject
# define PyUnicode_FromFormat PyUnicodeUCS2_FromFormat
# define PyUnicode_FromFormatV PyUnicodeUCS2_FromFormatV
# define PyUnicode_FromObject PyUnicodeUCS2_FromObject
# define PyUnicode_FromOrdinal PyUnicodeUCS2_FromOrdinal
# define PyUnicode_FromString PyUnicodeUCS2_FromString
# define PyUnicode_FromStringAndSize PyUnicodeUCS2_FromStringAndSize
# define PyUnicode_FromUnicode PyUnicodeUCS2_FromUnicode
# define PyUnicode_FromWideChar PyUnicodeUCS2_FromWideChar
# define PyUnicode_FSConverter PyUnicodeUCS2_FSConverter
# define PyUnicode_FSDecoder PyUnicodeUCS2_FSDecoder
# define PyUnicode_GetDefaultEncoding PyUnicodeUCS2_GetDefaultEncoding
# define PyUnicode_GetMax PyUnicodeUCS2_GetMax
# define PyUnicode_GetSize PyUnicodeUCS2_GetSize
# define PyUnicode_IsIdentifier PyUnicodeUCS2_IsIdentifier
# define PyUnicode_Join PyUnicodeUCS2_Join
# define PyUnicode_Partition PyUnicodeUCS2_Partition
# define PyUnicode_RPartition PyUnicodeUCS2_RPartition
# define PyUnicode_RSplit PyUnicodeUCS2_RSplit
# define PyUnicode_Replace PyUnicodeUCS2_Replace
# define PyUnicode_Resize PyUnicodeUCS2_Resize
# define PyUnicode_RichCompare PyUnicodeUCS2_RichCompare
# define PyUnicode_Split PyUnicodeUCS2_Split
# define PyUnicode_Splitlines PyUnicodeUCS2_Splitlines
# define PyUnicode_Tailmatch PyUnicodeUCS2_Tailmatch
# define PyUnicode_Translate PyUnicodeUCS2_Translate
# define PyUnicode_TranslateCharmap PyUnicodeUCS2_TranslateCharmap
# define _PyUnicode_AsDefaultEncodedString _PyUnicodeUCS2_AsDefaultEncodedString
# define _PyUnicode_Fini _PyUnicodeUCS2_Fini
# define _PyUnicode_Init _PyUnicodeUCS2_Init
# define PyUnicode_strdup PyUnicodeUCS2_strdup

#else

# define PyUnicode_AsASCIIString PyUnicodeUCS4_AsASCIIString
# define PyUnicode_AsCharmapString PyUnicodeUCS4_AsCharmapString
# define PyUnicode_AsDecodedObject PyUnicodeUCS4_AsDecodedObject
# define PyUnicode_AsDecodedUnicode PyUnicodeUCS4_AsDecodedUnicode
# define PyUnicode_AsEncodedObject PyUnicodeUCS4_AsEncodedObject
# define PyUnicode_AsEncodedString PyUnicodeUCS4_AsEncodedString
# define PyUnicode_AsEncodedUnicode PyUnicodeUCS4_AsEncodedUnicode
# define PyUnicode_AsLatin1String PyUnicodeUCS4_AsLatin1String
# define PyUnicode_AsRawUnicodeEscapeString PyUnicodeUCS4_AsRawUnicodeEscapeString
# define PyUnicode_AsUTF32String PyUnicodeUCS4_AsUTF32String
# define PyUnicode_AsUTF16String PyUnicodeUCS4_AsUTF16String
# define PyUnicode_AsUTF8String PyUnicodeUCS4_AsUTF8String
# define PyUnicode_AsUnicode PyUnicodeUCS4_AsUnicode
# define PyUnicode_AsUnicodeEscapeString PyUnicodeUCS4_AsUnicodeEscapeString
# define PyUnicode_AsWideChar PyUnicodeUCS4_AsWideChar
# define PyUnicode_AsWideCharString PyUnicodeUCS4_AsWideCharString
# define PyUnicode_ClearFreeList PyUnicodeUCS4_ClearFreelist
# define PyUnicode_Compare PyUnicodeUCS4_Compare
# define PyUnicode_CompareWithASCIIString PyUnicodeUCS4_CompareWithASCIIString
# define PyUnicode_Concat PyUnicodeUCS4_Concat
# define PyUnicode_Append PyUnicodeUCS4_Append
# define PyUnicode_AppendAndDel PyUnicodeUCS4_AppendAndDel
# define PyUnicode_Contains PyUnicodeUCS4_Contains
# define PyUnicode_Count PyUnicodeUCS4_Count
# define PyUnicode_Decode PyUnicodeUCS4_Decode
# define PyUnicode_DecodeASCII PyUnicodeUCS4_DecodeASCII
# define PyUnicode_DecodeCharmap PyUnicodeUCS4_DecodeCharmap
# define PyUnicode_DecodeLatin1 PyUnicodeUCS4_DecodeLatin1
# define PyUnicode_DecodeFSDefault PyUnicodeUCS4_DecodeFSDefault
# define PyUnicode_DecodeFSDefaultAndSize PyUnicodeUCS4_DecodeFSDefaultAndSize
# define PyUnicode_DecodeRawUnicodeEscape PyUnicodeUCS4_DecodeRawUnicodeEscape
# define PyUnicode_DecodeUTF32 PyUnicodeUCS4_DecodeUTF32
# define PyUnicode_DecodeUTF32Stateful PyUnicodeUCS4_DecodeUTF32Stateful
# define PyUnicode_DecodeUTF16 PyUnicodeUCS4_DecodeUTF16
# define PyUnicode_DecodeUTF16Stateful PyUnicodeUCS4_DecodeUTF16Stateful
# define PyUnicode_DecodeUTF8 PyUnicodeUCS4_DecodeUTF8
# define PyUnicode_DecodeUTF8Stateful PyUnicodeUCS4_DecodeUTF8Stateful
# define PyUnicode_DecodeUnicodeEscape PyUnicodeUCS4_DecodeUnicodeEscape
# define PyUnicode_Encode PyUnicodeUCS4_Encode
# define PyUnicode_EncodeASCII PyUnicodeUCS4_EncodeASCII
# define PyUnicode_EncodeCharmap PyUnicodeUCS4_EncodeCharmap
# define PyUnicode_EncodeDecimal PyUnicodeUCS4_EncodeDecimal
# define PyUnicode_EncodeLatin1 PyUnicodeUCS4_EncodeLatin1
# define PyUnicode_EncodeRawUnicodeEscape PyUnicodeUCS4_EncodeRawUnicodeEscape
# define PyUnicode_EncodeUTF32 PyUnicodeUCS4_EncodeUTF32
# define PyUnicode_EncodeUTF16 PyUnicodeUCS4_EncodeUTF16
# define PyUnicode_EncodeUTF8 PyUnicodeUCS4_EncodeUTF8
# define PyUnicode_EncodeUnicodeEscape PyUnicodeUCS4_EncodeUnicodeEscape
# define PyUnicode_Find PyUnicodeUCS4_Find
# define PyUnicode_Format PyUnicodeUCS4_Format
# define PyUnicode_FromEncodedObject PyUnicodeUCS4_FromEncodedObject
# define PyUnicode_FromFormat PyUnicodeUCS4_FromFormat
# define PyUnicode_FromFormatV PyUnicodeUCS4_FromFormatV
# define PyUnicode_FromObject PyUnicodeUCS4_FromObject
# define PyUnicode_FromOrdinal PyUnicodeUCS4_FromOrdinal
# define PyUnicode_FromString PyUnicodeUCS4_FromString
# define PyUnicode_FromStringAndSize PyUnicodeUCS4_FromStringAndSize
# define PyUnicode_FromUnicode PyUnicodeUCS4_FromUnicode
# define PyUnicode_FromWideChar PyUnicodeUCS4_FromWideChar
# define PyUnicode_FSConverter PyUnicodeUCS4_FSConverter
# define PyUnicode_FSDecoder PyUnicodeUCS4_FSDecoder
# define PyUnicode_GetDefaultEncoding PyUnicodeUCS4_GetDefaultEncoding
# define PyUnicode_GetMax PyUnicodeUCS4_GetMax
# define PyUnicode_GetSize PyUnicodeUCS4_GetSize
# define PyUnicode_IsIdentifier PyUnicodeUCS4_IsIdentifier
# define PyUnicode_Join PyUnicodeUCS4_Join
# define PyUnicode_Partition PyUnicodeUCS4_Partition
# define PyUnicode_RPartition PyUnicodeUCS4_RPartition
# define PyUnicode_RSplit PyUnicodeUCS4_RSplit
# define PyUnicode_Replace PyUnicodeUCS4_Replace
# define PyUnicode_Resize PyUnicodeUCS4_Resize
# define PyUnicode_RichCompare PyUnicodeUCS4_RichCompare
# define PyUnicode_Split PyUnicodeUCS4_Split
# define PyUnicode_Splitlines PyUnicodeUCS4_Splitlines
# define PyUnicode_Tailmatch PyUnicodeUCS4_Tailmatch
# define PyUnicode_Translate PyUnicodeUCS4_Translate
# define PyUnicode_TranslateCharmap PyUnicodeUCS4_TranslateCharmap
# define _PyUnicode_AsDefaultEncodedString _PyUnicodeUCS4_AsDefaultEncodedString
# define _PyUnicode_Fini _PyUnicodeUCS4_Fini
# define _PyUnicode_Init _PyUnicodeUCS4_Init
# define PyUnicode_strdup PyUnicodeUCS4_strdup

#endif

/* --- Internal Unicode Operations ---------------------------------------- */

/* Since splitting on whitespace is an important use case, and
   whitespace in most situations is solely ASCII whitespace, we
   optimize for the common case by using a quick look-up table
   _Py_ascii_whitespace (see below) with an inlined check.

 */
#ifndef Py_LIMITED_API
#define Py_UNICODE_ISSPACE(ch) \
    ((ch) < 128U ? _Py_ascii_whitespace[(ch)] : _PyUnicode_IsWhitespace(ch))

#define Py_UNICODE_ISLOWER(ch) _PyUnicode_IsLowercase(ch)
#define Py_UNICODE_ISUPPER(ch) _PyUnicode_IsUppercase(ch)
#define Py_UNICODE_ISTITLE(ch) _PyUnicode_IsTitlecase(ch)
#define Py_UNICODE_ISLINEBREAK(ch) _PyUnicode_IsLinebreak(ch)

#define Py_UNICODE_TOLOWER(ch) _PyUnicode_ToLowercase(ch)
#define Py_UNICODE_TOUPPER(ch) _PyUnicode_ToUppercase(ch)
#define Py_UNICODE_TOTITLE(ch) _PyUnicode_ToTitlecase(ch)

#define Py_UNICODE_ISDECIMAL(ch) _PyUnicode_IsDecimalDigit(ch)
#define Py_UNICODE_ISDIGIT(ch) _PyUnicode_IsDigit(ch)
#define Py_UNICODE_ISNUMERIC(ch) _PyUnicode_IsNumeric(ch)
#define Py_UNICODE_ISPRINTABLE(ch) _PyUnicode_IsPrintable(ch)

#define Py_UNICODE_TODECIMAL(ch) _PyUnicode_ToDecimalDigit(ch)
#define Py_UNICODE_TODIGIT(ch) _PyUnicode_ToDigit(ch)
#define Py_UNICODE_TONUMERIC(ch) _PyUnicode_ToNumeric(ch)

#define Py_UNICODE_ISALPHA(ch) _PyUnicode_IsAlpha(ch)

#define Py_UNICODE_ISALNUM(ch) \
       (Py_UNICODE_ISALPHA(ch) || \
    Py_UNICODE_ISDECIMAL(ch) || \
    Py_UNICODE_ISDIGIT(ch) || \
    Py_UNICODE_ISNUMERIC(ch))

#define Py_UNICODE_COPY(target, source, length)                         \
    Py_MEMCPY((target), (source), (length)*sizeof(Py_UNICODE))

#define Py_UNICODE_FILL(target, value, length) \
    do {Py_ssize_t i_; Py_UNICODE *t_ = (target); Py_UNICODE v_ = (value);\
    for (i_ = 0; i_ < (length); i_++) t_[i_] = v_;\
    } while (0)

/* Check if substring matches at given offset.  The offset must be
   valid, and the substring must not be empty. */

#define Py_UNICODE_MATCH(string, offset, substring) \
    ((*((string)->str + (offset)) == *((substring)->str)) && \
    ((*((string)->str + (offset) + (substring)->length-1) == *((substring)->str + (substring)->length-1))) && \
     !memcmp((string)->str + (offset), (substring)->str, (substring)->length*sizeof(Py_UNICODE)))
#endif /* Py_LIMITED_API */

#ifdef __cplusplus
extern "C" {
#endif

/* --- Unicode Type ------------------------------------------------------- */

#ifndef Py_LIMITED_API
typedef struct {
    PyObject_HEAD
    Py_ssize_t length;          /* Length of raw Unicode data in buffer */
    Py_UNICODE *str;            /* Raw Unicode buffer */
    Py_hash_t hash;             /* Hash value; -1 if not set */
    int state;                  /* != 0 if interned. In this case the two
                                 * references from the dictionary to this object
                                 * are *not* counted in ob_refcnt. */
    PyObject *defenc;           /* (Default) Encoded version as Python
                                   string, or NULL; this is used for
                                   implementing the buffer protocol */
} PyUnicodeObject;
#endif

PyAPI_DATA(PyTypeObject) PyUnicode_Type;
PyAPI_DATA(PyTypeObject) PyUnicodeIter_Type;

#define SSTATE_NOT_INTERNED 0
#define SSTATE_INTERNED_MORTAL 1
#define SSTATE_INTERNED_IMMORTAL 2

#define PyUnicode_Check(op) \
                 PyType_FastSubclass(Py_TYPE(op), Py_TPFLAGS_UNICODE_SUBCLASS)
#define PyUnicode_CheckExact(op) (Py_TYPE(op) == &PyUnicode_Type)

/* Fast access macros */
#ifndef Py_LIMITED_API
#define PyUnicode_GET_SIZE(op) \
    (assert(PyUnicode_Check(op)),(((PyUnicodeObject *)(op))->length))
#define PyUnicode_GET_DATA_SIZE(op) \
    (assert(PyUnicode_Check(op)),(((PyUnicodeObject *)(op))->length * sizeof(Py_UNICODE)))
#define PyUnicode_AS_UNICODE(op) \
    (assert(PyUnicode_Check(op)),(((PyUnicodeObject *)(op))->str))
#define PyUnicode_AS_DATA(op) \
    (assert(PyUnicode_Check(op)),((const char *)((PyUnicodeObject *)(op))->str))
#endif

/* --- Constants ---------------------------------------------------------- */

/* This Unicode character will be used as replacement character during
   decoding if the errors argument is set to "replace". Note: the
   Unicode character U+FFFD is the official REPLACEMENT CHARACTER in
   Unicode 3.0. */

#define Py_UNICODE_REPLACEMENT_CHARACTER ((Py_UNICODE) 0xFFFD)

/* === Public API ========================================================= */

/* --- Plain Py_UNICODE --------------------------------------------------- */

/* Create a Unicode Object from the Py_UNICODE buffer u of the given
   size.

   u may be NULL which causes the contents to be undefined. It is the
   user's responsibility to fill in the needed data afterwards. Note
   that modifying the Unicode object contents after construction is
   only allowed if u was set to NULL.

   The buffer is copied into the new object. */

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_FromUnicode(
    const Py_UNICODE *u,        /* Unicode buffer */
    Py_ssize_t size             /* size of buffer */
    );
#endif

/* Similar to PyUnicode_FromUnicode(), but u points to UTF-8 encoded bytes */
PyAPI_FUNC(PyObject*) PyUnicode_FromStringAndSize(
    const char *u,             /* UTF-8 encoded string */
    Py_ssize_t size            /* size of buffer */
    );

/* Similar to PyUnicode_FromUnicode(), but u points to null-terminated
   UTF-8 encoded bytes */
PyAPI_FUNC(PyObject*) PyUnicode_FromString(
    const char *u              /* UTF-8 encoded string */
    );

/* Return a read-only pointer to the Unicode object's internal
   Py_UNICODE buffer. */

#ifndef Py_LIMITED_API
PyAPI_FUNC(Py_UNICODE *) PyUnicode_AsUnicode(
    PyObject *unicode           /* Unicode object */
    );
#endif

/* Get the length of the Unicode object. */

PyAPI_FUNC(Py_ssize_t) PyUnicode_GetSize(
    PyObject *unicode           /* Unicode object */
    );

#ifndef Py_LIMITED_API
/* Get the maximum ordinal for a Unicode character. */
PyAPI_FUNC(Py_UNICODE) PyUnicode_GetMax(void);
#endif

/* Resize an already allocated Unicode object to the new size length.

   *unicode is modified to point to the new (resized) object and 0
   returned on success.

   This API may only be called by the function which also called the
   Unicode constructor. The refcount on the object must be 1. Otherwise,
   an error is returned.

   Error handling is implemented as follows: an exception is set, -1
   is returned and *unicode left untouched.

*/

PyAPI_FUNC(int) PyUnicode_Resize(
    PyObject **unicode,         /* Pointer to the Unicode object */
    Py_ssize_t length           /* New length */
    );

/* Coerce obj to an Unicode object and return a reference with
   *incremented* refcount.

   Coercion is done in the following way:

   1. bytes, bytearray and other char buffer compatible objects are decoded
      under the assumptions that they contain data using the UTF-8
      encoding. Decoding is done in "strict" mode.

   2. All other objects (including Unicode objects) raise an
      exception.

   The API returns NULL in case of an error. The caller is responsible
   for decref'ing the returned objects.

*/

PyAPI_FUNC(PyObject*) PyUnicode_FromEncodedObject(
    register PyObject *obj,     /* Object */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Coerce obj to an Unicode object and return a reference with
   *incremented* refcount.

   Unicode objects are passed back as-is (subclasses are converted to
   true Unicode objects), all other objects are delegated to
   PyUnicode_FromEncodedObject(obj, NULL, "strict") which results in
   using UTF-8 encoding as basis for decoding the object.

   The API returns NULL in case of an error. The caller is responsible
   for decref'ing the returned objects.

*/

PyAPI_FUNC(PyObject*) PyUnicode_FromObject(
    register PyObject *obj      /* Object */
    );

PyAPI_FUNC(PyObject *) PyUnicode_FromFormatV(
    const char *format,   /* ASCII-encoded string  */
    va_list vargs
    );
PyAPI_FUNC(PyObject *) PyUnicode_FromFormat(
    const char *format,   /* ASCII-encoded string  */
    ...
    );

#ifndef Py_LIMITED_API
/* Format the object based on the format_spec, as defined in PEP 3101
   (Advanced String Formatting). */
PyAPI_FUNC(PyObject *) _PyUnicode_FormatAdvanced(PyObject *obj,
                                                 Py_UNICODE *format_spec,
                                                 Py_ssize_t format_spec_len);
#endif

PyAPI_FUNC(void) PyUnicode_InternInPlace(PyObject **);
PyAPI_FUNC(void) PyUnicode_InternImmortal(PyObject **);
PyAPI_FUNC(PyObject *) PyUnicode_InternFromString(
    const char *u              /* UTF-8 encoded string */
    );
#ifndef Py_LIMITED_API
PyAPI_FUNC(void) _Py_ReleaseInternedUnicodeStrings(void);
#endif

/* Use only if you know it's a string */
#define PyUnicode_CHECK_INTERNED(op) (((PyUnicodeObject *)(op))->state)

/* --- wchar_t support for platforms which support it --------------------- */

#ifdef HAVE_WCHAR_H

/* Create a Unicode Object from the wchar_t buffer w of the given
   size.

   The buffer is copied into the new object. */

PyAPI_FUNC(PyObject*) PyUnicode_FromWideChar(
    register const wchar_t *w,  /* wchar_t buffer */
    Py_ssize_t size             /* size of buffer */
    );

/* Copies the Unicode Object contents into the wchar_t buffer w.  At
   most size wchar_t characters are copied.

   Note that the resulting wchar_t string may or may not be
   0-terminated.  It is the responsibility of the caller to make sure
   that the wchar_t string is 0-terminated in case this is required by
   the application.

   Returns the number of wchar_t characters copied (excluding a
   possibly trailing 0-termination character) or -1 in case of an
   error. */

PyAPI_FUNC(Py_ssize_t) PyUnicode_AsWideChar(
    PyObject *unicode,          /* Unicode object */
    register wchar_t *w,        /* wchar_t buffer */
    Py_ssize_t size             /* size of buffer */
    );

/* Convert the Unicode object to a wide character string. The output string
   always ends with a nul character. If size is not NULL, write the number of
   wide characters (excluding the null character) into *size.

   Returns a buffer allocated by PyMem_Alloc() (use PyMem_Free() to free it)
   on success. On error, returns NULL, *size is undefined and raises a
   MemoryError. */

PyAPI_FUNC(wchar_t*) PyUnicode_AsWideCharString(
    PyObject *unicode,          /* Unicode object */
    Py_ssize_t *size            /* number of characters of the result */
    );

#endif

/* --- Unicode ordinals --------------------------------------------------- */

/* Create a Unicode Object from the given Unicode code point ordinal.

   The ordinal must be in range(0x10000) on narrow Python builds
   (UCS2), and range(0x110000) on wide builds (UCS4). A ValueError is
   raised in case it is not.

*/

PyAPI_FUNC(PyObject*) PyUnicode_FromOrdinal(int ordinal);

/* --- Free-list management ----------------------------------------------- */

/* Clear the free list used by the Unicode implementation.

   This can be used to release memory used for objects on the free
   list back to the Python memory allocator.

*/

PyAPI_FUNC(int) PyUnicode_ClearFreeList(void);

/* === Builtin Codecs =====================================================

   Many of these APIs take two arguments encoding and errors. These
   parameters encoding and errors have the same semantics as the ones
   of the builtin str() API.

   Setting encoding to NULL causes the default encoding (UTF-8) to be used.

   Error handling is set by errors which may also be set to NULL
   meaning to use the default handling defined for the codec. Default
   error handling for all builtin codecs is "strict" (ValueErrors are
   raised).

   The codecs all use a similar interface. Only deviation from the
   generic ones are documented.

*/

/* --- Manage the default encoding ---------------------------------------- */

/* Return a Python string holding the default encoded value of the
   Unicode object.

   Same as PyUnicode_AsUTF8String() except
   the resulting string is cached in the Unicode object for subsequent
   usage by this function. The cached version is needed to implement
   the character buffer interface and will live (at least) as long as
   the Unicode object itself.

   The refcount of the string is *not* incremented.

   *** Exported for internal use by the interpreter only !!! ***

*/

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject *) _PyUnicode_AsDefaultEncodedString(
    PyObject *unicode,
    const char *errors);
#endif

/* Returns a pointer to the default encoding (UTF-8) of the
   Unicode object unicode and the size of the encoded representation
   in bytes stored in *size.

   In case of an error, no *size is set.

   *** This API is for interpreter INTERNAL USE ONLY and will likely
   *** be removed or changed in the future.

   *** If you need to access the Unicode object as UTF-8 bytes string,
   *** please use PyUnicode_AsUTF8String() instead.

*/

#ifndef Py_LIMITED_API
PyAPI_FUNC(char *) _PyUnicode_AsStringAndSize(
    PyObject *unicode,
    Py_ssize_t *size);
#endif

/* Returns a pointer to the default encoding (UTF-8) of the
   Unicode object unicode.

   Use of this API is DEPRECATED since no size information can be
   extracted from the returned data.

   *** This API is for interpreter INTERNAL USE ONLY and will likely
   *** be removed or changed for Python 3.1.

   *** If you need to access the Unicode object as UTF-8 bytes string,
   *** please use PyUnicode_AsUTF8String() instead.

*/

#ifndef Py_LIMITED_API
PyAPI_FUNC(char *) _PyUnicode_AsString(PyObject *unicode);
#endif

/* Returns "utf-8".  */

PyAPI_FUNC(const char*) PyUnicode_GetDefaultEncoding(void);

/* --- Generic Codecs ----------------------------------------------------- */

/* Create a Unicode object by decoding the encoded string s of the
   given size. */

PyAPI_FUNC(PyObject*) PyUnicode_Decode(
    const char *s,              /* encoded string */
    Py_ssize_t size,            /* size of buffer */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Decode a Unicode object unicode and return the result as Python
   object. */

PyAPI_FUNC(PyObject*) PyUnicode_AsDecodedObject(
    PyObject *unicode,          /* Unicode object */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Decode a Unicode object unicode and return the result as Unicode
   object. */

PyAPI_FUNC(PyObject*) PyUnicode_AsDecodedUnicode(
    PyObject *unicode,          /* Unicode object */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Encodes a Py_UNICODE buffer of the given size and returns a
   Python string object. */

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_Encode(
    const Py_UNICODE *s,        /* Unicode char buffer */
    Py_ssize_t size,            /* number of Py_UNICODE chars to encode */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );
#endif

/* Encodes a Unicode object and returns the result as Python
   object. */

PyAPI_FUNC(PyObject*) PyUnicode_AsEncodedObject(
    PyObject *unicode,          /* Unicode object */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Encodes a Unicode object and returns the result as Python string
   object. */

PyAPI_FUNC(PyObject*) PyUnicode_AsEncodedString(
    PyObject *unicode,          /* Unicode object */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Encodes a Unicode object and returns the result as Unicode
   object. */

PyAPI_FUNC(PyObject*) PyUnicode_AsEncodedUnicode(
    PyObject *unicode,          /* Unicode object */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Build an encoding map. */

PyAPI_FUNC(PyObject*) PyUnicode_BuildEncodingMap(
    PyObject* string            /* 256 character map */
   );

/* --- UTF-7 Codecs ------------------------------------------------------- */

PyAPI_FUNC(PyObject*) PyUnicode_DecodeUTF7(
    const char *string,         /* UTF-7 encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors          /* error handling */
    );

PyAPI_FUNC(PyObject*) PyUnicode_DecodeUTF7Stateful(
    const char *string,         /* UTF-7 encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors,         /* error handling */
    Py_ssize_t *consumed        /* bytes consumed */
    );

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_EncodeUTF7(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length,          /* number of Py_UNICODE chars to encode */
    int base64SetO,             /* Encode RFC2152 Set O characters in base64 */
    int base64WhiteSpace,       /* Encode whitespace (sp, ht, nl, cr) in base64 */
    const char *errors          /* error handling */
    );
#endif

/* --- UTF-8 Codecs ------------------------------------------------------- */

PyAPI_FUNC(PyObject*) PyUnicode_DecodeUTF8(
    const char *string,         /* UTF-8 encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors          /* error handling */
    );

PyAPI_FUNC(PyObject*) PyUnicode_DecodeUTF8Stateful(
    const char *string,         /* UTF-8 encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors,         /* error handling */
    Py_ssize_t *consumed        /* bytes consumed */
    );

PyAPI_FUNC(PyObject*) PyUnicode_AsUTF8String(
    PyObject *unicode           /* Unicode object */
    );

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_EncodeUTF8(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length,          /* number of Py_UNICODE chars to encode */
    const char *errors          /* error handling */
    );
#endif

/* --- UTF-32 Codecs ------------------------------------------------------ */

/* Decodes length bytes from a UTF-32 encoded buffer string and returns
   the corresponding Unicode object.

   errors (if non-NULL) defines the error handling. It defaults
   to "strict".

   If byteorder is non-NULL, the decoder starts decoding using the
   given byte order:

    *byteorder == -1: little endian
    *byteorder == 0:  native order
    *byteorder == 1:  big endian

   In native mode, the first four bytes of the stream are checked for a
   BOM mark. If found, the BOM mark is analysed, the byte order
   adjusted and the BOM skipped.  In the other modes, no BOM mark
   interpretation is done. After completion, *byteorder is set to the
   current byte order at the end of input data.

   If byteorder is NULL, the codec starts in native order mode.

*/

PyAPI_FUNC(PyObject*) PyUnicode_DecodeUTF32(
    const char *string,         /* UTF-32 encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors,         /* error handling */
    int *byteorder              /* pointer to byteorder to use
                                   0=native;-1=LE,1=BE; updated on
                                   exit */
    );

PyAPI_FUNC(PyObject*) PyUnicode_DecodeUTF32Stateful(
    const char *string,         /* UTF-32 encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors,         /* error handling */
    int *byteorder,             /* pointer to byteorder to use
                                   0=native;-1=LE,1=BE; updated on
                                   exit */
    Py_ssize_t *consumed        /* bytes consumed */
    );

/* Returns a Python string using the UTF-32 encoding in native byte
   order. The string always starts with a BOM mark.  */

PyAPI_FUNC(PyObject*) PyUnicode_AsUTF32String(
    PyObject *unicode           /* Unicode object */
    );

/* Returns a Python string object holding the UTF-32 encoded value of
   the Unicode data.

   If byteorder is not 0, output is written according to the following
   byte order:

   byteorder == -1: little endian
   byteorder == 0:  native byte order (writes a BOM mark)
   byteorder == 1:  big endian

   If byteorder is 0, the output string will always start with the
   Unicode BOM mark (U+FEFF). In the other two modes, no BOM mark is
   prepended.

*/

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_EncodeUTF32(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length,          /* number of Py_UNICODE chars to encode */
    const char *errors,         /* error handling */
    int byteorder               /* byteorder to use 0=BOM+native;-1=LE,1=BE */
    );
#endif

/* --- UTF-16 Codecs ------------------------------------------------------ */

/* Decodes length bytes from a UTF-16 encoded buffer string and returns
   the corresponding Unicode object.

   errors (if non-NULL) defines the error handling. It defaults
   to "strict".

   If byteorder is non-NULL, the decoder starts decoding using the
   given byte order:

    *byteorder == -1: little endian
    *byteorder == 0:  native order
    *byteorder == 1:  big endian

   In native mode, the first two bytes of the stream are checked for a
   BOM mark. If found, the BOM mark is analysed, the byte order
   adjusted and the BOM skipped.  In the other modes, no BOM mark
   interpretation is done. After completion, *byteorder is set to the
   current byte order at the end of input data.

   If byteorder is NULL, the codec starts in native order mode.

*/

PyAPI_FUNC(PyObject*) PyUnicode_DecodeUTF16(
    const char *string,         /* UTF-16 encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors,         /* error handling */
    int *byteorder              /* pointer to byteorder to use
                                   0=native;-1=LE,1=BE; updated on
                                   exit */
    );

PyAPI_FUNC(PyObject*) PyUnicode_DecodeUTF16Stateful(
    const char *string,         /* UTF-16 encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors,         /* error handling */
    int *byteorder,             /* pointer to byteorder to use
                                   0=native;-1=LE,1=BE; updated on
                                   exit */
    Py_ssize_t *consumed        /* bytes consumed */
    );

/* Returns a Python string using the UTF-16 encoding in native byte
   order. The string always starts with a BOM mark.  */

PyAPI_FUNC(PyObject*) PyUnicode_AsUTF16String(
    PyObject *unicode           /* Unicode object */
    );

/* Returns a Python string object holding the UTF-16 encoded value of
   the Unicode data.

   If byteorder is not 0, output is written according to the following
   byte order:

   byteorder == -1: little endian
   byteorder == 0:  native byte order (writes a BOM mark)
   byteorder == 1:  big endian

   If byteorder is 0, the output string will always start with the
   Unicode BOM mark (U+FEFF). In the other two modes, no BOM mark is
   prepended.

   Note that Py_UNICODE data is being interpreted as UTF-16 reduced to
   UCS-2. This trick makes it possible to add full UTF-16 capabilities
   at a later point without compromising the APIs.

*/

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_EncodeUTF16(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length,          /* number of Py_UNICODE chars to encode */
    const char *errors,         /* error handling */
    int byteorder               /* byteorder to use 0=BOM+native;-1=LE,1=BE */
    );
#endif

/* --- Unicode-Escape Codecs ---------------------------------------------- */

PyAPI_FUNC(PyObject*) PyUnicode_DecodeUnicodeEscape(
    const char *string,         /* Unicode-Escape encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors          /* error handling */
    );

PyAPI_FUNC(PyObject*) PyUnicode_AsUnicodeEscapeString(
    PyObject *unicode           /* Unicode object */
    );

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_EncodeUnicodeEscape(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length           /* Number of Py_UNICODE chars to encode */
    );
#endif

/* --- Raw-Unicode-Escape Codecs ------------------------------------------ */

PyAPI_FUNC(PyObject*) PyUnicode_DecodeRawUnicodeEscape(
    const char *string,         /* Raw-Unicode-Escape encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors          /* error handling */
    );

PyAPI_FUNC(PyObject*) PyUnicode_AsRawUnicodeEscapeString(
    PyObject *unicode           /* Unicode object */
    );

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_EncodeRawUnicodeEscape(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length           /* Number of Py_UNICODE chars to encode */
    );
#endif

/* --- Unicode Internal Codec ---------------------------------------------

    Only for internal use in _codecsmodule.c */

#ifndef Py_LIMITED_API
PyObject *_PyUnicode_DecodeUnicodeInternal(
    const char *string,
    Py_ssize_t length,
    const char *errors
    );
#endif

/* --- Latin-1 Codecs -----------------------------------------------------

   Note: Latin-1 corresponds to the first 256 Unicode ordinals.

*/

PyAPI_FUNC(PyObject*) PyUnicode_DecodeLatin1(
    const char *string,         /* Latin-1 encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors          /* error handling */
    );

PyAPI_FUNC(PyObject*) PyUnicode_AsLatin1String(
    PyObject *unicode           /* Unicode object */
    );

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_EncodeLatin1(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length,          /* Number of Py_UNICODE chars to encode */
    const char *errors          /* error handling */
    );
#endif

/* --- ASCII Codecs -------------------------------------------------------

   Only 7-bit ASCII data is excepted. All other codes generate errors.

*/

PyAPI_FUNC(PyObject*) PyUnicode_DecodeASCII(
    const char *string,         /* ASCII encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors          /* error handling */
    );

PyAPI_FUNC(PyObject*) PyUnicode_AsASCIIString(
    PyObject *unicode           /* Unicode object */
    );

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_EncodeASCII(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length,          /* Number of Py_UNICODE chars to encode */
    const char *errors          /* error handling */
    );
#endif

/* --- Character Map Codecs -----------------------------------------------

   This codec uses mappings to encode and decode characters.

   Decoding mappings must map single string characters to single
   Unicode characters, integers (which are then interpreted as Unicode
   ordinals) or None (meaning "undefined mapping" and causing an
   error).

   Encoding mappings must map single Unicode characters to single
   string characters, integers (which are then interpreted as Latin-1
   ordinals) or None (meaning "undefined mapping" and causing an
   error).

   If a character lookup fails with a LookupError, the character is
   copied as-is meaning that its ordinal value will be interpreted as
   Unicode or Latin-1 ordinal resp. Because of this mappings only need
   to contain those mappings which map characters to different code
   points.

*/

PyAPI_FUNC(PyObject*) PyUnicode_DecodeCharmap(
    const char *string,         /* Encoded string */
    Py_ssize_t length,          /* size of string */
    PyObject *mapping,          /* character mapping
                                   (char ordinal -> unicode ordinal) */
    const char *errors          /* error handling */
    );

PyAPI_FUNC(PyObject*) PyUnicode_AsCharmapString(
    PyObject *unicode,          /* Unicode object */
    PyObject *mapping           /* character mapping
                                   (unicode ordinal -> char ordinal) */
    );

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_EncodeCharmap(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length,          /* Number of Py_UNICODE chars to encode */
    PyObject *mapping,          /* character mapping
                                   (unicode ordinal -> char ordinal) */
    const char *errors          /* error handling */
    );
#endif

/* Translate a Py_UNICODE buffer of the given length by applying a
   character mapping table to it and return the resulting Unicode
   object.

   The mapping table must map Unicode ordinal integers to Unicode
   ordinal integers or None (causing deletion of the character).

   Mapping tables may be dictionaries or sequences. Unmapped character
   ordinals (ones which cause a LookupError) are left untouched and
   are copied as-is.

*/

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject *) PyUnicode_TranslateCharmap(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length,          /* Number of Py_UNICODE chars to encode */
    PyObject *table,            /* Translate table */
    const char *errors          /* error handling */
    );
#endif

#ifdef MS_WIN32

/* --- MBCS codecs for Windows -------------------------------------------- */

PyAPI_FUNC(PyObject*) PyUnicode_DecodeMBCS(
    const char *string,         /* MBCS encoded string */
    Py_ssize_t length,              /* size of string */
    const char *errors          /* error handling */
    );

PyAPI_FUNC(PyObject*) PyUnicode_DecodeMBCSStateful(
    const char *string,         /* MBCS encoded string */
    Py_ssize_t length,          /* size of string */
    const char *errors,         /* error handling */
    Py_ssize_t *consumed        /* bytes consumed */
    );

PyAPI_FUNC(PyObject*) PyUnicode_AsMBCSString(
    PyObject *unicode           /* Unicode object */
    );

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_EncodeMBCS(
    const Py_UNICODE *data,     /* Unicode char buffer */
    Py_ssize_t length,          /* Number of Py_UNICODE chars to encode */
    const char *errors          /* error handling */
    );
#endif

#endif /* MS_WIN32 */

/* --- Decimal Encoder ---------------------------------------------------- */

/* Takes a Unicode string holding a decimal value and writes it into
   an output buffer using standard ASCII digit codes.

   The output buffer has to provide at least length+1 bytes of storage
   area. The output string is 0-terminated.

   The encoder converts whitespace to ' ', decimal characters to their
   corresponding ASCII digit and all other Latin-1 characters except
   \0 as-is. Characters outside this range (Unicode ordinals 1-256)
   are treated as errors. This includes embedded NULL bytes.

   Error handling is defined by the errors argument:

      NULL or "strict": raise a ValueError
      "ignore": ignore the wrong characters (these are not copied to the
                output buffer)
      "replace": replaces illegal characters with '?'

   Returns 0 on success, -1 on failure.

*/

#ifndef Py_LIMITED_API
PyAPI_FUNC(int) PyUnicode_EncodeDecimal(
    Py_UNICODE *s,              /* Unicode buffer */
    Py_ssize_t length,          /* Number of Py_UNICODE chars to encode */
    char *output,               /* Output buffer; must have size >= length */
    const char *errors          /* error handling */
    );
#endif

/* Transforms code points that have decimal digit property to the
   corresponding ASCII digit code points.

   Returns a new Unicode string on success, NULL on failure.
*/

#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject*) PyUnicode_TransformDecimalToASCII(
    Py_UNICODE *s,              /* Unicode buffer */
    Py_ssize_t length           /* Number of Py_UNICODE chars to transform */
    );
#endif

/* --- File system encoding ---------------------------------------------- */

/* ParseTuple converter: encode str objects to bytes using
   PyUnicode_EncodeFSDefault(); bytes objects are output as-is. */

PyAPI_FUNC(int) PyUnicode_FSConverter(PyObject*, void*);

/* ParseTuple converter: decode bytes objects to unicode using
   PyUnicode_DecodeFSDefaultAndSize(); str objects are output as-is. */

PyAPI_FUNC(int) PyUnicode_FSDecoder(PyObject*, void*);

/* Decode a null-terminated string using Py_FileSystemDefaultEncoding
   and the "surrogateescape" error handler.

   If Py_FileSystemDefaultEncoding is not set, fall back to the locale
   encoding.

   Use PyUnicode_DecodeFSDefaultAndSize() if the string length is known.
*/

PyAPI_FUNC(PyObject*) PyUnicode_DecodeFSDefault(
    const char *s               /* encoded string */
    );

/* Decode a string using Py_FileSystemDefaultEncoding
   and the "surrogateescape" error handler.

   If Py_FileSystemDefaultEncoding is not set, fall back to the locale
   encoding.
*/

PyAPI_FUNC(PyObject*) PyUnicode_DecodeFSDefaultAndSize(
    const char *s,               /* encoded string */
    Py_ssize_t size              /* size */
    );

/* Encode a Unicode object to Py_FileSystemDefaultEncoding with the
   "surrogateescape" error handler, and return bytes.

   If Py_FileSystemDefaultEncoding is not set, fall back to the locale
   encoding.
*/

PyAPI_FUNC(PyObject*) PyUnicode_EncodeFSDefault(
    PyObject *unicode
    );

/* --- Methods & Slots ----------------------------------------------------

   These are capable of handling Unicode objects and strings on input
   (we refer to them as strings in the descriptions) and return
   Unicode objects or integers as apporpriate. */

/* Concat two strings giving a new Unicode string. */

PyAPI_FUNC(PyObject*) PyUnicode_Concat(
    PyObject *left,             /* Left string */
    PyObject *right             /* Right string */
    );

/* Concat two strings and put the result in *pleft
   (sets *pleft to NULL on error) */

PyAPI_FUNC(void) PyUnicode_Append(
    PyObject **pleft,           /* Pointer to left string */
    PyObject *right             /* Right string */
    );

/* Concat two strings, put the result in *pleft and drop the right object
   (sets *pleft to NULL on error) */

PyAPI_FUNC(void) PyUnicode_AppendAndDel(
    PyObject **pleft,           /* Pointer to left string */
    PyObject *right             /* Right string */
    );

/* Split a string giving a list of Unicode strings.

   If sep is NULL, splitting will be done at all whitespace
   substrings. Otherwise, splits occur at the given separator.

   At most maxsplit splits will be done. If negative, no limit is set.

   Separators are not included in the resulting list.

*/

PyAPI_FUNC(PyObject*) PyUnicode_Split(
    PyObject *s,                /* String to split */
    PyObject *sep,              /* String separator */
    Py_ssize_t maxsplit         /* Maxsplit count */
    );

/* Dito, but split at line breaks.

   CRLF is considered to be one line break. Line breaks are not
   included in the resulting list. */

PyAPI_FUNC(PyObject*) PyUnicode_Splitlines(
    PyObject *s,                /* String to split */
    int keepends                /* If true, line end markers are included */
    );

/* Partition a string using a given separator. */

PyAPI_FUNC(PyObject*) PyUnicode_Partition(
    PyObject *s,                /* String to partition */
    PyObject *sep               /* String separator */
    );

/* Partition a string using a given separator, searching from the end of the
   string. */

PyAPI_FUNC(PyObject*) PyUnicode_RPartition(
    PyObject *s,                /* String to partition */
    PyObject *sep               /* String separator */
    );

/* Split a string giving a list of Unicode strings.

   If sep is NULL, splitting will be done at all whitespace
   substrings. Otherwise, splits occur at the given separator.

   At most maxsplit splits will be done. But unlike PyUnicode_Split
   PyUnicode_RSplit splits from the end of the string. If negative,
   no limit is set.

   Separators are not included in the resulting list.

*/

PyAPI_FUNC(PyObject*) PyUnicode_RSplit(
    PyObject *s,                /* String to split */
    PyObject *sep,              /* String separator */
    Py_ssize_t maxsplit         /* Maxsplit count */
    );

/* Translate a string by applying a character mapping table to it and
   return the resulting Unicode object.

   The mapping table must map Unicode ordinal integers to Unicode
   ordinal integers or None (causing deletion of the character).

   Mapping tables may be dictionaries or sequences. Unmapped character
   ordinals (ones which cause a LookupError) are left untouched and
   are copied as-is.

*/

PyAPI_FUNC(PyObject *) PyUnicode_Translate(
    PyObject *str,              /* String */
    PyObject *table,            /* Translate table */
    const char *errors          /* error handling */
    );

/* Join a sequence of strings using the given separator and return
   the resulting Unicode string. */

PyAPI_FUNC(PyObject*) PyUnicode_Join(
    PyObject *separator,        /* Separator string */
    PyObject *seq               /* Sequence object */
    );

/* Return 1 if substr matches str[start:end] at the given tail end, 0
   otherwise. */

PyAPI_FUNC(Py_ssize_t) PyUnicode_Tailmatch(
    PyObject *str,              /* String */
    PyObject *substr,           /* Prefix or Suffix string */
    Py_ssize_t start,           /* Start index */
    Py_ssize_t end,             /* Stop index */
    int direction               /* Tail end: -1 prefix, +1 suffix */
    );

/* Return the first position of substr in str[start:end] using the
   given search direction or -1 if not found. -2 is returned in case
   an error occurred and an exception is set. */

PyAPI_FUNC(Py_ssize_t) PyUnicode_Find(
    PyObject *str,              /* String */
    PyObject *substr,           /* Substring to find */
    Py_ssize_t start,           /* Start index */
    Py_ssize_t end,             /* Stop index */
    int direction               /* Find direction: +1 forward, -1 backward */
    );

/* Count the number of occurrences of substr in str[start:end]. */

PyAPI_FUNC(Py_ssize_t) PyUnicode_Count(
    PyObject *str,              /* String */
    PyObject *substr,           /* Substring to count */
    Py_ssize_t start,           /* Start index */
    Py_ssize_t end              /* Stop index */
    );

/* Replace at most maxcount occurrences of substr in str with replstr
   and return the resulting Unicode object. */

PyAPI_FUNC(PyObject *) PyUnicode_Replace(
    PyObject *str,              /* String */
    PyObject *substr,           /* Substring to find */
    PyObject *replstr,          /* Substring to replace */
    Py_ssize_t maxcount         /* Max. number of replacements to apply;
                                   -1 = all */
    );

/* Compare two strings and return -1, 0, 1 for less than, equal,
   greater than resp. */

PyAPI_FUNC(int) PyUnicode_Compare(
    PyObject *left,             /* Left string */
    PyObject *right             /* Right string */
    );

PyAPI_FUNC(int) PyUnicode_CompareWithASCIIString(
    PyObject *left,
    const char *right           /* ASCII-encoded string */
    );

/* Rich compare two strings and return one of the following:

   - NULL in case an exception was raised
   - Py_True or Py_False for successfuly comparisons
   - Py_NotImplemented in case the type combination is unknown

   Note that Py_EQ and Py_NE comparisons can cause a UnicodeWarning in
   case the conversion of the arguments to Unicode fails with a
   UnicodeDecodeError.

   Possible values for op:

     Py_GT, Py_GE, Py_EQ, Py_NE, Py_LT, Py_LE

*/

PyAPI_FUNC(PyObject *) PyUnicode_RichCompare(
    PyObject *left,             /* Left string */
    PyObject *right,            /* Right string */
    int op                      /* Operation: Py_EQ, Py_NE, Py_GT, etc. */
    );

/* Apply a argument tuple or dictionary to a format string and return
   the resulting Unicode string. */

PyAPI_FUNC(PyObject *) PyUnicode_Format(
    PyObject *format,           /* Format string */
    PyObject *args              /* Argument tuple or dictionary */
    );

/* Checks whether element is contained in container and return 1/0
   accordingly.

   element has to coerce to an one element Unicode string. -1 is
   returned in case of an error. */

PyAPI_FUNC(int) PyUnicode_Contains(
    PyObject *container,        /* Container string */
    PyObject *element           /* Element string */
    );

/* Checks whether the string contains any NUL characters. */

#ifndef Py_LIMITED_API
PyAPI_FUNC(int) _PyUnicode_HasNULChars(PyObject *);
#endif

/* Checks whether argument is a valid identifier. */

PyAPI_FUNC(int) PyUnicode_IsIdentifier(PyObject *s);

#ifndef Py_LIMITED_API
/* Externally visible for str.strip(unicode) */
PyAPI_FUNC(PyObject *) _PyUnicode_XStrip(
    PyUnicodeObject *self,
    int striptype,
    PyObject *sepobj
    );
#endif

/* Using the current locale, insert the thousands grouping
   into the string pointed to by buffer.  For the argument descriptions,
   see Objects/stringlib/localeutil.h */

#ifndef Py_LIMITED_API
PyAPI_FUNC(Py_ssize_t) _PyUnicode_InsertThousandsGroupingLocale(Py_UNICODE *buffer,
                                                   Py_ssize_t n_buffer,
                                                   Py_UNICODE *digits,
                                                   Py_ssize_t n_digits,
                                                   Py_ssize_t min_width);
#endif

/* Using explicit passed-in values, insert the thousands grouping
   into the string pointed to by buffer.  For the argument descriptions,
   see Objects/stringlib/localeutil.h */
#ifndef Py_LIMITED_API
PyAPI_FUNC(Py_ssize_t) _PyUnicode_InsertThousandsGrouping(Py_UNICODE *buffer,
                                                   Py_ssize_t n_buffer,
                                                   Py_UNICODE *digits,
                                                   Py_ssize_t n_digits,
                                                   Py_ssize_t min_width,
                                                   const char *grouping,
                                                   const char *thousands_sep);
#endif
/* === Characters Type APIs =============================================== */

/* Helper array used by Py_UNICODE_ISSPACE(). */

#ifndef Py_LIMITED_API
PyAPI_DATA(const unsigned char) _Py_ascii_whitespace[];

/* These should not be used directly. Use the Py_UNICODE_IS* and
   Py_UNICODE_TO* macros instead.

   These APIs are implemented in Objects/unicodectype.c.

*/

PyAPI_FUNC(int) _PyUnicode_IsLowercase(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsUppercase(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsTitlecase(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsXidStart(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsXidContinue(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsWhitespace(
    const Py_UCS4 ch         /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsLinebreak(
    const Py_UCS4 ch         /* Unicode character */
    );

PyAPI_FUNC(Py_UCS4) _PyUnicode_ToLowercase(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(Py_UCS4) _PyUnicode_ToUppercase(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(Py_UCS4) _PyUnicode_ToTitlecase(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_ToDecimalDigit(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_ToDigit(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(double) _PyUnicode_ToNumeric(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsDecimalDigit(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsDigit(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsNumeric(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsPrintable(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(int) _PyUnicode_IsAlpha(
    Py_UCS4 ch       /* Unicode character */
    );

PyAPI_FUNC(size_t) Py_UNICODE_strlen(
    const Py_UNICODE *u
    );

PyAPI_FUNC(Py_UNICODE*) Py_UNICODE_strcpy(
    Py_UNICODE *s1,
    const Py_UNICODE *s2);

PyAPI_FUNC(Py_UNICODE*) Py_UNICODE_strcat(
    Py_UNICODE *s1, const Py_UNICODE *s2);

PyAPI_FUNC(Py_UNICODE*) Py_UNICODE_strncpy(
    Py_UNICODE *s1,
    const Py_UNICODE *s2,
    size_t n);

PyAPI_FUNC(int) Py_UNICODE_strcmp(
    const Py_UNICODE *s1,
    const Py_UNICODE *s2
    );

PyAPI_FUNC(int) Py_UNICODE_strncmp(
    const Py_UNICODE *s1,
    const Py_UNICODE *s2,
    size_t n
    );

PyAPI_FUNC(Py_UNICODE*) Py_UNICODE_strchr(
    const Py_UNICODE *s,
    Py_UNICODE c
    );

PyAPI_FUNC(Py_UNICODE*) Py_UNICODE_strrchr(
    const Py_UNICODE *s,
    Py_UNICODE c
    );

/* Create a copy of a unicode string ending with a nul character. Return NULL
   and raise a MemoryError exception on memory allocation failure, otherwise
   return a new allocated buffer (use PyMem_Free() to free the buffer). */

PyAPI_FUNC(Py_UNICODE*) PyUnicode_AsUnicodeCopy(
    PyObject *unicode
    );
#endif /* Py_LIMITED_API */

PyAPI_FUNC(void) _PyUnicode_DebugMallocStats(FILE *out);

#ifdef __cplusplus
}
#endif
#endif /* !Py_UNICODEOBJECT_H */
