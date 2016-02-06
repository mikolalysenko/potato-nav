#include <algorithm>
#include <vector>
#include <iostream>

#include "graph.h"
#include "csr.h"
#include "dynindex.h"
#include "crawler.h"

using namespace SPUD;

template<typename T> bool labelsIntersect(const T& a, const T& b) {
  int64_t aSize = a.size();
  int64_t bSize = b.size();
  int64_t aPtr = 0;
  int64_t bPtr = 0;
  while(aPtr < aSize && bPtr < bSize) {
    auto aHead = a[aPtr].hub;
    auto bHead = b[bPtr].hub;
    if(aHead < bHead) {
      aPtr++;
    } else if(aHead > bHead) {
      bPtr++;
    } else {
      return true;
    }
  }
  return false;
}

template<typename T> void insertLabel(
  T& labels,
  VertexId hub,
  Cost cost) {
  DynamicLabel label = { hub, cost };
  labels.insert(
    std::lower_bound(
      labels.begin(),
      labels.end(),
      label,
      [](const auto& a, const auto& b) {
        return a.hub < b.hub;
      }),
    label);
}

void DynamicIndex::print() {
  for(int64_t i=0; i<numVerts(); ++i) {
    std::cout << i << ':' << std::endl;
    for(auto label : vertexLabels[i].inLabels) {
      std::cout << "\t-> " << label.hub << '\t' << label.cost << std::endl;
    }
    for(auto label : vertexLabels[i].outLabels) {
      std::cout << "\t<- " << label.hub << '\t' << label.cost << std::endl;
    }
  }
}

DynamicIndex* DynamicIndex::create(CSRGraph* adj, CSRGraph* transpose) {
  auto index = new DynamicIndex(adj, transpose);

  Crawler crawler(index->numVerts());

  auto crawl = [&](auto matrix, auto start, auto visit) {
    crawler.crawl(start, [&](VertexId v, Priority p) {
      if(visit(v, p)) {
        auto arcBegin = matrix->arcBegin(v);
        auto arcEnd = matrix->arcEnd(v);
        for(auto arc=arcBegin; arc<arcEnd; ++arc) {
          auto u = arc->target;
          auto l = arc->cost;
          if(crawler.visited(u)) {
            continue;
          }
          crawler.push(u, l + p);
        }
      }
    });
  };

  auto addForwardLabels = [&](VertexId start) {
    crawl(adj, start, [&](VertexId v, Priority p) {
      if(labelsIntersect(index->vertexLabels[start].outLabels,
                         index->vertexLabels[v].inLabels)) {
        return false;
      }
      insertLabel(index->vertexLabels[v].inLabels, start, p);
      return true;
    });
  };

  auto addBackwardLabels = [&](VertexId start) {
    crawl(transpose, start, [&](VertexId v, Priority p) {
      if(labelsIntersect(index->vertexLabels[start].inLabels,
                         index->vertexLabels[v].outLabels)) {
        return false;
      }
      insertLabel(index->vertexLabels[v].outLabels, start, p);
      return true;
    });
  };

  //Create vertices
  auto numVerts = index->numVerts();
  for(int64_t i=0; i<numVerts; ++i) {
    //TODO: Select next best vertex using some criteria

    addForwardLabels(i);
    addBackwardLabels(i);
  }

  return index;
}
