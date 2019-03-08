#pragma once

#include <memory>
#include <queue>

#include "tree.hh"

class Heap
{
    public:
	Heap() = default;

	void Push(Tree::ptr t);
	int PeekMax();
	Tree::vecptr Pop();
	void Open(Tree::ptr t);

	int size();

    private:
	struct compare_tree
	{
		bool operator()(const Tree::ptr& t1, const Tree::ptr& t2)
		{
			return t1->GetHeight() < t2->GetHeight();
		}
	};

	std::priority_queue<Tree::ptr, Tree::vecptr, compare_tree> heap_;
};
