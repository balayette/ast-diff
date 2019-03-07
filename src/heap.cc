#include "heap.hh"

void Heap::Push(std::shared_ptr<Tree> t)
{
	heap_.push(t);
}

int Heap::PeekMax()
{
	if (heap_.size() == 0)
		return -1;
	return heap_.top()->GetHeight();
}

std::vector<std::shared_ptr<Tree>> Heap::Pop()
{
	if (heap_.size() == 0)
		throw "Pop called on empty heap.";

	std::vector<std::shared_ptr<Tree>> ret;

	for (int max = heap_.top()->GetHeight();
	     heap_.size() > 0 && heap_.top()->GetHeight() == max; heap_.pop())
		ret.push_back(heap_.top());

	return ret;
}

void Heap::Open(std::shared_ptr<Tree> t)
{
	for (auto it : t->GetChildren())
		heap_.push(it);
}

int Heap::size()
{
	return heap_.size();
}
