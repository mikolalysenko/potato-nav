#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>

#include "csr.h"

int main(int argc, char** argv) {
  if(argc != 2) {
    std::cout << "Please specify output file" << std::endl;
    return -1;
  }

  auto graph = CSRGraph::read(argv[1]);

  auto numVerts = graph->header->numVerts;
  auto numArcs = graph->header->numArcs;

  auto arcs = graph->arcs;

  std::cout << numVerts << std::endl << std::flush;

  for(int i=0; i<numVerts; ++i) {
    auto begin = graph->verts[i].offset;
    auto end = graph->verts[i+1].offset;

    for(int j=begin; j<end; ++j) {
      std::cout
        << i << ' ' << arcs[j].target << ' ' << arcs[j].weight << std::endl;
    }
  }

  graph->close();

  return 0;
}
