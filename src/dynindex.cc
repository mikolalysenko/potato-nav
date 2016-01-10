

#include "graph.h"
#include "csr.h"
#include "dynindex.h"

using namespace SPUD;

DynamicIndex* DynamicIndex::create(CSRGraph* adj, CSRGraph* transpose) {
  auto index = new DynamicIndex(adj, transpose);

  //Create vertices


  return NULL;
}

//Add vertex to label set
Energy DynamicIndex::addLabels(VertexId root) {
  return 0;
}
