#include <cstdint>

#include "graph.h"
#include "csr.h"
#include "generate.h"

using namespace SPUD;
using namespace Generate;

CSRGraph* random(const char* path, int64_t numVerts, int64_t numArcs) {
  const arcs = new Arc[numArcs];
  for(int64_t i=0; i<numArcs; ++i) {
    arcs[i].source = random() % numVerts;
    arcs[i].target = random() % numVerts;
    arcs[i].cost = drand48();
  }
  auto result = CSRGraph::fromArcList(path, numVerts, numArcs, arcs);
  delete[] arcs;
  return result;
}

CSRGraph* ring(int64_t numVerts) {
  const arcs = new Arc[numVerts];
  for(int64_t i=0; i<numVerts; ++i) {
    arcs[i].source = i;
    arcs[i].target = (i + 1) % numVerts;
    arcs[i].cost = 1.0;
  }
  auto result = CSRGraph::fromArcList(path, numVerts, numVerts, arcs);
  delete[] arcs;
  return result;
}

CSRGraph* hub(int64_t numVerts) {
  const arcs = new Arc[numVerts];
  for(int64_t i=1; i<numVerts; ++i) {
    arcs[i].source = 0;
    arcs[i].target = i;
    arcs[i].cost = 1.0;
  }
  auto result = CSRGraph::fromArcList(path, numVerts, numArcs, arcs);
  delete[] arcs;
  return result;
}
