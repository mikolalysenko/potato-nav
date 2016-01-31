#pragma once

#include <cstdint>
#include "graph.h"

namespace SPUD {
  #pragma pack(1)
  struct CSRHeader {
    int64_t numVerts;
    int64_t numArcs;
  };

  struct CSRVertex {
    int64_t offset;
  };

  struct CSRArc {
    VertexId target;
    Cost cost;
  };
  #pragma pack()

  struct CSRGraph {
    CSRHeader* header;
    CSRVertex* verts;
    CSRArc*    arcs;

    int     fd;
    size_t  dataLength;
    void*   data;

    int64_t numVerts() const { return header->numVerts; }
    int64_t numArcs() const { return header->numArcs; }

    //Arc data for vertex i
    CSRArc* arcBegin(VertexId i) { return arcs + verts[i].offset; }
    CSRArc* arcEnd(VertexId i) { return arcs + verts[i+1].offset; }

    void transpose(CSRGraph* output);

    void print();
    void close();

    std::ostream& operator<<(std::ostream& in) const;

    static CSRGraph* create(const char*, int64_t numVerts, int64_t numArcs);
    static CSRGraph* read(const char*);

  private:
    CSRGraph(int fd, void* data, size_t dataLength);
  };
}
