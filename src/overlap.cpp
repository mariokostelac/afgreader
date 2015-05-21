#include "overlap.h"

namespace AMOS {

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

  bool Overlap::use_start(const uint32_t read) const {
    if (read != read1 && read != read2) {
      return false;
    }

    if (read == read1 && a_hang <= 0) {
      return true;
    } else if (read == read2 && adjacency == 'N' && a_hang >= 0) {
      return true;
    } else if (read == read2 && adjacency == 'I' && b_hang <= 0) {
      return true;
    }

    return false;
  }

  bool Overlap::use_end(const uint32_t read) const {
    if (read != read1 && read != read2) {
      return false;
    }

    if (read == read1 && b_hang >= 0) {
      return true;
    } else if (read == read2 && adjacency == 'N' && b_hang <= 0) {
      return true;
    } else if (read == read2 && adjacency == 'I' && a_hang >= 0) {
      return true;
    }

    return false;
  }

  ostream& operator << (ostream &o, const Overlap& overlap) {
    o << "{OVL" << endl;
    o << "adj:" << overlap.adjacency << endl;
    o << "rds:" << overlap.read1 << "," << overlap.read2 << endl;
    o << "ahg:" << overlap.a_hang << endl;
    o << "bhg:" << overlap.b_hang << endl;
    o << "scr:" << overlap.score << endl;
    o << "}" << endl;
    return o;
  }
};
