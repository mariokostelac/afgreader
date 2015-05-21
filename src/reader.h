
#ifndef _AMOS_READER_H
#define _AMOS_READER_H

#include "read.h"
#include "overlap.h"

#include <cstdio>
#include <vector>
#include <stack>

namespace AMOS {

  enum ReaderState {
    OUT,
    IN,
    IN_READ,
    IN_SEQ,
    IN_QLT,
    IN_OVL,
    IN_COM,
  };

  enum ObjectType {
    Contig,
    ContigEdge,
    ContigLink,
    Distribution,
    Edge,
    Feature,
    Fragment,
    Group,
    IDMap,
    Index,
    Kmer,
    Layout,
    Library,
    Link,
    Overlap,
    Read,
    Scaffold,
    ScaffoldEdge,
    ScaffoldLink,
    Sequence,
    Universal
  };

  enum MarkType {
    ObjectDef,
    AttrDef
  };

  class BufferMark {
    public:
    MarkType mark_type;
    int lo;
    int hi;

    BufferMark(MarkType type, int lo, int hi): mark_type(type), lo(lo), hi(hi) {}
  };

  class Reader {
    public:
      Reader(std::istream& input);
      bool has_next();
      int skip_next();
      ObjectType next_type();
      class Read* next_read();
      class Overlap* next_overlap();

    private:
      std::istream* input;
      char* buff;
      int buff_cap;
      int buff_written;
      std::vector<BufferMark> buff_marks;
      std::stack<ReaderState> states;
      ObjectType next_type_;

      int buffer_next();
      int buffer_double();
      int buffer_clear();
  };
}

#endif
