#include "tree.hh"

template <typename Func>
void Tree::PreorderTraversal(Func f) {
	f(this);

	for (auto& it : children_) it->PreorderTraversal(f);
}

template <typename Func>
void Tree::PostorderTraversal(Func f) {
	for (auto& it : children_) it->PreorderTraversal(f);

	f(this);
}
