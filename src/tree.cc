#include "tree.hh"

#include <algorithm>

Tree::Tree()
    : value_("")
    , parent_(nullptr)
{
	idx_ = node_count_++;
}

Tree::Tree(std::string& value, Tree& parent)
    : value_(value)
    , parent_(&parent)
    , height_(1)
    , depth_(1)
{
	idx_ = node_count_++;
}

void Tree::AddChild(Tree::ptr& tree)
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

Tree::vecptr& Tree::GetChildren()
{
	return children_;
}

void Tree::SetValue(std::string& value)
{
	value_ = value;
}

Symbol& Tree::GetValue()
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

Tree& Tree::GetParent()
{
	return *parent_;
}

void Tree::SetParent(Tree& p)
{
	parent_ = &p;
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
	std::string out(value_.get());

	for (size_t f = out.find("\"", 0); f != std::string::npos;
	     f = out.find("\"", f + 2))
		out.replace(f, 1, "\\\"");

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

bool Tree::IsIsomorphic(Tree& t)
{
	if (height_ != t.height_)
		return false;

	if (children_.size() != t.children_.size())
		return false;

	if (value_ != t.value_)
		return false;

	if (children_.size() == 0)
		return true;

	for (size_t i = 0; i < children_.size(); i++)
	{
		auto found = std::find_if(
		    t.children_.begin(), t.children_.end(),
		    [&](Tree::ptr& elem) {
			    return elem->value_ == children_[i]->value_;
		    });

		if (found == t.children_.end())
			return false;

		if (!children_[i]->IsIsomorphic(**found))
			return false;
	}

	return true;
}

void getDescendants(Tree& t, Tree::vecref& v)
{
	auto& children = t.GetChildren();
	for (const auto& child : children)
		v.push_back(*child);

	for (auto& it : children)
		getDescendants(*it, v);
}

Tree::vecref GetDescendants(Tree& t)
{
	Tree::vecref v;

	getDescendants(t, v);

	return v;
}

void DumpMapping(std::ostream& stream, Tree& t1, Tree& t2, Mappings& v)
{
	stream << "digraph G {\n\tsubgraph AST1 {\n";
	t1.dumpDot(stream);
	stream << "}\n\tsubgraph AST2 {\n";
	t2.dumpDot(stream);
	stream << "}\n";

	for (auto p : v)
		stream << p.first.get().idx_ << " -> " << p.second.get().idx_
		       << " [fillcolor = blue] [color = blue] [style = dashed] "
			  "[constraint = false];\n";

	stream << "}\n";
}

bool Tree::operator==(const Tree& t)
{
	return this == &t;
}

bool Tree::operator!=(const Tree& t)
{
	return this != &t;
}

bool operator==(std::reference_wrapper<Tree> a, std::reference_wrapper<Tree> b)
{
	return &a == &b;
}
