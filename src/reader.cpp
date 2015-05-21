
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

  //int get_reads(std::vector<const Read*>& container, FILE *fd) {
    //int records = 0;

    //char line[buff_cap] = {0};

    //ReaderState state = OUT;
    //Read* curr_read = new Read();
    //char curr_seq[MAX_SEQ] = {0};
    //int curr_seq_len = 0;
    //int line_len = 0;
    //long last_pos = ftell(fd);
    //const char **dst_str = nullptr;
    //while (!feof(fd)) {
      //// read next line and update pointers
      //fgets(line, buff_cap, fd);
      //long curr_pos = ftell(fd);
      //assert(curr_pos - last_pos < buff_cap);
      //line_len = curr_pos - last_pos;
      //last_pos = curr_pos;

      //// strip \n
      //line[line_len - 1] = 0;
      //line_len--;

      //switch (state) {
        //case OUT:
          //if (strstr(line, "{RED") != nullptr) {
            //state = IN_READ;
          //}
          //break;
        //case IN_READ:
          //if (sscanf(line, " iid: %d ", &curr_read->iid)) {
            //state = IN_READ;
          //} else if (strstr(line, "seq:") != nullptr) {
            //dst_str = &(curr_read->seq);
            //state = IN_SEQ;
          //} else if (strstr(line, "qlt:") != nullptr) {
            //dst_str = &(curr_read->qlt);
            //state = IN_QLT;
          //} else if (sscanf(line, " clr: %d, %d ", &curr_read->clr_lo, &curr_read->clr_hi)) {
            //state = IN_READ;
          //} else if (line[0] == '}') {
            //state = OUT;
            //container.push_back(curr_read);
            //curr_read = new Read();
            //records++;
          //}
          //break;
        //case IN_SEQ:
        //case IN_QLT:
          //if (line[0] == '.') {
            //assert(dst_str != nullptr);
            //char *cpy = new char[curr_seq_len + 1];
            //strncpy(cpy, curr_seq, curr_seq_len + 1);
            //curr_seq_len = 0;
            //*dst_str = cpy; // assign copied string to the destination
            //dst_str = nullptr;
            //state = IN_READ;
          //} else {
            //assert(line_len < MAX_SEQ - curr_seq_len);
            //strcpy(curr_seq + curr_seq_len, line);
            //curr_seq_len += line_len;
          //}
          //break;
        //default:
          //assert(false);
      //}
    //}
    //delete curr_read;

    //return records;
  //}

  //int get_overlaps(std::vector<const Overlap*>& container, FILE* fd) {
    //int records = 0;

    //char line[buff_cap] = {0};

    //ReaderState state = OUT;
    //Overlap* curr_overlap = new Overlap();
    //int line_len = 0;
    //long last_pos = ftell(fd);
    //const char **dst_str = nullptr;
    //while (!feof(fd)) {
      //// read next line and update pointers
      //fgets(line, buff_cap, fd);
      //long curr_pos = ftell(fd);
      //assert(curr_pos - last_pos < buff_cap);
      //line_len = curr_pos - last_pos;
      //last_pos = curr_pos;

      //// strip \n
      //line[line_len - 1] = 0;
      //line_len--;

      //switch (state) {
        //case OUT:
          //if (strstr(line, "{OVL") != nullptr) {
            //state = IN_OVL;
          //}
          //break;
        //case IN_OVL:
          //if (sscanf(line, " rds: %d, %d ", &curr_overlap->read1, &curr_overlap->read2)) {
            //state = IN_OVL;
          //} else if (sscanf(line, " adj: %c", &curr_overlap->adjacency)) {
            //state = IN_OVL;
          //} else if (sscanf(line, " ahg: %d", &curr_overlap->a_hang)) {
            //state = IN_OVL;
          //} else if (sscanf(line, " bhg: %d", &curr_overlap->b_hang)) {
            //state = IN_OVL;
          //} else if (sscanf(line, " scr: %d", &curr_overlap->score)) {
            //state = IN_OVL;
          //} else if (line[0] == '}') {
            //state = OUT;
            //container.push_back(curr_overlap);
            //curr_overlap = new Overlap();
            //records++;
          //}
          //break;
        //default:
          //assert(false);
      //}
    //}
    //delete curr_overlap;

    //return records;
  //}
}
