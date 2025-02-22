/* Complex sine function for float. */

/* Copyright (C) 1997-1999 Free Software Foundation, Inc.

   This file is part of the GNU ISO C++ Library.  This library is free
   software; you can redistribute it and/or modify it under the
   terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option)
   any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this library; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.

   As a special exception, you may use this file as part of a free software
   library without restriction.  Specifically, if other files instantiate
   templates or use macros or inline functions from this file, or you compile
   this file and link it with other files to produce an executable, this
   file does not by itself cause the resulting executable to be covered by
   the GNU General Public License.  This exception does not however
   invalidate any other reasons why the executable file might be covered by
   the GNU General Public License.  */


#include <math.h>
#include "mathconf.h"


__complex__ float
csinf (__complex__ float x)
{
  __complex__ float retval;
  int negate = signbit (__real__ x);

  __real__ x = fabsf (__real__ x);

  if (FINITEF_P (__imag__ x))
    {
      /* Imaginary part is finite.  */
      if (FINITEF_P (__real__ x))
	{
	  /* Real part is finite.  */
	  float sinh_val = sinhf (__imag__ x);
	  float cosh_val = coshf (__imag__ x);
	  float sinix = sinf (__real__ x);
	  float cosix = cosf (__real__ x);

	  __real__ retval = cosh_val * sinix;
	  __imag__ retval = sinh_val * cosix;

	  if (negate)
	    __real__ retval = -__real__ retval;
	}
      else
	{
	  if (__imag__ x == 0.0)
	    {
	      /* Imaginary part is 0.0.  */
	      __real__ retval = NAN;
	      __imag__ retval = __imag__ x;
	    }
	  else
	    {
	      __real__ retval = NAN;
	      __imag__ retval = NAN;
	    }
	}
    }
  else if (INFINITEF_P (__imag__ x))
    {
      /* Imaginary part is infinite.  */
      if (__real__ x == 0.0)
	{
	  /* Real part is 0.0.  */
	  __real__ retval = copysignf (0.0, negate ? -1.0 : 1.0);
	  __imag__ retval = __imag__ x;
	}
      else if (FINITEF_P (__real__ x))
	{
	  /* Real part is finite.  */
	  float sinix = sinf (__real__ x);
	  float cosix = cosf (__real__ x);

	  __real__ retval = copysignf (HUGE_VALF, sinix);
	  __imag__ retval = copysignf (HUGE_VALF, cosix);

	  if (negate)
	    __real__ retval = -__real__ retval;
	  if (signbit (__imag__ x))
	    __imag__ retval = -__imag__ retval;
	}
      else
	{
	  /* The addition raises the invalid exception.  */
	  __real__ retval = NAN;
	  __imag__ retval = HUGE_VALF;
	}
    }
  else
    {
      if (__real__ x == 0.0)
	__real__ retval = copysignf (0.0, negate ? -1.0 : 1.0);
      else
	__real__ retval = NAN;
      __imag__ retval = NAN;
    }

  return retval;
}
