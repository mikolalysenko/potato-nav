#include "graph.h"
#include "csr.h"
#include "dynindex.h"
#include "crawler.h"

using namespace SPUD;


DynamicIndex* DynamicIndex::create(CSRGraph* adj, CSRGraph* transpose) {

  auto index = new DynamicIndex(adj, transpose);


  Crawler crawler(index->numVerts());

  auto addLabels = [](VertexId v) {
    crawler.begin();
    crawler.push(v, 0);

    while(!crawler.empty()) {
      
    }

    crawler.end();
  }

  //Create vertices
  auto numVerts = index->numVerts;
  for(int64_t i=0; i<numVerts; ++i) {
    //TODO: Select next best vertex using some criteria


    addLabels(i);
  }

  return index;
}
