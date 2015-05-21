#ifndef _AMOS_READ_H
#define _AMOS_READ_H

#include <cstdint>

namespace AMOS {

  class Read {
    public:
      uint32_t iid;
      uint32_t clr_lo;
      uint32_t clr_hi;
      char *seq;
      char *qlt;

      Read() : seq(nullptr), qlt(nullptr) {}

      Read(uint32_t iid, uint32_t clr_lo, uint32_t clr_hi, char *seq, char *qlt)
        :iid(iid), clr_lo(clr_lo), clr_hi(clr_hi), seq(seq), qlt(qlt)
      {}

      ~Read() {
        if (seq != nullptr) {
          delete[] seq;
        }
        if (qlt != nullptr) {
          delete[] qlt;
        }
      }
  };
}
#endif
