#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>

#include "csr.h"

using namespace SPUD;

int main(int argc, char** argv) {
  if(argc != 3) {
    return -1;
  }

  auto input = CSRGraph::read(argv[1]);
  if(input == NULL) {
    return -1;
  }

  auto output = CSRGraph::transpose(argv[2], input);
  if(output == NULL) {
    return -1;
  }

  input->close();
  output->close();

  return 0;
}
