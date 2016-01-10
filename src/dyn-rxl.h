#ifndef DYN_RXL_H
#define DYN_RXL_H

#include <vector>

#include "csr.h"
#include "rxl.h"

typedef std::vector<RXLLabel> DRXLVertexLabel;

struct DRXLSampleTree {

};

struct DRXLIndex {
  std::vector<DRXLVertexLabel> vertexLabels;

  std::vector<DRXLSampleSets> samples;

  //Retrieves vertex priority
  int64_t vertexPriority(CSRVertexId v);

};

#endif
