#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

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

  auto mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  auto fd = ::open(
    path,
    O_RDWR|O_CREAT|O_TRUNC,
    mode);
  if(fd < 0) {
    return NULL;
  }

  //Compute size of file
  auto size =
    sizeof(CSRHeader) +
    sizeof(CSRVertex) * (numVerts+1) +
    sizeof(CSRArc)    * numArcs;

  //reserve space
  ftruncate(fd, size);

  //mmap that sucker
  void* data = ::mmap(
    NULL,
    size,
    PROT_READ|PROT_WRITE,
    MAP_SHARED,
    fd,
    0);
  if(data == MAP_FAILED) {
    return NULL;
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

void CSRGraph::transpose(CSRGraph* output) {
  auto nv = numVerts();

  auto oVerts = output->verts;
  memset(oVerts, 0, sizeof(CSRVertex) * (nv + 1));

  for(int64_t i=0; i<nv; ++i) {
    for(auto arc=arcBegin(i); arc<arcEnd(i); ++arc) {
      oVerts[arc->target+1].offset++;
    }
  }

  for(int64_t i=1; i<=nv; ++i) {
    oVerts[i].offset += oVerts[i-1].offset;
  }

  auto oArcs = output->arcs;
  for(int64_t i=0; i<nv; ++i) {
    for(auto arc=arcBegin(i); arc<arcEnd(i); ++arc) {
      auto oArc = &oArcs[oVerts[arc->target].offset++];
      oArc->target = static_cast<VertexId>(i);
      oArc->cost = arc->cost;
    }
  }

  for(int64_t i=nv; i>0; --i) {
    oVerts[i].offset = oVerts[i-1].offset;
  }

  oVerts[0].offset = 0;
}

void CSRGraph::close() {
  ::msync(data, dataLength, MS_SYNC);
  ::munmap(data, dataLength);
  ::close(fd);

  header      = NULL;
  verts       = NULL;
  arcs        = NULL;
  fd          = -1;
  data        = NULL;
  dataLength  = 0;
}
