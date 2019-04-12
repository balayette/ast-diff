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
  Mappings M(T1->GetRightMostDesc() + 1,
             T1->GetRightMostDesc() - T1->GetLeftMostDesc() + 1);

  L1.Push(T1);
  L2.Push(T2);

  for (auto min = std::min(L1.PeekMax(), L2.PeekMax());
       min != -1 && min > MIN_HEIGHT;
       min = std::min(L1.PeekMax(), L2.PeekMax())) {

    auto m1 = L1.PeekMax();
    auto m2 = L2.PeekMax();
    if (m1 > m2) {
      for (auto t : L1.Pop())
        L1.Open(t);
    }
    if (m1 < m2) {
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

      M.AddMapping(t1, t2);
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

double dice(Tree *t1, Tree *t2, Mappings &M) {
  auto nbr = 0;

  for (auto *desc : GetDescendants(t1)) {
    auto *dest = M.GetDestination(desc);
    if (!dest)
      continue;
    if (!dest->IsDescendantOf(t2))
      continue;

    nbr++;
  }

  double d = ((double)nbr * 2.0) /
             (GetDescendants(t1).size() + GetDescendants(t2).size());
  return d;
}

Tree *candidate(Tree *t1, Tree *T2, Mappings &M) {
  Tree *best = nullptr;
  double bestSim = 0.0;

  T2->PreorderTraversal([&](Tree *c) {
    if (c->GetValue() != t1->GetValue())
      return;
    if (M.ContainsDestinationMapping(c))
      return;

    double s = dice(t1, c, M);
    if (s > bestSim) {
      bestSim = s;
      best = c;
    }
  });

  return best;
}

void bottomUp(Tree *T1, Tree *T2, Mappings &M) {
  Tree::vecptr descendantsPost;
  T1->PostorderTraversal([&](Tree *curr) {
    if (M.ContainsSourceMapping(curr))
      return;

    for (auto &child : curr->GetChildren()) {
      if (!M.ContainsSourceMapping(child.get()))
        continue;

      descendantsPost.push_back(curr);
      return;
    }
  });

  for (auto *t1 : descendantsPost) {
    auto *t2 = candidate(t1, T2, M);
    if (!t2) {
      continue;
    }
    M.AddMapping(t1, t2);
    // FIXME: No optimal mappings.
  }
}

Mappings Gumtree(Tree *T1, Tree *T2) {
  auto mappings = topDown(T1, T2);
  bottomUp(T1, T2, mappings);
  std::cout << "Similarity: "
            << (2 * mappings.size() /
                ((double)GetDescendants(T1).size() + 1 +
                 GetDescendants(T2).size() + 1))
            << '\n';
  return mappings;
}

