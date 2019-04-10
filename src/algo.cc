#include "algo.hh"

#include <algorithm>
#include <iostream>

#include "heap.hh"
#include "utils.hh"

#define MIN_HEIGHT (1)

Mappings Gumtree(Tree *t1, Tree *t2) {
  Heap L1, L2;
  Mappings M;

  L1.Push(t1);
  L2.Push(t2);

  for (auto min = std::min(L1.PeekMax(), L2.PeekMax());
       min != -1 && min > MIN_HEIGHT;
       min = std::min(L1.PeekMax(), L2.PeekMax())) {
    if (L1.PeekMax() > L2.PeekMax()) {
      for (auto t : L1.Pop())
        L1.Open(t);
    }
    if (L1.PeekMax() < L2.PeekMax()) {
      for (auto t : L2.Pop())
        L2.Open(t);
    }

    auto H1 = L1.Pop();
    auto H2 = L2.Pop();

    foreach_pair(H1, H2, [&](auto *t1, auto *t2) {
      if (!t1->IsIsomorphic(t2))
        return;
      if (M.ContainsSourceMapping(t1))
        return;
      if (M.ContainsDestinationMapping(t2))
        return;

    });

  }
	return M;
}

