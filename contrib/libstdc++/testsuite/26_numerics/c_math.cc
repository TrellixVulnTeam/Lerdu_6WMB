// 1999-06-05
// Gabriel Dos Reis <dosreis@cmla.ens-cachan.fr>

// Copyright (C) 2000, 1999 Free Software Foundation, Inc.
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

#include <cmath>
#include <cassert>

// test compilation.
void test01()
{
    float a = 1.f;
    float b;
    std::modf(a, &b);
}

// need more extravagant checks than this, of course, but this used to core...
void test02()
{
  sin(static_cast<float>(0));
}

// as did this.
void test03()
{
  double powtest = pow(2., 0);
}

// this used to abort.
void test04()
{
  float x[2] = {1, 2};
  float y = 3.4;
  std::modf(y, &x[0]);
  assert (x[1] == 2);
}

int main()
{
  test01();
  test02();
  test03();
  test04();
  return 0;
}
