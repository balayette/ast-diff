#include "tree.hh"

#include <algorithm>

Tree::Tree()
    : value_("")
    , parent_(nullptr)
{
	idx_ = node_count_++;
}

Tree::Tree(std::string& value, Tree* parent)
    : value_(value)
    , parent_(parent)
    , height_(1)
    , depth_(1)
{
	idx_ = node_count_++;
}

void Tree::AddChild(std::shared_ptr<Tree>& tree)
{
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

std::ostream& Tree::PrettyPrint(std::ostream& stream)
{
	for (int i = 0; i < depth_; i++)
		stream << ' ';

	stream << '(' << value_;
	if (children_.size() > 0)
		stream << '\n';

	auto end = children_.end();
	auto last = end - 1;
	for (auto it = children_.begin(); it != end; it++)
	{
		(*it)->PrettyPrint(stream);
		if (it != last)
			stream << '\n';
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

void Tree::SetHeight(int height)
{
	height_ = height;
}

int Tree::GetHeight()
{
	return height_;
}

int Tree::computeHeightDepth(int depth)
{
	depth_ = depth;
	auto max = 0;
	for (auto it = children_.begin(); it != children_.end(); it++)
	{
		auto h = (*it)->computeHeightDepth(depth + 1);
		if (h > max)
			max = h;
	}

	height_ = max + 1;
	return height_;
}

int Tree::ComputeHeightDepth()
{
	return computeHeightDepth(0);
}

void Tree::dumpDot(std::ostream& stream)
{
	stream << '\t' << idx_ << " [label=<" << value_ << ">]\n";

	for (auto& it : children_)
		stream << '\t' << idx_ << " -> " << it->idx_ << '\n';

	stream << '\n';

	for (auto& it : children_)
		it->dumpDot(stream);
}

std::ostream& Tree::DumpDot(std::ostream& stream)
{
	stream << "digraph AST {\n";

	dumpDot(stream);

	return stream << "}\n";
}

bool Tree::IsIsomorphic(std::shared_ptr<Tree>& t)
{
	if (height_ != t->height_)
		return false;

	if (children_.size() != t->children_.size())
		return false;

	if (value_ != t->value_)
		return false;

	for (size_t i = 0; i < children_.size(); i++)
	{
		auto found = std::find_if(
		    t->children_.begin(), t->children_.end(),
		    [&](std::shared_ptr<Tree>& elem) {
			    return elem->value_ == children_[i]->value_;
		    });

		if (found == children_.end())
			return false;

		if (!children_[i]->IsIsomorphic(*found))
			return false;
	}

	return true;
}
