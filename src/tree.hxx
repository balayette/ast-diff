#include "tree.hh"

template <typename Func>
void Tree::PreorderTraversal(Func f)
{
	f(*this);

	for (auto& it : children_)
		it->PreorderTraversal(f);
}

template <typename Func>
void Tree::PostorderTraversal(Func f)
{
	for (auto& it : children_)
		it->PreorderTraversal(f);

	f(*this);
}

template <typename Pred>
Tree::optref FindIf(Tree& t, Pred f)
{
	if (f(t))
		return t;

	auto& children = t.GetChildren();
	if (children.size() == 0)
		return std::nullopt;

	for (const auto& it : children)
	{
		auto r = FindIf(*it, f);
		if (r != std::nullopt)
			return r;
	}

	return std::nullopt;
}

template <typename Pred>
void findAll(Tree& t, Pred f,
	     Tree::vecref& v)
{
	if (f(t))
		v.push_back(t);

	for (auto& it : t.GetChildren())
		findAll(*it, f, v);
}

template <typename Pred>
Tree::vecref FindAll(Tree& t, Pred f)
{
	Tree::vecref v;

	findAll(t, f, v);

	return v;
}
