
#include "reader.h"

#include <cassert>
#include <cstring>
#include <stack>

#define BUFF_CAP 4096

using std::istream;
using std::stack;

namespace AMOS {

  Reader::Reader(istream& input): input(&input), buff_written(0) {
    buff = new char[BUFF_CAP];
    buff_cap = BUFF_CAP;
    states.push(OUT);
  }

  bool Reader::has_next() {
    if (buff_written == 0) {
      return buffer_next() > 0;
    }

    return buff_written > 0;
  }

  int Reader::skip_next() {
    int skipped = 0;

    if (buff_written == 0) {
      skipped = buffer_next();
      if (skipped == -1) {
        return -1;
      }
    } else {
      skipped = buff_written;
    }

    //cerr << "SKIP " << buff << endl;
    buffer_clear();

    return skipped;
  }


  ObjectType Reader::next_type() {
    return next_type_;
  }

  bool Reader::next(Read* dst) {
    if (!has_next()) {
      return nullptr;
    }

    while (next_type() != READ) {
      //cerr << "WRONG TYPE" << endl;
      skip_next();
      if (!has_next()) {
        return false;
      }
    }

    //cout << "READ " << buff << endl;
    return read_from_buff(dst);
  }

  bool Reader::next(Overlap* dst) {
    if (!has_next()) {
      return false;
    }

    while (next_type() != OVERLAP) {
      skip_next();
      if (!has_next()) {
        return false;
      }
    }

    return overlap_from_buff(dst);
  }

  int Reader::buffer_clear() {
    //cerr << "FLUSH " << buff << endl;
    int was_written = buff_written;
    buff_written = 0;
    buff_marks.clear();
    return was_written;
  }

  int Reader::buffer_double() {
    buff = (char *) realloc(buff, buff_cap * 2);
    if (buff == nullptr) {
      buff_cap = 0;
      buff_written = 0;
      return -1;
    }

    buff_cap *= 2;
    return buff_cap;
  }

  int Reader::buffer_next() {
    assert(states.size() == 1);
    assert(buff_written == 0);

    if (input->eof()) return 0;

    int seq_start = 0;
    while (true) {
      auto line = buff + buff_written;

      input->getline(line, buff_cap - buff_written);

      int line_start = buff_written;
      buff_written += strlen(line);

      if (input->eof()) {
        return -1;
      }
      if (input->fail()) {
        return -1;
      }

      auto state = states.top();
      switch (state) {
        case OUT:
          if (strncmp(line, "{RED", 4) == 0) {
            states.push(IN);
            next_type_ = READ;
          } else if (strncmp(line, "{OVL", 4) == 0) {
            states.push(IN);
            next_type_ = OVERLAP;
          } else if (strncmp(line, "{UNV", 4) == 0) {
            states.push(IN);
            next_type_ = UNIVERSAL;
          } else if (strncmp(line, "{LIB", 4) == 0) {
            states.push(IN);
            next_type_ = LIBRARY;
          } else if (strncmp(line, "{FRG", 4) == 0) {
            states.push(IN);
            next_type_ = FRAGMENT;
          } else {
            assert(false);
          }

          buff_marks.push_back(BufferMark(ObjectDef, line_start, buff_written));
          break;
        case IN:
          if (line[0] == '}') {
            buff_marks.push_back(BufferMark(ObjectEnd, line_start, buff_written));
            states.pop();
          } else if (strncmp(line, "seq:", 4) == 0) {
            states.push(IN_STR);
            seq_start = line_start;
          } else if (strncmp(line, "qlt:", 4) == 0) {
            states.push(IN_STR);
            seq_start = line_start;
          } else if (strncmp(line, "com:", 4) == 0) {
            states.push(IN_STR);
            seq_start = line_start;
          } else if (strncmp(line, "{DST", 4) == 0) {
            buff_marks.push_back(BufferMark(ObjectDef, line_start, buff_written));
            states.push(IN);
          } else {
            buff_marks.push_back(BufferMark(AttrDef, line_start, buff_written));
          }
          break;
        case IN_STR:
          if (line[0] == '.') {
            // we do not want to include the '.' at the end
            buff_marks.push_back(BufferMark(AttrDef, seq_start, buff_written - 1));
            states.pop();
          }
          break;
        default:
          assert(false);
      }

      if (states.size() == 1) {
        //cerr << "FILL " << buff << endl;
        return buff_written;
      }
    }

    return -1;
  }

  bool Reader::read_from_buff(Read* read) {
    if (buff_written == 0) {
      return false;
    }

    if (strncmp(buff, "{RED", 4) != 0) {
      return false;
    }

    // use buffer marks to create a read from buffer
    for (auto& mark: buff_marks) {
      if (mark.type != AttrDef) {
        continue;
      }

      // temporary terminate the string so we can use sscanf
      char tmp = buff[mark.hi];
      buff[mark.hi] = 0;

      if (sscanf(buff + mark.lo, "clr:%d, %d", &read->clr_lo, &read->clr_hi)) {
        // just skip other branches
      } else if (sscanf(buff + mark.lo, "iid:%d", &read->iid)) {
        // just skip other branches
      } else if (strncmp(buff + mark.lo, "seq:", 4) == 0) {
        int seq_start = mark.lo + 4;
        read->seq = string(buff + seq_start);
      } else if (strncmp(buff + mark.lo, "qlt:", 4) == 0) {
        int qlt_start = mark.lo + 4;
        read->qlt = string(buff + qlt_start);
      }

      // return the right character back
      buff[mark.hi] = tmp;
    }

    buffer_clear();

    return true;
  }

  bool Reader::overlap_from_buff(Overlap* overlap) {
    if (buff_written == 0) {
      return false;
    }

    if (strncmp(buff, "{OVL", 4) != 0) {
      return false;
    }

    // use buffer marks to create an overlap from buffer
    for (auto& mark: buff_marks) {
      if (mark.type != AttrDef) {
        continue;
      }

      // temporary terminate the string so we can use sscanf
      char tmp = buff[mark.hi];
      buff[mark.hi] = 0;

      if (sscanf(buff + mark.lo, "rds:%u, %u", &overlap->a_id, &overlap->b_id)) {
        // just skip other branches
      } else if (sscanf(buff + mark.lo, "adj:%c", &overlap->adjacency)) {
        // just skip other branches
      } else if (sscanf(buff + mark.lo, "scr:%d", &overlap->score)) {
        // just skip other branches
      } else if (sscanf(buff + mark.lo, "ahg:%d", &overlap->a_hang)) {
        // just skip other branches
      } else if (sscanf(buff + mark.lo, "bhg:%d", &overlap->b_hang)) {
        // just skip other branches
      }

      // return the right character back
      buff[mark.hi] = tmp;
    }

    buffer_clear();

    return true;
  }
}
