#pragma once

#include <memory>
#include <queue>

#include "tree.hh"

class Heap {
public:
  Heap() = default;

  void Push(Tree *t);
  int PeekMax();
  Tree::vecptr Pop();
  void Open(Tree *t);

  int size();

private:
  struct compare_tree {
    bool operator()(Tree *t1, Tree *t2) {
      return t1->GetHeight() < t2->GetHeight();
    }
  };

  std::priority_queue<Tree *, Tree::vecptr, compare_tree> heap_;
};
