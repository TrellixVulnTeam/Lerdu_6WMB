// 2000-01-15  Anders Widell  <awl@hem.passagen.se>

// Copyright (C) 2000 Free Software Foundation, Inc.
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

#include <string>
#include <set>
#include <bitset>

#ifdef DEBUG_ASSERT
#include <assert.h>
#endif

static char original_bits[1024];
static char left_shifted[1024];
static char right_shifted[1024];

char
random_bit() {
  static long x = 1;
  return ((x = (3432L*x + 6789L) % 9973L) & 1) + '0';
}

void
initialise(size_t size) {
  for (size_t i=0; i<size; i++)
    original_bits[i] = random_bit();  

  original_bits[size] = '\0';
  left_shifted[size] = '\0';
  right_shifted[size] = '\0';
}

void
shift_arrays(size_t shift_step, size_t size) {
  for (size_t i=shift_step; i<size; i++) {
    right_shifted[i] = original_bits[i-shift_step];
    left_shifted[size-i-1] = original_bits[size+shift_step-i-1];
  }
  for (size_t i=0; i<shift_step && i<size; i++) {
    right_shifted[i] = '0';
    left_shifted[size-i-1] = '0';
  }
}

template <size_t size>
  bool
  do_test() {
    bool test = true;

    std::bitset<size> shifted;
    std::bitset<size> correct;
  
    initialise(size);

    std::bitset<size> original = std::string(original_bits); 

    for (size_t shift_step=0; shift_step==0 || shift_step<size; shift_step++) {
      shift_arrays(shift_step, size);

      shifted = original;
      shifted <<= shift_step;
      correct = std::string(left_shifted);
      test &= shifted == correct;

      shifted = original;
      shifted >>= shift_step;
      correct = std::string(right_shifted);
      test &= shifted == correct;
    }

    return test;
  }

bool
test01() {
  bool test = true;

  test &= do_test<32>();
  test &= do_test<48>();
  test &= do_test<64>();

  test &= do_test<511>();
  test &= do_test<513>();
  test &= do_test<997>();

#ifdef DEBUG_ASSERT
  assert(test);
#endif
  return test;
}

int
main() {
  test01();

  return 0;
}
