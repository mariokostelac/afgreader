#include <iostream>
#include "reader.h"
using namespace std;

int main(int argc, char** argv) {

  auto reader = new AMOS::Reader(cin);

  while (reader->has_next()) {
    reader->next_read();
  }
}
