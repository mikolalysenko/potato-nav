#include <iostream>

#include "graph.h"
#include "csr.h"
#include "dynindex.h"
#include "generate.h"

using namespace SPUD;

int main(int argc, char** argv) {
  auto graph  = Generate::ring(NULL, 8);
  std::cout << "graph: " << std::endl;
  graph->print();

  auto graphT = CSRGraph::transpose(NULL, graph);
  std::cout << "graphT: " << std::endl;
  graphT->print();

  /*
  auto index = DynamicIndex::create(graph, graphT);
  std::cout << "index: " << std::endl;
  index->print();
  */

  return 0;
}
