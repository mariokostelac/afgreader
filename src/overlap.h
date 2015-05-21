#ifndef _AMOS_OVERLAP_H
#define _AMOS_OVERLAP_H
#include <iostream>
#include <cstdint>
#include <cassert>
using namespace std;

namespace AMOS {
  class Overlap {
    public:
      uint32_t read1;
      uint32_t read2;
      char adjacency;
      int32_t a_hang;
      int32_t b_hang;
      uint32_t score;

      Overlap(const uint32_t r1, const uint32_t r2, const char adj, const int32_t ahg, const int32_t bhg, const int32_t scr)
        :read1(r1), read2(r2), adjacency(adj), a_hang(ahg), b_hang(bhg), score(scr)
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

      bool use_start(const uint32_t read) const;

      bool use_end(const uint32_t read) const;
  };

  ostream& operator << (ostream &o, const AMOS::Overlap& overlap);
}
#endif
