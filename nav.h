#pragma once

#include <vector>
#include <utility>
#include <algorithm>

#include "graph.h"
#include "dynindex.h"

namespace SPUD {
  #pragma pack(1)

  struct NAVHeader {
    int64_t numVerts;
    int64_t numLabels;
  };

  struct NAVVertex {
    int64_t labelOffset;
    int64_t inCount;
    int64_t outCount;
  };

  #pragma pack(0)

  struct NAVHalfLabel {
    int64_t count;
    VertexId* hub;
    Cost* cost;
  };

  struct NAVLabel {
    NAVHalfLabel in, out;
  };

  struct NAVIndex {
    NAVHeader* header;
    NAVVertex* verts;

    NAVLabel* labels;

    int fd;
    size_t dataLength;
    void* data;

    int64_t numVerts() { return header->numVerts; }

    std::pair<VertexId, Cost> hubQuery(VertexId start, VertexId end);

    Cost distance(VertexId start, VertexId end) {
      return hubQuery(start, end).second;
    }


    void print();
    void close();

    static NAVIndex* create(
      const char* path,
      int64_t numVerts,
      int64_t numLabels);
    static NAVIndex* read(const char* path);
    static NAVIndex* fromDynamicIndex(const char* path, DynamicIndex* index);

    ~NAVIndex() {
      delete[] labels;
    }

  private:
    NAVIndex(int fd_, size_t dataLength_, void* data_) :
      fd(fd_),
      dataLength(dataLength_),
      data(data_),
      header(static_cast<NAVHeader*>(data_)),
      verts(reinterpret_cast<NAVVertex*>((char*)data_ + sizeof(NAVHeader))),
      labels(new NAVLabel[header->numVerts]) {}
  };
}
