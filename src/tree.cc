#include "tree.hh"

Tree::Tree() : value_(""), parent_(nullptr) {}

Tree::Tree(std::string& value, Tree* parent) : value_(value), parent_(parent) {}

void Tree::AddChild(std::shared_ptr<Tree>& tree) {
	tree->parent_ = this;
	children_.push_back(tree);
}

std::ostream& Tree::Print(std::ostream& stream)
{
	stream << '(' << value_;
	for (auto it = children_.begin(); it != children_.end(); it++)
	{
		stream << ' ';
		(*it)->Print(stream);
	}
	stream << ')';

	return stream;
}

size_t Tree::ChildrenSize()
{
	return children_.size();
}

std::vector<std::shared_ptr<Tree>>& Tree::GetChildren()
{
	return children_;
}

void Tree::SetValue(std::string& value)
{
	value_ = value;
}

std::string& Tree::GetValue()
{
	return value_;
}
