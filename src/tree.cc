#include "tree.hh"

#include <algorithm>
#include <string>

Tree::Tree()
    : value_(""), parent_(nullptr), location_info_(""),
      tree_id_(tree_count_.fetch_add(1, std::memory_order_relaxed)), idx_(0),
      height_(1), depth_(1), left_desc_(0), right_desc_(0) {}

Tree::Tree(std::string &value) : Tree() { value_ = value; }

void Tree::AddChild(Tree::sptr &tree) { children_.push_back(tree); }

std::ostream &Tree::Print(std::ostream &stream) {
  stream << '(' << value_;
  for (auto it = children_.begin(); it != children_.end(); it++) {
    stream << ' ';
    (*it)->Print(stream);
  }
  stream << ')';

  return stream;
}

std::ostream &Tree::PrettyPrint(std::ostream &stream) {
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

Tree::vecsptr &Tree::GetChildren() { return children_; }

void Tree::SetValue(std::string &value) { value_ = value; }

Symbol &Tree::GetValue() { return value_; }

void Tree::SetHeight(int height) { height_ = height; }

int Tree::GetHeight() { return height_; }

Tree *Tree::GetParent() { return parent_; }

void Tree::SetParent(Tree *p) { parent_ = p; }

int Tree::initTree(int depth, Tree *parent) {
  depth_ = depth;
  auto max = 0;

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

int Tree::InitTree() {
  int node_count = 0;
  PostorderTraversal([&](Tree *t) {
    t->idx_ = node_count++;
    t->left_desc_ = t->idx_;
    t->right_desc_ = t->idx_;
  });
  return initTree(0, nullptr);
}

void Tree::dumpDot(std::ostream &stream) {
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

std::ostream &Tree::DumpDot(std::ostream &stream) {
  stream << "digraph AST {\n";

  dumpDot(stream);

  return stream << "}\n";
}

// Find an isomorphic child to `this` among the children of t.
Tree *Tree::FindIsomorphicChild(Tree *t) {
  for (auto &other : t->children_) {
    if (IsIsomorphic(other.get())) {
      return other.get();
    }
  }

  return nullptr;
}

bool Tree::IsIsomorphic(Tree *t) {
  std::lock_guard<std::recursive_mutex> lock(iso_lock_);

  auto found = iso_cache_.find(t);
  if (found != iso_cache_.end())
    return true;

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

    iso_cache_.insert(iso);
  }

  iso_cache_.insert(t);

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

bool Tree::IsDescendantOf(Tree *t) {
  return idx_ >= t->left_desc_ && idx_ <= t->right_desc_;
}

int Tree::GetIdx() { return idx_; }

int Tree::GetLeftMostDesc() { return left_desc_; }
int Tree::GetRightMostDesc() { return right_desc_; }

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
