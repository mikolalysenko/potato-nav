#pragma once

#include <cstdint>
#include "csr.h"

namespace SPUD {
  namespace Generate {
    CSRGraph* random(const char*, int64_t numVerts, int64_t numArcs);
    CSRGraph* ring(const char*, int64_t numVerts);
    CSRGraph* hub(const char*, int64_t numVerts);
  }
}
