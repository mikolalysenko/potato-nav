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

  int64_t numVerts;
  std::cin >> numVerts;

  //Read in arcs
  Arc inArc;
  std::vector<Arc> inArcs;
  while(std::cin >> inArc.source >> inArc.target >> inArc.cost) {
    inArcs.push_back(inArc);
  }

  //Allocate graph
  auto graph = CSRGraph::fromArcList(
    argv[1],
    numVerts,
    inArcs.size(),
    &inArcs[0]);
  if(!graph) {
    return -1;
  }
  graph->close();

  //done
  return 0;
}
