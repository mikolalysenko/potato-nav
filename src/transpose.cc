#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>

#include "csr.h"

int main(int argc, char** argv) {
  if(argc != 3) {
    return -1;
  }

  auto input = CSRGraph::read(argv[1]);
  if(input == NULL) {
    return -1;
  }

  auto numVerts = input->header->numVerts;
  auto numArcs = input->header->numArcs;

  auto output = CSRGraph::create(argv[2], numVerts, numArcs);
  if(output == NULL) {
    return -1;
  }

  input->transpose(output);

  input->close();
  output->close();

  return 0;
}
