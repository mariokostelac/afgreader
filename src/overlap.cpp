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
    if (read != a_id && read != b_id) {
      return false;
    }

    if (read == a_id && a_hang <= 0) {
      return true;
    } else if (read == b_id && adjacency == 'N' && a_hang >= 0) {
      return true;
    } else if (read == b_id && adjacency == 'I' && b_hang <= 0) {
      return true;
    }

    return false;
  }

  bool Overlap::use_end(const uint32_t read) const {
    if (read != a_id && read != b_id) {
      return false;
    }

    if (read == a_id && b_hang >= 0) {
      return true;
    } else if (read == b_id && adjacency == 'N' && b_hang <= 0) {
      return true;
    } else if (read == b_id && adjacency == 'I' && a_hang >= 0) {
      return true;
    }

    return false;
  }

  ostream& operator << (ostream &o, const Overlap& overlap) {
    o << "{OVL" << endl;
    o << "adj:" << overlap.adjacency << endl;
    o << "rds:" << overlap.a_id << "," << overlap.b_id << endl;
    o << "ahg:" << overlap.a_hang << endl;
    o << "bhg:" << overlap.b_hang << endl;
    o << "scr:" << overlap.score << endl;
    o << "}" << endl;
    return o;
  }
};
