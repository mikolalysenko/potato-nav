#pragma once

#include "graph.h"

namespace SPUD {
  struct Crawler {
    int64_t numVerts;
    int64_t counter;
    int64_t head;
    int64_t tail;
    VertexId active;

    Cost* v_cost;
    VertexId* to_visit;
    VertexId* parents;
    int64_t*  offset;
    int64_t*  last_visit;

    Crawler(int64_t numVerts_) :
      numVerts(numVerts_),
      counter(0),
      head(0),
      tail(0),
      active(-1),
      v_cost(new Cost[numVerts]),
      to_visit(new VertexId[numVerts]),
      parents(new VertexId[numVerts]),
      offset(new int64_t[numVerts]),
      last_visit(new int64_t[numVerts]) {
      memset(last_visit, 0, sizeof(int64_t) * numVerts);
    }

    ~Crawler() {
      delete[] v_cost;
      delete[] to_visit;
      delete[] parents;
      delete[] offset;
      delete[] last_visit;
    }

    bool visited(VertexId v) const {
      return marked(v) && offset[v] < head;
    }

    bool marked(VertexId v) const {
      return last_visit[v] >= counter;
    }

    VertexId parent(VertexId v) const {
      return parents[v];
    }

    Cost priority(VertexId v) const {
      return v_cost[offset[v]];
    }

    bool empty() const {
      return tail <= head;
    }

    void push(VertexId v, Cost p) {
      if(last_visit[v] < counter) {
        v_cost[tail] = p;
        to_visit[tail] = v;
        parents[v] = active;
        offset[v] = tail;
        last_visit[v] = counter;
        tail++;
      } else {
        auto idx = offset[v];
        if(v_cost[idx] < p) {
          parents[v] = active;
          v_cost[idx] = p;
        }
      }
    }

    std::pair<VertexId, Cost> pop() {
      Cost min_priority = v_cost[head];
      auto min_index = head;

      //Unless our network is something pathological like an expander, it is
      //usually faster to do a linear scan than use a priority queue.  If this
      //becomes a problem in bench marks, I'll switch it to use a heap later.
      //
      //Road networks aren't expanders usually.
      //
      for(auto i=head+1; i<tail; ++i) {
        auto p = v_cost[i];
        if(p < min_priority) {
          min_priority = p;
          min_index    = i;
        }
      }

      auto v = to_visit[min_index];
      auto h = to_visit[head];

      to_visit[min_index] = h;
      v_cost[min_index] = v_cost[head];
      offset[h] = min_index;

      to_visit[head] = v;
      v_cost[head] = min_priority;
      offset[v] = head;

      head++;

      return std::make_pair(v, min_priority);
    }

    template<typename T> void crawl(VertexId start, T visit) {
      counter++;
      head = tail = 0;
      active = start;
      push(start, 0);
      while(!empty()) {
        auto top = pop();
        active = top.first;
        visit(top.first, top.second);
      }
    }
  };
};
