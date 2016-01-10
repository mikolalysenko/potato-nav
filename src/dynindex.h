#pragma once

#include <cassert>
#include <vector>

#include "graph.h"
#include "csr.h"

namespace SPUD {
  typedef int64_t Energy;

  struct DynamicLabel {
    VertexId  hub;
    ArcWeight weight;
  };

  struct DynamicLabelSet {
    std::vector<DynamicLabel> labels;
  };

  struct DynamicIndex {
    std::vector<DynamicLabelSet> vertexLabels;

    //Creates a dynamic index from a pair of CSR graphs
    static DynamicIndex* create(CSRGraph* adj, CSRGraph* transp);

    //Compute distance between two vertices
    ArcWeight distance(VertexId s, VertexId t) const;


  private:
    CSRGraph* forward;
    CSRGraph* transpose;

    DynamicIndex(
      CSRGraph* forward_,
      CSRGraph* transpose_) :
        vertexLabels(forward_->numVerts()),
        forward(forward_),
        transpose(transpose_) {
          assert(forward_->numVerts() == transpose_->numVerts());
          assert(forward_->numArcs() == transpose_->numArcs());
        }

    Energy addLabels(VertexId root);
  };
}
