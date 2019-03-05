#include "tree.hh"

template <typename Func>
void Tree::PreorderTraversal(Func f)
{
	f(this);

	for (auto& it : children_)
		it->PreorderTraversal(f);
}

template <typename Func>
void Tree::PostorderTraversal(Func f)
{
	for (auto& it : children_)
		it->PreorderTraversal(f);

	f(this);
}

template <typename Pred>
std::shared_ptr<Tree> FindIf(std::shared_ptr<Tree>& t, Pred f)
{
	if (f(t))
		return t;

	auto& children = t->GetChildren();
	if (children.size() == 0)
		return nullptr;

	for (const auto& it : children)
	{
		auto r = FindIf(it, f);
		if (r != nullptr)
			return r;
	}

	return nullptr;
}

template <typename Pred>
void findAll(std::shared_ptr<Tree>& t, Pred f,
	     std::vector<std::shared_ptr<Tree>>& v)
{
	if (f(t))
		v.push_back(t);

	for (auto& it : t->GetChildren())
		findAll(it, f, v);
}

template <typename Pred>
std::vector<std::shared_ptr<Tree>> FindAll(std::shared_ptr<Tree>& t, Pred f)
{
	std::vector<std::shared_ptr<Tree>> v;

	findAll(t, f, v);

	return v;
}
