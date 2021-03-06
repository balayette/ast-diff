#include "heap.hh"

void Heap::Push(Tree *t) { heap_.push(t); }

int Heap::PeekMax() const {
  if (heap_.size() == 0)
    return -1;
  return heap_.top()->GetHeight();
}

Tree::vecptr Heap::Pop() {
  if (heap_.size() == 0)
    throw "Pop called on empty heap.";

  Tree::vecptr ret;

  for (size_t max = heap_.top()->GetHeight();
       heap_.size() > 0 && heap_.top()->GetHeight() == max; heap_.pop())
    ret.push_back(heap_.top());

  return ret;
}

void Heap::Open(const Tree *t) {
  for (auto *it : t->GetChildren())
    heap_.push(it);
}

size_t Heap::size() const { return heap_.size(); }
