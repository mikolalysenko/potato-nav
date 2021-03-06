#include <iostream>

#include "graph.h"
#include "csr.h"
#include "dynindex.h"
#include "nav.h"
#include "generate.h"

using namespace SPUD;

int main(int argc, char** argv) {
  auto graph  = Generate::ring(NULL, 5);
  std::cout << "graph: " << std::endl;
  graph->print();

  auto graphT = CSRGraph::transpose(NULL, graph);
  std::cout << "graphT: " << std::endl;
  graphT->print();

  auto index = DynamicIndex::create(graph, graphT);
  std::cout << "index: " << std::endl;
  index->print();

  auto nav = NAVIndex::fromDynamicIndex(NULL, index);
  std::cout << "nav data: " << std::endl;
  nav->print();

  return 0;
}
