#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "csr.h"
#include "crawler.h"

using namespace SPUD;

int main(int argc, char** argv) {
  if(argc != 3) {
    return -1;
  }

  auto graph = CSRGraph::read(argv[1]);
  auto start = atoi(argv[2]);

  auto crawler = new Crawler(graph->numVerts());

  crawler->crawl(start, [&](VertexId v, Priority w) {
    for(auto arc=graph->arcBegin(v), end=graph->arcEnd(v); arc<end; ++arc) {
      crawler->push(arc->target, arc->cost + w);
    }
  });

  for(int64_t i=0; i<graph->numVerts(); ++i) {
    std::cout << crawler->parent(i) << ' ' << crawler->priority(i) << std::endl;
  }

  graph->close();

  return 0;
}
