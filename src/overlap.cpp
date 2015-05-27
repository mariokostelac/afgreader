#include "overlap.h"
#include <memory>

using std::shared_ptr;

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

  int32_t Overlap::length() const {
    return (overlap_length_a() + overlap_length_b())/2;
  }

  int32_t Overlap::length(const uint32_t r_id) const {
    if (r_id == a_id) {
      return overlap_length_a();
    } else if (r_id == b_id) {
      return overlap_length_b();
    } else {
      return -1;
    }
  }

  int32_t Overlap::overlap_length_a() const {
    int len = a->seq.length();
    if (a_hang > 0) {
      len -= a_hang;
    }

    if (b_hang < 0) {
      len += b_hang;
    }

    return len;
  }

  int32_t Overlap::overlap_length_b() const {
    int len = b->seq.length();
    if (b_hang > 0) {
      len -= b_hang;
    }

    if (a_hang < 0) {
      len += a_hang;
    }

    return len;
  }

  bool Overlap::use_prefix(const uint32_t read) const {
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

  bool Overlap::use_suffix(const uint32_t read) const {
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

  int32_t Overlap::hanging_length(const uint32_t r_id) const {
    if (r_id == a_id) {
      return hanging_length_a();
    } else if (r_id == b_id) {
      return hanging_length_b();
    } else {
      return -1;
    }
  }

  uint32_t Overlap::hanging_length_a() const {
    return a->seq.length() - overlap_length_a();
  }

  uint32_t Overlap::hanging_length_b() const {
    return b->seq.length() - overlap_length_b();
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
