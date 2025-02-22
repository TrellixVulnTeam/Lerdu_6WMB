// 2000-03-23 bkoz

// Copyright (C) 2000 Free Software Foundation
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
//

#include <sstream>
#include <ostream>
#ifdef DEBUG_ASSERT
  #include <assert.h>
#endif


void test01()
{
  using namespace std;
  typedef std::stringbuf::pos_type        pos_type;
  typedef std::stringbuf::off_type        off_type;
  bool test = true;

  // tellp
  ostringstream ost;
  pos_type pos1;
  pos1 = ost.tellp();
  test &= pos1 == pos_type(-1);
  ost << "RZA ";
  pos1 = ost.tellp();
  test &= pos1 == pos_type(4);
  ost << "ghost dog: way of the samurai";
  pos1 = ost.tellp();
  test &= pos1 == pos_type(33);

#ifdef DEBUG_ASSERT
  assert(test);
#endif
}                                    

int main()
{
  test01();
}
