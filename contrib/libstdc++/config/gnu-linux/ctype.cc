// Locale support -*- C++ -*-

// Copyright (C) 1997-1999, 2000 Free Software Foundation, Inc.
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
  
// Information as gleaned from /usr/include/ctype.h
  
  ctype<char>::ctype(const mask* __table = 0, bool __del = false, 
	size_t __refs = 0) throw()
    : _Ctype_nois<char>(__refs), _M_del(__table != 0 && __del), 
      _M_toupper(__ctype_toupper), _M_tolower(__ctype_tolower),
      _M_ctable(__ctype_b), _M_table(__table == 0 ? _M_ctable: __table) 
    { }

  char
  ctype<char>::do_toupper(char __c) const
  { return _M_toupper[(int) __c]; }

  const char*
  ctype<char>::do_toupper(char* __low, const char* __high) const
  {
    while (__low < __high)
      {
	*__low = _M_toupper[(int) *__low];
	++__low;
      }
    return __high;
  }

  char
  ctype<char>::do_tolower(char __c) const
  { return _M_tolower[(int) __c]; }

  const char* 
  ctype<char>::do_tolower(char* __low, const char* __high) const
  {
    while (__low < __high)
      {
	*__low = _M_tolower[(int) *__low];
	++__low;
      }
    return __high;
  }

#ifdef _GLIBCPP_USE_WCHAR_T  
  ctype<wchar_t>::ctype(size_t /*__refs*/) throw()
    : _M_toupper(__ctype_toupper), _M_tolower(__ctype_tolower),
      _M_ctable(__ctype_b)
    { }

  wchar_t
  ctype<wchar_t>::do_toupper(wchar_t __c) const
  { 
    return ((static_cast<__table_type>(__c) < _S_table_size) 
	    ? _M_toupper[__c] : __c);
  }
  
  const wchar_t*
  ctype<wchar_t>::do_toupper(wchar_t* low, const wchar_t* high) const
  {
    for (;low < high; ++low)
      if (static_cast<__table_type>(*low) < _S_table_size)
        *low = _M_toupper[*low];
    return high;
  }
  
  wchar_t
  ctype<wchar_t>::do_tolower(wchar_t __c) const
  { 
    return ((static_cast<__table_type>(__c) < _S_table_size) 
	    ? _M_tolower[__c] : __c);
  }
  
  const wchar_t*
  ctype<wchar_t>::do_tolower(wchar_t* __low, const wchar_t* __high) const
  {
    for (; __low < __high; ++__low)
      if (static_cast<__table_type>(*__low) < _S_table_size)
        *__low = _M_toupper[*__low];
    return __high;
  }
#endif


