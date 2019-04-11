#include "tree.hh"

template <typename Func> void Tree::PreorderTraversal(Func f) {
  f(this);

  for (auto &it : children_)
    it->PreorderTraversal(f);
}

template <typename Func> void Tree::PostorderTraversal(Func f) {
  for (auto &it : children_)
    it->PostorderTraversal(f);

  f(this);
}

template <typename Pred> Tree *FindIf(Tree *t, Pred f) {
  if (f(t))
    return t;

  auto &children = t->GetChildren();
  if (children.size() == 0)
    return nullptr;

  for (auto &it : children) {
    auto *r = FindIf(it.get(), f);
    if (r)
      return r;
  }

  return nullptr;
}

template <typename Pred> void findAll(Tree *t, Pred f, Tree::vecptr v) {
  if (f(t))
    v.push_back(t);

  for (auto &it : t->GetChildren())
    findAll(it.get(), f, v);
}

template <typename Pred> Tree::vecptr FindAll(Tree *t, Pred f) {
  Tree::vecptr v;

  findAll(t, f, v);

  return v;
}
