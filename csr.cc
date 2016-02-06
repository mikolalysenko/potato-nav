#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>

#include <stdio.h>

#include "csr.h"

using namespace SPUD;

CSRGraph::CSRGraph(int fd_, void* data_, size_t dataLength_)
  : fd(fd_),
    dataLength(dataLength_),
    data(data_) {
  auto ptr = static_cast<int8_t*>(data);

  header = reinterpret_cast<CSRHeader*>(ptr);

  ptr += sizeof(CSRHeader);
  verts = reinterpret_cast<CSRVertex*>(ptr);

  ptr += sizeof(CSRVertex) * (header->numVerts+1);
  arcs = reinterpret_cast<CSRArc*>(ptr);
}

CSRGraph* CSRGraph::create(
  const char* path,
  int64_t numVerts,
  int64_t numArcs) {

  //Compute size of file
  auto size =
    sizeof(CSRHeader) +
    sizeof(CSRVertex) * (numVerts+1) +
    sizeof(CSRArc)    * numArcs;

  //Dummy fd
  int fd = -1;
  void* data = NULL;

  if(path) {
    auto mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    fd = ::open(
      path,
      O_RDWR|O_CREAT|O_TRUNC,
      mode);
    if(fd < 0) {
      return NULL;
    }

    //reserve space
    ftruncate(fd, size);

    //mmap that sucker
    data = ::mmap(
      NULL,
      size,
      PROT_READ|PROT_WRITE,
      MAP_SHARED,
      fd,
      0);
    if(data == MAP_FAILED) {
      return NULL;
    }
  } else {
    data = malloc(size);
  }

  auto header = static_cast<CSRHeader*>(data);
  header->numVerts = numVerts;
  header->numArcs  = numArcs;

  return new CSRGraph(fd, data, size);
}


CSRGraph* CSRGraph::read(const char* path) {
  auto fd = ::open(path, O_RDONLY);
  if(fd < 0) {
    return NULL;
  }

  struct stat st;
  if(::fstat(fd, &st) != 0) {
    return NULL;
  }

  auto size = st.st_size;

  void* data = ::mmap(
    NULL,
    size,
    PROT_READ,
    MAP_PRIVATE,
    fd,
    0);
  if(data == MAP_FAILED) {
    return NULL;
  }

  return new CSRGraph(fd, data, size);
}

void CSRGraph::close() {
  if(fd < 0) {
    free(data);
  } else {
    ::msync(data, dataLength, MS_SYNC);
    ::munmap(data, dataLength);
    ::close(fd);
  }

  header      = NULL;
  verts       = NULL;
  arcs        = NULL;
  fd          = -1;
  data        = NULL;
  dataLength  = 0;
}

void CSRGraph::print() {
  auto numVerts = header->numVerts;

  std::cout << numVerts << std::endl << std::flush;

  for(int64_t i=0; i<numVerts; ++i) {
    auto begin = verts[i].offset;
    auto end = verts[i+1].offset;

    for(int64_t j=begin; j<end; ++j) {
      std::cout
        << i << ' ' << arcs[j].target << ' ' << arcs[j].cost << std::endl;
    }
  }
}

CSRGraph* CSRGraph::fromArcList(
  const char* path,
  int64_t numVerts,
  int64_t numArcs,
  Arc* arcs) {

  //Sort arcs lexicographically
  std::sort(arcs, arcs + numArcs,
    [](const Arc& a, const Arc& b) {
      if(a.source < b.source) {
        return true;
      } else if(a.source > b.source) {
        return false;
      }
      return a.target < b.target;
    });

  //Allocate graph
  auto graph = CSRGraph::create(path, numVerts, numArcs);
  if(!graph) {
    return NULL;
  }

  auto g_verts = graph->verts;
  auto g_arcs  = graph->arcs;

  //Scan vertices and copy to table
  VertexId prev = -1;
  for(int64_t i=0; i<numArcs; ++i) {
    auto x = arcs[i];
    while(prev < x.source) {
      g_verts[++prev].offset = i;
    }
    g_arcs[i].target = x.target;
    g_arcs[i].cost = x.cost;
  }

  while(prev < numVerts) {
    g_verts[++prev].offset = static_cast<int64_t>(numArcs);
  }

  return graph;
}

CSRGraph* CSRGraph::transpose(const char* path, CSRGraph* graph) {
  auto nv = graph->numVerts();
  auto na = graph->numArcs();

  auto output = create(path, nv, na);

  auto oVerts = output->verts;
  memset(oVerts, 0, sizeof(CSRVertex) * (nv + 1));

  for(int64_t i=0; i<nv; ++i) {
    for(auto arc=graph->arcBegin(i); arc<graph->arcEnd(i); ++arc) {
      ++oVerts[arc->target].offset;
    }
  }

  for(int64_t i=1; i<=nv; ++i) {
    oVerts[i].offset += oVerts[i-1].offset;
  }

  auto oArcs = output->arcs;
  for(int64_t i=nv-1; i>=0; --i) {
    for(auto arc=graph->arcBegin(i); arc<graph->arcEnd(i); ++arc) {
      auto oArc = &oArcs[--oVerts[arc->target].offset];
      oArc->target = static_cast<VertexId>(i);
      oArc->cost = arc->cost;
    }
  }

  return output;
}
