#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>

#include "csr.h"

using namespace SPUD;

int main(int argc, char** argv) {
  if(argc != 2) {
    return -1;
  }

  auto graph = CSRGraph::read(argv[1]);
  graph->print();
  graph->close();

  return 0;
}
