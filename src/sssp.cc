#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "csr.h"

int main(int argc, char** argv) {
  if(argc != 3) {
    return -1;
  }

  auto graph = CSRGraph::read(argv[1]);
  auto start = atoi(argv[2]);

  auto numVerts = graph->numVerts();
  auto numArcs = graph->numArcs();

  auto parent = std::vector<CSRVertexId>(numVerts);
  std::fill(parent.begin(), parent.end(), -1);

  auto distance = std::vector<CSRWeight>(numVerts);
  std::fill(distance.begin(), distance.end(), INFINITY);

  auto to_visit = std::vector<CSRVertexId>(numVerts);
  int head = 0;
  int tail = 0;

  to_visit[tail++] = start;
  distance[start] = 0;
  parent[start] = start;

  while(head < tail) {
    //Instead of using a priority queue we do a linear scan, since
    //the to_visit set is typically pretty small
    auto x = std::min_element(
      to_visit.begin() + head,
      to_visit.begin() + tail);
    auto v = *x;
    *x = to_visit[head++];
    auto w = distance[v];

    for(auto arc = graph->arcBegin(v); arc < graph->arcEnd(v); ++arc) {
      auto u = arc->target;
      auto d = arc->weight + w;

      if(d < distance[u]) {
        if(distance[u] == INFINITY) {
          to_visit[tail++] = u;
        }
        distance[u] = d;
        parent[u] = v;
      }
    }
  }

  for(int i=0; i<numVerts; ++i) {
    std::cout << parent[i] << ' ' << distance[i] << std::endl;
  }

  graph->close();

  return 0;
}
