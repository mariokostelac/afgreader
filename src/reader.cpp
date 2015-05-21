
#include "reader.h"

#include <cassert>
#include <cstring>
#include <stack>

#define BUFF_CAP 4096
#define MAX_SEQ 524288

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

  class Read* Reader::next_read() {
    //cerr << "NEXT_READ" << endl;
    if (!has_next()) {
      return nullptr;
    }

    while (next_type() != ObjectType::Read) {
      //cerr << "WRONG TYPE" << endl;
      skip_next();
      if (!has_next()) {
        return nullptr;
      }
    }

    //cout << "READ " << buff << endl;
    skip_next();
    // add code for parsing read

    return nullptr;
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

      int seq_start = 0;
      auto state = states.top();
      switch (state) {
        case OUT:
          if (strncmp(line, "{RED", 4) == 0) {
            states.push(IN);
            next_type_ = ObjectType::Read;
          } else if (strncmp(line, "{OVL", 4) == 0) {
            states.push(IN);
            next_type_ = ObjectType::Overlap;
          } else if (strncmp(line, "{UNV", 4) == 0) {
            states.push(IN);
            next_type_ = ObjectType::Universal;
          } else if (strncmp(line, "{LIB", 4) == 0) {
            states.push(IN);
            next_type_ = ObjectType::Library;
          } else if (strncmp(line, "{FRG", 4) == 0) {
            states.push(IN);
            next_type_ = ObjectType::Fragment;
          } else {
            assert(false);
          }

          buff_marks.push_back(BufferMark(ObjectDef, line_start, buff_written));
          break;
        case IN:
          if (line[0] == '}') {
            states.pop();
          } else if (strncmp(line, "seq:", 4) == 0) {
            states.push(IN_SEQ);
            seq_start = line_start;
          } else if (strncmp(line, "qlt:", 4) == 0) {
            states.push(IN_QLT);
            seq_start = line_start;
          } else if (strncmp(line, "com:", 4) == 0) {
            states.push(IN_COM);
            seq_start = line_start;
          } else if (strncmp(line, "{DST", 4) == 0) {
            buff_marks.push_back(BufferMark(ObjectDef, line_start, buff_written));
            states.push(IN);
          } else {
            buff_marks.push_back(BufferMark(AttrDef, line_start, buff_written));
          }
          break;
        case IN_SEQ:
        case IN_QLT:
        case IN_COM:
          if (line[0] == '.') {
            buff_marks.push_back(BufferMark(ObjectDef, seq_start, buff_written));
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
}
