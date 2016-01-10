#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>

#include "csr.h"

using namespace SPUD;

struct InputArc {
  VertexId s, t;
  ArcWeight w;
};

int main(int argc, char** argv) {
  if(argc != 2) {
    std::cout << "Please specify output file" << std::endl;
    return -1;
  }

  int64_t numVerts;
  std::cin >> numVerts;

  //Read in arcs
  InputArc inArc;
  std::vector<InputArc> inArcs;
  while(std::cin >> inArc.s >> inArc.t >> inArc.w) {
    inArcs.push_back(inArc);
  }

  std::cout
    << "read in " << inArcs.size() << " arcs" << std::endl
    << "sorting..." << std::endl;

  //Sort
  std::sort(inArcs.begin(), inArcs.end(),
    [](const InputArc& a, const InputArc& b) {
      if(a.s < b.s) {
        return true;
      } else if(a.s == b.s) {
        return a.t < b.t;
      }
      return false;
    });

  std::cout << "allocating graph" << std::endl;

  //Allocate graph
  auto graph = CSRGraph::create(argv[1], numVerts, inArcs.size());

  if(!graph) {
    std::cout << "error allocating graph" << std::endl;
    return -1;
  }

  auto verts = graph->verts;
  auto arcs  = graph->arcs;

  //Scan vertices and copy to table
  std::cout << "scanning arcs" << std::endl;
  VertexId prev = -1;
  for(int64_t i=0; i<static_cast<int64_t>(inArcs.size()); ++i) {
    auto x = inArcs[i];
    while(prev < x.s) {
      verts[++prev].offset = i;
    }
    arcs[i].target = x.t;
    arcs[i].weight = x.w;
  }

  while(prev <= numVerts) {
    verts[++prev].offset = static_cast<int64_t>(inArcs.size());
  }

  //Close and flush to disk
  std::cout << "flush results to disk" << std::endl;
  graph->close();

  //done
  std::cout << "done!" << std::endl;

  return 0;
}
