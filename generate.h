#pragma once

#include <cstdint>
#include "csr.h"

namespace SPUD {
  namespace Generate {
    CSRGraph* random(int64_t numVerts, int64_t numArcs);
    CSRGraph* ring(int64_t numVerts);
    CSRGraph* hub(int64_t numVerts);
  }
}
