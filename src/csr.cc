#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <stdio.h>

#include "csr.h"

CSRGraph::CSRGraph(int fd_, void* data_, size_t dataLength_)
  : fd(fd_),
    data(data_),
    dataLength(dataLength_) {
  auto ptr = (char*)data;

  header = (CSRHeader*)ptr;

  ptr += sizeof(CSRHeader);
  verts = (CSRVertex*)ptr;

  ptr += sizeof(CSRVertex) * (header->numVerts+1);
  arcs = (CSRArc*)ptr;
}

CSRGraph* CSRGraph::create(
  const char* path,
  CSRInt numVerts,
  CSRInt numArcs) {

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

  auto header = (CSRHeader*)data;
  header->numVerts = numVerts;
  header->numArcs = numArcs;

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
  auto na = numArcs();

  auto oVerts = output->verts;
  memset(oVerts, 0, sizeof(CSRVertex) * (nv + 1));

  for(CSRInt i=0; i<nv; ++i) {
    for(auto arc=arcBegin(i); arc<arcEnd(i); ++arc) {
      oVerts[arc->target+1].offset++;
    }
  }

  for(CSRInt i=1; i<=nv; ++i) {
    oVerts[i].offset += oVerts[i-1].offset;
  }

  auto oArcs = output->arcs;
  for(CSRInt i=0; i<nv; ++i) {
    for(auto arc=arcBegin(i); arc<arcEnd(i); ++arc) {
      auto oArc = &oArcs[oVerts[arc->target].offset++];
      oArc->target = (CSRVertexId)i;
      oArc->weight = arc->weight;
    }
  }

  for(CSRInt i=nv; i>0; --i) {
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
