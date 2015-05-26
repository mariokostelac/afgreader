#include <iostream>
#include "reader.h"
using namespace std;
using AMOS::Overlap;

int main(int argc, char** argv) {

  auto reader = new AMOS::Reader(cin);

  while (reader->has_next()) {
    Overlap o;
    reader->next(&o);
    cout << o.adjacency << " " << o.a_id << " " << o.b_id << " " << o.a_hang << " " << o.b_hang << endl;
  }
}
