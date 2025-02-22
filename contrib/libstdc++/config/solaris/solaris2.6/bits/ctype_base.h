// Locale support -*- C++ -*-

// Copyright (C) 1997-1999 Cygnus Solutions
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

//
// ISO C++ 14882: 22.1  Locales
//
  
// Information as gleaned from /usr/include/ctype.h. Looks like this
// only works with solaris2.6 and solaris2.7, but not solaris2.5.1.

  struct ctype_base
  {
    typedef unsigned int 	mask;
    // Non-standard typedefs.
    typedef long* 		__to_type;

    enum
    {
      space = _ISSPACE,
      print = _ISPRINT,
      cntrl = _ISCNTRL,
      upper = _ISUPPER,
      lower = _ISLOWER,
      alpha = _ISALPHA,
      digit = _ISDIGIT,
      punct = _ISPUNCT,
      xdigit = _ISXDIGIT,
      alnum = _ISALNUM,
      graph = _ISGRAPH
    };
  };

