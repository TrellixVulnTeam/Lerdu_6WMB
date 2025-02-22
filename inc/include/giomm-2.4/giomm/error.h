// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GIOMM_ERROR_H
#define _GIOMM_ERROR_H


#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2 -*-

/* Copyright (C) 2007 The giomm Development Team
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

#include <glibmm/error.h>
#include <glibmm/interface.h>

// There have been issues with other libraries defining HOST_NOT_FOUND (e.g.
// netdb.h).  As a workaround, we added the alternate name HOST_WAS_NOT_FOUND.
// Portable code should not use HOST_NOT_FOUND.  Undefining it here (and
// restoring it below) will allow programs to compile even if they include
// netdb.h.  See Bug #529496
#ifdef HOST_NOT_FOUND
#define GIOMM_SAVED_HOST_NOT_FOUND HOST_NOT_FOUND
#undef HOST_NOT_FOUND
#endif // HOST_NOT_FOUND


namespace Gio
{

//Note that GIOErrorEnum is not named GIOError in gio because there is already a GIOError in glib,
//But we can have both Glib::Error and Gio::Error in C++.

/** Exception class for giomm errors.
 */
class Error : public Glib::Error
{
public:
  enum Code
  {
    FAILED,
    NOT_FOUND,
    EXISTS,
    IS_DIRECTORY,
    NOT_DIRECTORY,
    NOT_EMPTY,
    NOT_REGULAR_FILE,
    NOT_SYMBOLIC_LINK,
    NOT_MOUNTABLE_FILE,
    FILENAME_TOO_LONG,
    INVALID_FILENAME,
    TOO_MANY_LINKS,
    NO_SPACE,
    INVALID_ARGUMENT,
    PERMISSION_DENIED,
    NOT_SUPPORTED,
    NOT_MOUNTED,
    ALREADY_MOUNTED,
    CLOSED,
    CANCELLED,
    PENDING,
    READ_ONLY,
    CANT_CREATE_BACKUP,
    WRONG_ETAG,
    TIMED_OUT,
    WOULD_RECURSE,
    BUSY,
    WOULD_BLOCK,
    HOST_NOT_FOUND,
    HOST_WAS_NOT_FOUND = HOST_NOT_FOUND,
    WOULD_MERGE,
    FAILED_HANDLED,
    TOO_MANY_OPEN_FILES,
    NOT_INITIALIZED,
    ADDRESS_IN_USE,
    PARTIAL_INPUT,
    INVALID_DATA,
    DBUS_ERROR,
    HOST_UNREACHABLE,
    NETWORK_UNREACHABLE,
    CONNECTION_REFUSED,
    PROXY_FAILED,
    PROXY_AUTH_FAILED,
    PROXY_NEED_AUTH,
    PROXY_NOT_ALLOWED
  };

  Error(Code error_code, const Glib::ustring& error_message);
  explicit Error(GError* gobject);
  Code code() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:

  static void throw_func(GError* gobject);

  friend void wrap_init(); // uses throw_func()

  #endif //DOXYGEN_SHOULD_SKIP_THIS
};


class ResolverError : public Glib::Error
{
public:
  enum Code
  {
    NOT_FOUND,
    TEMPORARY_FAILURE,
    INTERNAL
  };

  ResolverError(Code error_code, const Glib::ustring& error_message);
  explicit ResolverError(GError* gobject);
  Code code() const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:

  static void throw_func(GError* gobject);

  friend void wrap_init(); // uses throw_func()

  #endif //DOXYGEN_SHOULD_SKIP_THIS
};


} // namespace Gio

#ifdef GIOMM_SAVED_HOST_NOT_FOUND
// restore the previously-defined HOST_NOT_FOUND macro
#define HOST_NOT_FOUND GIOMM_SAVED_HOST_NOT_FOUND
#undef GIOMM_SAVED_HOST_NOT_FOUND
#endif // GIOMM_SAVED_HOST_NOT_FOUND


#endif /* _GIOMM_ERROR_H */

