#include "tree.hh"

Tree::Tree() : value_(""), parent_(nullptr) {}

Tree::Tree(std::string& value, Tree* parent)
    : value_(value), parent_(parent), height_(1), depth_(1) {}

void Tree::AddChild(std::shared_ptr<Tree>& tree) {
	tree->parent_ = this;
	children_.push_back(tree);
}

std::ostream& Tree::Print(std::ostream& stream) {
	stream << '(' << value_;
	for (auto it = children_.begin(); it != children_.end(); it++) {
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
	for (auto it = children_.begin(); it != end; it++) {
		(*it)->PrettyPrint(stream);
		if (it != last)
			stream << '\n';
	}

	stream << ')';

	return stream;
}

size_t Tree::ChildrenSize() { return children_.size(); }

std::vector<std::shared_ptr<Tree>>& Tree::GetChildren() { return children_; }

void Tree::SetValue(std::string& value) { value_ = value; }

std::string& Tree::GetValue() { return value_; }

void Tree::SetHeight(int height) { height_ = height; }

int Tree::GetHeight() { return height_; }

int Tree::computeHeightDepth(int depth)
{
	depth_ = depth;
	auto max = 0;
	for (auto it = children_.begin(); it != children_.end(); it++) {
		auto h = (*it)->computeHeightDepth(depth + 1);
		if (h > max) max = h;
	}

	height_ = max + 1;
	return height_;
}

int Tree::ComputeHeightDepth() {
	return computeHeightDepth(0);
}
