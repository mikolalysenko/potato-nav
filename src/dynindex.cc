#include "graph.h"
#include "csr.h"
#include "dynindex.h"
#include "crawler.h"

using namespace SPUD;

template<typename T> bool labelsIntersect(const T& from, const T& to) {
  return false;
}

template<typename T> void insertLabel(T& labels, VertexId hub, ArcWeight w) {
}

DynamicIndex* DynamicIndex::create(CSRGraph* adj, CSRGraph* transpose) {

  auto index = new DynamicIndex(adj, transpose);

  Crawler crawler(index->numVerts());

  auto addForwardLabels = [](VertexId start) {
    crawler.crawl(start, [](VertexId v, Priority p) {
      //Check if v is not covered
      if(labelsIntersect(index->vertexLabels[start].outLabels,
                         index->vertexLabels[v].inLabels)) {
        return;
      }

      //Add v to label set and store distance
      insertLabel(index->vertexLabels[v].inLabels, start, p);

      //Scan neighbors of v, append to arc list
      auto arcBegin = adj->arcBegin(v);
      auto arcEnd = adj->arcEnd(v);
      for(auto arc=arcBegin; arc<arcEnd; ++arc) {
        auto u = arc->target;
        auto l = arc->weight;
        if(crawler->visited(u)) {
          continue;
        }
        crawler->push(u, l + p);
      }
    })
  }

  auto addBackwardLabels = [](VertexId start) {
    crawler.crawl(start, [](VertexId v, Priority p) {
      //Check if v is not covered
      if(labelsIntersect(index->vertexLabels[start].inLabels,
                         index->vertexLabels[v].outLabels)) {
        return;
      }

      //Add v to label set and store distance
      insertLabel(index->vertexLabels[v].outLabels, start, p);

      //Scan neighbors of v, append to arc list
      auto arcBegin = transpose->arcBegin(v);
      auto arcEnd = transpose->arcEnd(v);
      for(auto arc=arcBegin; arc<arcEnd; ++arc) {
        auto u = arc->target;
        auto l = arc->weight;
        if(crawler->visited(u)) {
          continue;
        }
        crawler->push(u, l + p);
      }
    })
  }

  //Create vertices
  auto numVerts = index->numVerts;
  for(int64_t i=0; i<numVerts; ++i) {
    //TODO: Select next best vertex using some criteria

    addForwarLabels(i);
    addBackwardLabels(i);
  }

  return index;
}
