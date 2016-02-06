#pragma once

#include <cassert>
#include <vector>

#include "graph.h"
#include "csr.h"

namespace SPUD {
  struct DynamicLabel {
    VertexId  hub;
    Cost cost;
  };

  struct DynamicLabelSet {
    std::vector<DynamicLabel> inLabels, outLabels;
  };

  struct DynamicIndex {
    std::vector<DynamicLabelSet> vertexLabels;

    CSRGraph* forward;
    CSRGraph* transpose;

    int64_t numVerts() const { return (int64_t)vertexLabels.size(); }

    void print();

    //Creates a dynamic index from a pair of CSR graphs
    static DynamicIndex* create(CSRGraph* adj, CSRGraph* transp);

    //Compute distance between two vertices
    Cost distance(VertexId s, VertexId t) const;

  private:
    DynamicIndex(
      CSRGraph* forward_,
      CSRGraph* transpose_) :
        vertexLabels(forward_->numVerts()),
        forward(forward_),
        transpose(transpose_) {
          assert(forward_->numVerts() == transpose_->numVerts());
          assert(forward_->numArcs() == transpose_->numArcs());
        }
  };
}
