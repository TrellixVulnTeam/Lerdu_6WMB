// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_ERROR_H
#define _GSTREAMERMM_ERROR_H


#include <glibmm.h>

/* gstreamermm - a C++ wrapper for gstreamer
 *
 * Copyright 2008 The gstreamermm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


namespace Gst
{

/** The GStreamer core GError type.
 * GStreamer elements can throw non-fatal warnings and fatal errors.
 * Higher-level elements and applications can programatically filter the ones
 * they are interested in or can recover from, and have a default handler
 * handle the rest of them. Both warnings and fatal errors are treated
 * similarly.
 *
 * Core errors are errors inside the core GStreamer library:
 *
 * - FAILED - a general error which doesn't fit in any other category. Make
 * sure you add a custom message to the error call.
 * - TOO_LAZY - do not use this except as a placeholder for deciding where to
 * go while developing code.
 * - NOT_IMPLEMENTED - use this when you do not want to implement this
 * functionality yet.
 * - STATE_CHANGE - used for state change errors.
 * - PAD - used for pad-related errors.
 * - THREAD - used for thread-related errors.
 * - NEGOTIATION - used for negotiation-related errors.
 * - EVENT - used for event-related errors.
 * - SEEK - used for seek-related errors.
 * - CAPS - used for caps-related errors.
 * - TAG - used for negotiation-related errors.
 * - MISSING_PLUGIN - used if a plugin is missing.
 * - CLOCK - used for clock related errors.
 * - DISABLED - used if functionality has been disabled at compile time (Since:
 * 0.10.13).
 * - NUM_ERRORS - the number of core error types.
 *
 * Elements do not have the context required to decide what to do with
 * errors. As such, they should only inform about errors, and stop their
 * processing. In short, an element doesn't know what it is being used for.
 *
 * It is the application or compound element using the given element that has
 * more context about the use of the element. Errors can be received by
 * listening to the Gst::Bus of the element/pipeline for Gst::Message objects
 * with the type Gst::MESSAGE_ERROR or Gst::MESSAGE_WARNING. The thrown
 * errors should be inspected, and filtered if appropriate.
 *
 * An application is expected to, by default, present the user with a dialog
 * box (or an equivalent) showing the error message. The dialog should also
 * allow a way to get at the additional debug information, so the user can
 * provide bug reporting information.
 *
 * A compound element is expected to forward errors by default higher up the
 * hierarchy; this is done by default in the same way as for other types of
 * Gst::Message.
 *
 * When applications or compound elements trigger errors that they can
 * recover from, they can filter out these errors and take appropriate
 * action. For example, an application that gets an error from xvimagesink
 * that indicates all XVideo ports are taken, the application can attempt to
 * use another sink instead.
 */
class CoreError : public Glib::Error
{
public:
  enum Code
  {
    FAILED = 1,
    TOO_LAZY,
    NOT_IMPLEMENTED,
    STATE_CHANGE,
    PAD,
    THREAD,
    NEGOTIATION,
    EVENT,
    SEEK,
    CAPS,
    TAG,
    MISSING_PLUGIN,
    CLOCK,
    DISABLED,
    NUM_ERRORS
  };

  CoreError(Code error_code, const Glib::ustring& error_message);
  explicit CoreError(GError* gobject);
  Code code() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:

  static void throw_func(GError* gobject);

  friend void wrap_init(); // uses throw_func()

  #endif //DOXYGEN_SHOULD_SKIP_THIS
};


/** Gets the error domain for core system. Errors in this domain will be from
 * the Gst::CoreError enumeration.
 *
 * @return The quark associated with the core error domain.
 */
Glib::QueryQuark get_core_error_quark();

/** The GStreamer library GError type.
 * GStreamer elements can throw non-fatal warnings and fatal errors.
 * Higher-level elements and applications can programatically filter the ones
 * they are interested in or can recover from, and have a default handler
 * handle the rest of them. Both warnings and fatal errors are treated
 * similarly.
 *
 * Library errors are for errors from the library being used by elements
 * (initializing, finalizing, settings, ...)
 *
 * - FAILED - a general error which doesn't fit in any other category. Make
 * sure you add a custom message to the error call.
 * - TOO_LAZY - do not use this except as a placeholder for deciding where to
 * go while developing code.
 * - INIT - used when the library could not be opened.
 * - SHUTDOWN - used when the library could not be closed.
 * - SETTINGS - used when the library doesn't accept settings.
 * - ENCODE - used when the library generated an encoding error.
 * - NUM_ERRORS - the number of library error types.
 *
 * Elements do not have the context required to decide what to do with
 * errors. As such, they should only inform about errors, and stop their
 * processing. In short, an element doesn't know what it is being used for.
 *
 * It is the application or compound element using the given element that has
 * more context about the use of the element. Errors can be received by
 * listening to the Gst::Bus of the element/pipeline for Gst::Message objects
 * with the type Gst::MESSAGE_ERROR or Gst::MESSAGE_WARNING. The thrown
 * errors should be inspected, and filtered if appropriate.
 *
 * An application is expected to, by default, present the user with a dialog
 * box (or an equivalent) showing the error message. The dialog should also
 * allow a way to get at the additional debug information, so the user can
 * provide bug reporting information.
 *
 * A compound element is expected to forward errors by default higher up the
 * hierarchy; this is done by default in the same way as for other types of
 * Gst::Message.
 *
 * When applications or compound elements trigger errors that they can
 * recover from, they can filter out these errors and take appropriate
 * action. For example, an application that gets an error from xvimagesink
 * that indicates all XVideo ports are taken, the application can attempt to
 * use another sink instead.
 */
class LibraryError : public Glib::Error
{
public:
  enum Code
  {
    FAILED = 1,
    TOO_LAZY,
    INIT,
    SHUTDOWN,
    SETTINGS,
    ENCODE,
    NUM_ERRORS
  };

  LibraryError(Code error_code, const Glib::ustring& error_message);
  explicit LibraryError(GError* gobject);
  Code code() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:

  static void throw_func(GError* gobject);

  friend void wrap_init(); // uses throw_func()

  #endif //DOXYGEN_SHOULD_SKIP_THIS
};


/** Gets the error domain for library loading. Errors in this domain will be
 * from the gst::libraryerror enumeration.
 *
 * @return The quark associated with the library error domain.
 */
Glib::QueryQuark get_library_error_quark();

/** The GStreamer resource GError type.
 * GStreamer elements can throw non-fatal warnings and fatal errors.
 * Higher-level elements and applications can programatically filter the ones
 * they are interested in or can recover from, and have a default handler
 * handle the rest of them. Both warnings and fatal errors are treated
 * similarly.
 *
 * Resource errors are for any resource used by an element: memory, files,
 * network connections, process space, ... They're typically used by source and
 * sink elements.
 *
 * - FAILED - a general error which doesn't fit in any other category. Make
 * sure you add a custom message to the error call.
 * - TOO_LAZY - do not use this except as a placeholder for deciding where to
 * go while developing code.
 * - NOT_FOUND - used when the resource could not be found.
 * - BUSY - used when resource is busy.
 * - OPEN_READ - used when resource fails to open for reading.
 * - OPEN_WRITE - used when resource fails to open for writing.
 * - OPEN_READ_WRITE - used when resource cannot be opened for both reading and
 * writing, or either (but unspecified which).
 * - CLOSE - used when the resource can't be closed.
 * - READ - used when the resource can't be read from.
 * - WRITE - used when the resource can't be written to.
 * - SEEK - used when a seek on the resource fails.
 * - SYNC - used when a synchronize on the resource fails.
 * - SETTINGS - used when settings can't be manipulated on.
 * - NO_SPACE_LEFT - used when the resource has no space left.
 * - NUM_ERRORS - the number of resource error types.
 *
 * Elements do not have the context required to decide what to do with
 * errors. As such, they should only inform about errors, and stop their
 * processing. In short, an element doesn't know what it is being used for.
 *
 * It is the application or compound element using the given element that has
 * more context about the use of the element. Errors can be received by
 * listening to the Gst::Bus of the element/pipeline for Gst::Message objects
 * with the type Gst::MESSAGE_ERROR or Gst::MESSAGE_WARNING. The thrown
 * errors should be inspected, and filtered if appropriate.
 *
 * An application is expected to, by default, present the user with a dialog
 * box (or an equivalent) showing the error message. The dialog should also
 * allow a way to get at the additional debug information, so the user can
 * provide bug reporting information.
 *
 * A compound element is expected to forward errors by default higher up the
 * hierarchy; this is done by default in the same way as for other types of
 * Gst::Message.
 *
 * When applications or compound elements trigger errors that they can
 * recover from, they can filter out these errors and take appropriate
 * action. For example, an application that gets an error from xvimagesink
 * that indicates all XVideo ports are taken, the application can attempt to
 * use another sink instead.
 */
class ResourceError : public Glib::Error
{
public:
  enum Code
  {
    FAILED = 1,
    TOO_LAZY,
    NOT_FOUND,
    BUSY,
    OPEN_READ,
    OPEN_WRITE,
    OPEN_READ_WRITE,
    CLOSE,
    READ,
    WRITE,
    SEEK,
    SYNC,
    SETTINGS,
    NO_SPACE_LEFT,
    NUM_ERRORS
  };

  ResourceError(Code error_code, const Glib::ustring& error_message);
  explicit ResourceError(GError* gobject);
  Code code() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:

  static void throw_func(GError* gobject);

  friend void wrap_init(); // uses throw_func()

  #endif //DOXYGEN_SHOULD_SKIP_THIS
};


/** Gets error domain for resource handling. Errors in this domain will be
 * from the Gst::ResourceError enumeration.
 *
 * @return The quark associated with the resource error domain.
 */
Glib::QueryQuark get_resource_error_quark();

/** The GStreamer stream GError type.
 * GStreamer elements can throw non-fatal warnings and fatal errors.
 * Higher-level elements and applications can programatically filter the ones
 * they are interested in or can recover from, and have a default handler
 * handle the rest of them. Both warnings and fatal errors are treated
 * similarly.
 *
 * Stream errors are for anything related to the stream being processed: format
 * errors, media type errors, ... They're typically used by decoders, demuxers,
 * converters, ...
 * 
 * - FAILED - a general error which doesn't fit in any other category. Make
 * sure you add a custom message to the error call.
 * - TOO_LAZY - do not use this except as a placeholder for deciding where to
 * go while developing code.
 * - NOT_IMPLEMENTED - use this when you do not want to implement this
 * functionality yet.
 * - TYPE_NOT_FOUND - used when the element doesn't know the stream's type.
 * - WRONG_TYPE - used when the element doesn't handle this type of stream.
 * - CODEC_NOT_FOUND - used when there's no codec to handle the stream's type.
 * - DECODE - used when decoding fails.
 * - ENCODE - used when encoding fails.
 * - DEMUX - used when demuxing fails.
 * - MUX - used when muxing fails.
 * - FORMAT - used when the stream is of the wrong format (for example, wrong
 * caps).
 * - DECRYPT - used when the stream is encrypted and can't be decrypted because
 * this is not supported by the element. (Since: 0.10.20)
 * - DECRYPT_NOKEY - used when the stream is encrypted and can't be decrypted
 * because no suitable key is available. (Since: 0.10.20)
 * - NUM_ERRORS - the number of stream error types.
 *
 * Elements do not have the context required to decide what to do with
 * errors. As such, they should only inform about errors, and stop their
 * processing. In short, an element doesn't know what it is being used for.
 *
 * It is the application or compound element using the given element that has
 * more context about the use of the element. Errors can be received by
 * listening to the Gst::Bus of the element/pipeline for Gst::Message objects
 * with the type Gst::MESSAGE_ERROR or Gst::MESSAGE_WARNING. The thrown
 * errors should be inspected, and filtered if appropriate.
 *
 * An application is expected to, by default, present the user with a dialog
 * box (or an equivalent) showing the error message. The dialog should also
 * allow a way to get at the additional debug information, so the user can
 * provide bug reporting information.
 *
 * A compound element is expected to forward errors by default higher up the
 * hierarchy; this is done by default in the same way as for other types of
 * Gst::Message.
 *
 * When applications or compound elements trigger errors that they can
 * recover from, they can filter out these errors and take appropriate
 * action. For example, an application that gets an error from xvimagesink
 * that indicates all XVideo ports are taken, the application can attempt to
 * use another sink instead.
 */
class StreamError : public Glib::Error
{
public:
  enum Code
  {
    FAILED = 1,
    TOO_LAZY,
    NOT_IMPLEMENTED,
    TYPE_NOT_FOUND,
    WRONG_TYPE,
    CODEC_NOT_FOUND,
    DECODE,
    ENCODE,
    DEMUX,
    MUX,
    FORMAT,
    DECRYPT,
    DECRYPT_NOKEY,
    NUM_ERRORS
  };

  StreamError(Code error_code, const Glib::ustring& error_message);
  explicit StreamError(GError* gobject);
  Code code() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:

  static void throw_func(GError* gobject);

  friend void wrap_init(); // uses throw_func()

  #endif //DOXYGEN_SHOULD_SKIP_THIS
};


/** Gets error domain for media stream processing. Errors in this domain will
 * be from the Gst::StreamError enumeration.
 *
 * @return The quark associated with the media stream error domain.
 */
Glib::QueryQuark get_stream_error_quark();

} // namespace Gst


#endif /* _GSTREAMERMM_ERROR_H */

