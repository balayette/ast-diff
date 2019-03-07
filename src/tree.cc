#include "tree.hh"

#include <algorithm>
#include <regex>

Tree::Tree()
    : value_("")
    , parent_(nullptr)
{
	idx_ = node_count_++;
}

Tree::Tree(std::string& value, std::shared_ptr<Tree>& parent)
    : value_(value)
    , parent_(parent)
    , height_(1)
    , depth_(1)
{
	idx_ = node_count_++;
}

void Tree::AddChild(std::shared_ptr<Tree>& tree)
{
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

std::shared_ptr<Tree>& Tree::GetParent()
{
	return parent_;
}

void Tree::SetParent(std::shared_ptr<Tree>& p)
{
	parent_ = p;
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
	std::string out(value_);
	out = std::regex_replace(out, std::regex("\\\""), "\\\"");
	stream << '\t' << idx_ << " [label=\"" << out << "\"];\n";

	for (auto& it : children_)
		stream << '\t' << idx_ << " -> " << it->idx_ << ";\n";

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

	if (children_.size() == 0)
		return true;

	for (size_t i = 0; i < children_.size(); i++)
	{
		auto found = std::find_if(
		    t->children_.begin(), t->children_.end(),
		    [&](std::shared_ptr<Tree>& elem) {
			    return elem->value_ == children_[i]->value_;
		    });

		if (found == t->children_.end())
			return false;

		if (!children_[i]->IsIsomorphic(*found))
			return false;
	}

	return true;
}

void getDescendants(std::shared_ptr<Tree>& t,
		    std::vector<std::shared_ptr<Tree>>& v)
{
	auto& children = t->GetChildren();
	v.insert(v.end(), children.begin(), children.end());
	for (auto& it : children)
		getDescendants(it, v);
}

std::vector<std::shared_ptr<Tree>> GetDescendants(std::shared_ptr<Tree>& t)
{
	std::vector<std::shared_ptr<Tree>> v;

	getDescendants(t, v);

	return v;
}

void DumpMapping(
    std::ostream& stream, std::shared_ptr<Tree>& t1, std::shared_ptr<Tree>& t2,
    std::vector<std::pair<std::shared_ptr<Tree>, std::shared_ptr<Tree>>>& v)
{
	stream << "digraph G {\n\tsubgraph AST1 {\n";
	t1->dumpDot(stream);
	stream << "}\n\tsubgraph AST2 {\n";
	t2->dumpDot(stream);
	stream << "}\n";

	for (auto p : v)
		stream
		    << p.first->idx_ << " -> " << p.second->idx_
		    << " [fillcolor = blue] [color = blue] [style = dashed] [constraint = false];\n";

	stream << "}\n";
}
