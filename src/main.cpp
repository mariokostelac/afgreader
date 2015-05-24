#include <iostream>
#include "reader.h"
using namespace std;

int main(int argc, char** argv) {

  auto reader = new AMOS::Reader(cin);

  while (reader->has_next()) {
    auto o = reader->next_overlap();
    cout << o->adjacency << " " << o->read1 << " " << o->read2 << " " << o->a_hang << " " << o->b_hang << endl;
  }
}
