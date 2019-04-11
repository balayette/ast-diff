#include "algo.hh"

#include <algorithm>
#include <iostream>

#include "heap.hh"
#include "utils.hh"

#define MIN_HEIGHT (1)

void mapDescendants(Tree *t1, Tree *t2, Mappings &M) {
  for (auto &child : t1->GetChildren()) {
    auto *matching = child->FindIsomorphicChild(t2);
    M.AddMapping(child.get(), matching);
    mapDescendants(child.get(), matching, M);
  }
}

Mappings topDown(Tree *T1, Tree *T2) {
  Heap L1, L2;
  Mappings M;

  L1.Push(T1);
  L2.Push(T2);

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

      mapDescendants(t1, t2, M);
    });

    for (auto *tree : H1) {
      if (!M.ContainsSourceMapping(tree))
        L1.Open(tree);
    }

    for (auto *tree : H2) {
      if (!M.ContainsDestinationMapping(tree))
        L2.Open(tree);
    }
  }
  return M;
}

void bottomUp(Tree *T1, Tree *T2, Mappings &M) {
  (void)T1;
  (void)T2;
  (void)M;
}

Mappings Gumtree(Tree *T1, Tree *T2) {
  auto mappings = topDown(T1, T2);
  bottomUp(T1, T2, mappings);
  return mappings;
}

