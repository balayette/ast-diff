#pragma once

#include <memory>
#include <queue>
#include "tree.hh"

class Heap {
       public:
	Heap() = default;

	void Push(std::shared_ptr<Tree> t);
	int PeekMax();
	std::vector<std::shared_ptr<Tree>> Pop();
	void Open(std::shared_ptr<Tree> t);

	int size();

       private:
	struct compare_tree {
		bool operator()(const std::shared_ptr<Tree>& t1,
				const std::shared_ptr<Tree>& t2) {
			return t1->GetHeight() < t2->GetHeight();
		}
	};

	std::priority_queue<std::shared_ptr<Tree>,
			    std::vector<std::shared_ptr<Tree>>, compare_tree>
	    heap_;
};
