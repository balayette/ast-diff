#include "tree.hh"

#include <algorithm>
#include <string>

Tree::Tree()
    : value_(""), parent_(nullptr), location_info_(""),
      tree_id_(tree_count_.fetch_add(1, std::memory_order_relaxed)), idx_(0),
      height_(1), depth_(1), left_desc_(0), right_desc_(0) {}

Tree::Tree(const std::string &value) : Tree() { value_ = value; }

void Tree::AddChild(Tree::sptr &tree) { children_.push_back(tree); }

std::ostream &Tree::Print(std::ostream &stream) const {
  stream << '(' << value_;
  for (auto it = children_.begin(); it != children_.end(); it++) {
    stream << ' ';
    (*it)->Print(stream);
  }
  stream << ')';

  return stream;
}

std::ostream &Tree::PrettyPrint(std::ostream &stream) {
  for (size_t i = 0; i < depth_; i++)
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

size_t Tree::ChildrenSize() const { return children_.size(); }

const Tree::vecsptr &Tree::GetChildren() const { return children_; }

void Tree::SetValue(const std::string &value) { value_ = value; }

const Symbol &Tree::GetValue() const { return value_; }

void Tree::SetHeight(size_t height) { height_ = height; }

size_t Tree::GetHeight() const { return height_; }

const Tree *Tree::GetParent() const { return parent_; }

void Tree::SetParent(Tree *p) { parent_ = p; }

size_t Tree::initTree(size_t depth, const Tree *parent) {
  depth_ = depth;
  size_t max = 0;

  for (auto &it : children_) {
    auto h = it->initTree(depth + 1, this);
    max = std::max(h, max);
    left_desc_ = std::min(left_desc_, it->left_desc_);
    right_desc_ = std::max(right_desc_, it->right_desc_);
  }

  height_ = max + 1;
  parent_ = parent;
  return height_;
}

size_t Tree::InitTree() {
  size_t node_count = 0;
  PostorderTraversal([&](Tree *t) {
    t->idx_ = node_count++;
    t->left_desc_ = t->idx_;
    t->right_desc_ = t->idx_;
  });
  return initTree(0, nullptr);
}

void Tree::dumpDot(std::ostream &stream) const {
  std::string out(value_.get());

  for (size_t f = out.find("\"", 0); f != std::string::npos;
       f = out.find("\"", f + 2))
    out.replace(f, 1, "\\\"");

  stream << "\t" << *this << " [label=\"" << out << "\"] [tooltip=\""
         << location_info_ << "\"];\n";

  for (auto &it : children_)
    stream << "\t" << *this << " -> " << *it << ";\n";

  stream << '\n';

  for (auto &it : children_)
    it->dumpDot(stream);
}

std::ostream &Tree::DumpDot(std::ostream &stream) const {
  stream << "digraph AST {\n";

  dumpDot(stream);

  return stream << "}\n";
}

// Find an isomorphic child to `this` among the children of t.
Tree *Tree::FindIsomorphicChild(const Tree *t) const {
  for (auto &other : t->children_) {
    if (IsIsomorphic(other.get())) {
      return other.get();
    }
  }

  return nullptr;
}

bool Tree::IsIsomorphic(const Tree *t) const {
  if (height_ != t->height_)
    return false;

  if (children_.size() != t->children_.size())
    return false;

  if (value_ != t->value_)
    return false;

  for (auto &me : children_) {
    auto *iso = me->FindIsomorphicChild(t);
    if (iso == nullptr)
      return false;
  }

  return true;
}

void getDescendants(Tree *t, Tree::vecptr &v) {
  auto &children = t->GetChildren();
  for (const auto &child : children)
    v.push_back(child.get());

  for (auto &it : children)
    getDescendants(it.get(), v);
}

Tree::vecptr GetDescendants(Tree *t) {
  Tree::vecptr v;

  getDescendants(t, v);

  return v;
}

bool Tree::IsDescendantOf(const Tree *t) const {
  return idx_ >= t->left_desc_ && idx_ <= t->right_desc_;
}

int Tree::GetIdx() const { return idx_; }

int Tree::GetLeftMostDesc() const { return left_desc_; }
int Tree::GetRightMostDesc() const { return right_desc_; }

void Tree::LoadLocation(std::istream &stream) {
  PreorderTraversal([&](Tree *f) { std::getline(stream, f->location_info_); });
}

void Tree::LoadLocation(const std::vector<std::string> &loc) {
  int idx = 0;
  PreorderTraversal([&](Tree *t) { t->location_info_ = loc[idx++]; });
}

const std::string &Tree::GetLocationInfo() { return location_info_; }

std::ostream &operator<<(std::ostream &os, const Tree &tree) {
  return os << '"' << tree.tree_id_ << '-' << tree.idx_ << '"';
}
