#ifndef _AMOS_READ_H
#define _AMOS_READ_H

#include <cstdint>
#include <string>

namespace AMOS {

  class Read {
    public:
      uint32_t iid;
      uint32_t clr_lo;
      uint32_t clr_hi;
      std::string seq;
      std::string qlt;

      Read() : seq(nullptr), qlt(nullptr) {}

      Read(uint32_t iid, uint32_t clr_lo, uint32_t clr_hi, const std::string seq, const std::string qlt)
        :iid(iid), clr_lo(clr_lo), clr_hi(clr_hi), seq(seq), qlt(qlt)
      {}
  };
}
#endif
