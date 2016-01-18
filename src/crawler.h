#pragma once

#include "graph.h"

namespace SPUD {

  typedef float Priority;

  struct Crawler {
    int64_t numVerts;
    int64_t counter;
    int64_t head;
    int64_t tail;

    Priority* v_priority;
    VertexId* to_visit;
    int64_t*  offset;
    int64_t*  last_visit;

    Crawler(int64_t numVerts_) :
      numVerts(numVerts_),
      counter(0),
      head(0),
      tail(0),
      v_priority(new Priority[numVerts]),
      to_visit(new VertexId[numVerts]),
      offset(new int64_t[numVerts]),
      last_visit(new int64_t[numVerts]) {
      memset(last_visit, 0, sizeof(int64_t) * numVerts);
    }

    ~Crawler() {
      delete[] priority;
      delete[] to_visit;
      delete[] offset;
      delete[] last_visit;
    }

    bool visited(VertexId v) const {
      return marked(v) && offset[v] < head;
    }

    bool marked(VertexId v) const {
      return last_visit[v] >= counter;
    }

    Priority priority(VertexId v) const {
      return v_priority[offset[v]];
    }

    bool empty() const {
      return tail <= head;
    }

    void push(VertexId v, Priority p) {
      if(last_visit[v] < counter) {
        v_priority[tail] = p;
        to_visit[tail] = v;
        offset[v]      = tail;
        last_visit[v]  = counter;
        tail++;
      } else {
        auto idx = offset[v];
        v_priority[idx] = min(v_priority[idx], p);
      }
    }

    std::pair<VertexId, Priority> pop() {
      Priority min_priority = v_priority[head];
      auto min_index = head;

      //Unless our network is an expander, it is usually faster to do a linear
      //scan than use a priority queue.  If this becomes a problem in bench
      //marks, I'll switch it to use a heap later.
      //
      for(auto i=head+1; i<tail; ++i) {
        auto p = v_priority[i];
        if(p < min_priority) {
          min_priority = p;
          min_index    = i;
        }
      }

      auto v = to_visit[i];
      auto h = to_visit[head];

      to_visit[i]   = h;
      v_priority[i] = v_priority[head];
      offset[h]     = i;

      to_visit[head]   = v;
      v_priority[head] = min_priority;
      offset[v]        = head;

      head++;

      return make_pair(v, min_priority);
    }

    template<typename T> void crawl(VertexId start, T visit) {
      counter++;
      head = tail = 0;
      push(start, 0);
      while(!empty()) {
        auto top = pop();
        visit(top.first, top.second);
      }
    }
  };
};
