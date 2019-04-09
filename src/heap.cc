#include "heap.hh"

void Heap::Push(Tree &t) { heap_.push(t); }

int Heap::PeekMax() {
  if (heap_.size() == 0)
    return -1;
  return heap_.top().get().GetHeight();
}

Tree::vecref Heap::Pop() {
  if (heap_.size() == 0)
    throw "Pop called on empty heap.";

  Tree::vecref ret;

  for (int max = heap_.top().get().GetHeight();
       heap_.size() > 0 && heap_.top().get().GetHeight() == max; heap_.pop())
    ret.push_back(heap_.top());

  return ret;
}

void Heap::Open(Tree &t) {
  for (const auto &it : t.GetChildren())
    heap_.push(*it);
}

int Heap::size() { return heap_.size(); }
