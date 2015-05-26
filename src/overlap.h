#ifndef _AMOS_OVERLAP_H
#define _AMOS_OVERLAP_H
#include "read.h"

#include <iostream>
#include <memory>
#include <cstdint>
#include <cassert>

using namespace std;

namespace AMOS {
  class Overlap {
    public:
      shared_ptr<Read> a;
      shared_ptr<Read> b;
      uint32_t a_id;
      uint32_t b_id;
      int32_t a_hang;
      int32_t b_hang;

      char adjacency;
      uint32_t score;

      Overlap(const uint32_t r1, const uint32_t r2, const char adj, const int32_t ahg, const int32_t bhg, const int32_t scr)
        :a_id(r1), b_id(r2), a_hang(ahg), b_hang(bhg), adjacency(adj), score(scr)
      {
        assert(adj == 'N' || adj == 'I');
      }

      Overlap() {}

      //
      // ahang - number of bases from read A that come before the overlap.
      // bhang - number of bases from read B that come after the overlap.
      //
      // read a      -------------------|-------------->     bhang
      // read b            ahang     ---------------|--------------->
      //
      // read a           -ahang     ---------------|--------------->
      // read b      -------------------|-------------->     -bhang
      //

      bool use_prefix(const uint32_t read) const;

      bool use_suffix(const uint32_t read) const;

      // length returns the overlap's length.
      int32_t length() const;

      // length returns overlap's length contained in read with given id.
      // if wrong r_id given, returns -1.
      int32_t length(const uint32_t r_id) const;

      int32_t overlap_length_a() const;

      int32_t overlap_length_b() const;

      // Returns which end of the read this overlap is using.
      // 0 for prefix, 1 for suffix.
      // It respects direction, so if overlap looks like
      // ----->
      // <-----
      // it will return 1 for second read.
      //
      // Returns -1 if wrong r_id is given.

      int32_t hanging_length(const uint32_t r_id) const;
      uint32_t hanging_length_a() const;
      uint32_t hanging_length_b() const;
  };

  ostream& operator << (ostream &o, const AMOS::Overlap& overlap);
}
#endif
