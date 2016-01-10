//Simple data structure for memory mapped sparse graphs
#ifndef CSR_H
#define CSR_H

#include <cstdint>

typedef int64_t CSRInt;
typedef int64_t CSRVertexId;
typedef double  CSRWeight;

#pragma pack(1)
struct CSRHeader {
  CSRInt numVerts;
  CSRInt numArcs;
};

struct CSRVertex {
  CSRInt offset;
};

struct CSRArc {
  CSRVertexId target;
  CSRWeight weight;
};
#pragma pack()

struct CSRGraph {
  CSRHeader* header;
  CSRVertex* verts;
  CSRArc*    arcs;

  int     fd;
  size_t  dataLength;
  void*   data;

  CSRInt numVerts() const { return header->numVerts; }
  CSRInt numArcs() const { return header->numArcs; }

  //Arc data for vertex i
  CSRArc* arcBegin(CSRInt i) { return arcs + verts[i].offset; }
  CSRArc* arcEnd(CSRInt i) { return arcs + verts[i+1].offset; }

  void transpose(CSRGraph* output);

  void close();

  static CSRGraph* create(const char*, int64_t numVerts, int64_t numArcs);
  static CSRGraph* read(const char*);

private:
  CSRGraph(int fd, void* data, size_t dataLength);
};

#endif
