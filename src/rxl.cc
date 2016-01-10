#include <cmath>

#include "csr.h"
#include "rxl.h"

RXLWeight RXLStaticIndex::distance(RXLVertexId s, RXLVertexId t) const {
  if(s == t) {
    return 0;
  }

  auto sPtr = labelBegin(s);
  auto sEnd = labelEnd(s);
  auto tPtr = labelBegin(t);
  auto tEnd = labelEnd(t);

  while(sPtr < sEnd && tPtr < tEnd) {
    auto sv = sPtr->vertex;
    auto tv = tPtr->vertex;

    if(sv < tv) {
      sPtr++;
    } else if(sv > tv) {
      tPtr++;
    } else {
      return sPtr->weight + tPtr->weight;
    }
  }

  return INFINITY;
}

RXLStaticIndex::RXLStaticIndex(int fd_, void* data_, size_t length_)
  : fd(fd_),
    data(data_),
    dataLength(length_) {

  char* ptr = data;
  header = reinterpret_cast<RXLHeader*>(ptr);

  ptr += sizeof(RXLHeader);
  labelSets = reinterpret_cast<RXLLabelSet*>(ptr);

  ptr += sizeof(RXLLabelSet) * header->numVerts;
  labels = reinterpret_cast<RXLLabel*>(ptr);
}

void RXLStaticIndex::close() {
  ::munmap(data, dataLength);
  ::close(fd);

  header      = NULL;
  labelSet    = NULL;
  labels      = NULL;
  fd          = -1;
  data        = NULL;
  dataLength  = 0;
}

RXLStaticIndex* RXLStaticIndex::read(const char* path) {
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

  return new RXLStaticIndex(fd, data, size);
}
