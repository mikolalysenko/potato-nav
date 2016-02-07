#include <iostream>

#include "nav.h"

using namespace SPUD;

std::pair<VertexId, Cost> NAVIndex::hubQuery(VertexId start, VertexId end) {
  auto out = labels[start].out;
  auto inp = labels[end].in;
  int64_t aPtr = 0, bPtr = 0,
          aCount = out.count, bCount = inp.count;
  VertexId hub = -1;
  Cost distance = INFINITY;
  while(aPtr < aCount && bPtr < bCount) {
    auto aHead = out.hub[aPtr];
    auto bHead = inp.hub[bPtr];
    if(aHead < bHead) {
      aHead++;
    } else if(aHead > bHead) {
      bHead++;
    } else {
      auto d = out.cost[aPtr++] + inp.cost[bPtr++];
      if(d < distance) {
        distance = d;
        hub = aHead;
      }
    }
  }
  return make_pair(hub, distance);
}


void NAVIndex::print()  {
  auto printHalfLabel = [](const char* prefix, auto l) {
    for(int64_t i=0; i<l.count; ++i) {
      std::cout << prefix << l.hub[i] << ' ' << l.cost[i] << std::endl;
    }
  }
  for(int64_t i=0; i<numVerts; ++i) {
    printHalfLabel("\t<- ", labels[i].in);
    printHalfLabel("\t-> ", labels[i].out);
  }
}

void NAVIndex::close() {
  if(fd < 0) {
    free(data);
  } else {
    ::msync(data, dataLength, MS_SYNC);
    ::munmap(data, dataLength);
    ::close(fd);
  }

  fd          = -1;
  data        = NULL;
  dataLength  = 0;
}

NAVIndex* NAVIndex::create(
  const char* path,
  int64_t numVerts,
  int64_t numLabels) {

  //Compute size of file
  auto size =
    sizeof(NAVHeader) +
    sizeof(NAVVertex) * numVerts +
    (sizeof(VertexId) + sizeof(Cost)) * numLabels;

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

  NAVHeader* header = static_cast<NAVHeader*>data;
  header->numVerts = numVerts;
  header->numLabels = numLabels;

  return new NAVIndex(fd, size, data);
}

NAVIndex* NAVIndex::read(const char* path) {
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

  auto result = new NAVIndex(fd, size, data);

  auto numVerts = result->numVerts();
  for(int64_t i=0; i<numVerts; ++i) {
    auto& label = result->labels[i];
    auto offset = result->verts[i];

    auto inCount = offset.inCount;
    auto outCount = offset.outCount;

    char* ptr = data;
    ptr += offset.labelOffset;

    label.in.count = inCount;
    label.in.hub = static_cast<VertexId*>(ptr);
    ptr += inCount * sizeof(VertexId);
    label.in.cost = static_cast<Cost*>(ptr);
    ptr += inCount * sizeof(Cost);

    label.out.count = outCount;
    label.out.hub = static_cast<VertexId*>(ptr);
    ptr += outCount * sizeof(VertexId);
    label.out.cost = static_cast<Cost*>(ptr);
    ptr += outCount * sizeof(Cost);
  }

  return result;
}

NAVIndex* NAVIndex::fromDynamicIndex(const char* path, DynamicIndex* index) {
  int64_t numLabels = 0;
  int64_t numVerts = index->numVerts();
  for(int64_t i=0; i<numVerts; ++i) {
    auto& label = index->vertexLabels[i];
    numLabels += static_cast<int64_t>(
      label.inLabels.size() + label.outLabels.size());
  }

  auto result = create(path, numVerts, numLabels);
  if(!result) {
    return NULL;
  }

  void* ptr = result->data;

  auto fillHalfLabel = [](auto& halfLabel, auto& dynamicLabel) {
    halfLabel.count = dynamicLabel.size();

    VertexId* ids = static_cast<VertexId*>(ptr);
    for(auto l : dynamicLabel) {
      *(ids++) = l.hub;
    }

    Cost* costs = reinterpret_cast<Cost*>(ids);
    for(auto l : dynamicLabel) {
      *(costs++) = l;
    }
    
    ptr = reinterpret_cast<void*>(costs);
  };

  for(int64_t i=0; i<numVerts; ++i) {
    auto& dynLabel = index->vertexLabels[i];
    auto& navLabel = result->labels[i];

    auto& vert = result->verts[i];

    vert.labelOffset = static_cast<int64_t>(ptr - data);
    vert.inCount = dynLabel.inLabels.size();
    vert.outCount = dynLabel.outLabels.size();

    fillHalfLabel(navLabel.in, dynLabel.inLabels);
    fillHalfLabel(navLabel.out, dynLabel.outLabels);
  }

  return result;
}
