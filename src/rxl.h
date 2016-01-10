#ifndef RXL_H
#define RXL_H

#include <cstdint>

#include "csr.h"

#pragma pack(1)

typedef CSRVertexId RXLVertexId;
typedef CSRWeight   RXLWeight;

struct RXLLabel {
  RXLVertexId vertex;
  RXLWeight   weight;
};

struct RXLLabelSet {
  int64_t offset;
  int64_t count;
};

struct RXLHeader {
  int64_t numVerts;
  int64_t numLabels;
};

#pragma pack()

struct RXLStaticIndex {
  RXLHeader*    header;
  RXLLabelSet*  labelSets;
  RXLLabel*     labels;

  int fd;
  void* data;
  size_t dataLength;

  int64_t numVerts() const { return header->numVerts; }

  RXLLabel* labelBegin(RXLVertexId v) const {
    return labels + labelSets[v].offset;
  }

  RXLLabel* labelEnd(RXLVertexId v) const {
    return labels + (labelSets[v].offset + labelSets[v].count);
  }

  RXLWeight distance(RXLVertexId s, RXLVertexId t) const;

  RXLStaticIndex(int fd, void* data, size_t length);

  void close();

  static RXLStaticIndex* read(const char* path);
};


#endif
