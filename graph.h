#pragma once

#include <cstdint>

namespace SPUD {
  typedef int64_t VertexId;
  typedef float Cost;

  struct Arc {
    VertexId source, target;
    Cost cost;
  };
}
