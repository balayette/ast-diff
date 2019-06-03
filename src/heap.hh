#pragma once

#include <memory>
#include <queue>

#include "tree.hh"

class Heap {
public:
  Heap() = default;

  void Push(Tree *t);
  int PeekMax() const;
  Tree::vecptr Pop();
  void Open(const Tree *t);

  size_t size() const;

private:
  struct compare_tree {
    bool operator()(const Tree *t1, const Tree *t2) {
      return t1->GetHeight() < t2->GetHeight();
    }
  };

  std::priority_queue<Tree *, Tree::vecptr, compare_tree> heap_;
};
