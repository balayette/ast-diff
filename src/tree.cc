#include "tree.hh"

#include <algorithm>

Tree::Tree() : value_(""), parent_(nullptr) { idx_ = node_count_++; }

Tree::Tree(std::string &value)
    : value_(value), parent_(nullptr), height_(1), depth_(1) {
  idx_ = node_count_++;
}

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

int Tree::computeHeightDepth(int depth, Tree *parent) {
  depth_ = depth;
  auto max = 0;

  for (auto &it : children_) {
    auto h = it->computeHeightDepth(depth + 1, this);
    if (h > max)
      max = h;
  }

  height_ = max + 1;
  parent_ = parent;
  return height_;
}

int Tree::ComputeHeightDepth() { return computeHeightDepth(0, nullptr); }

void Tree::dumpDot(std::ostream &stream) {
  std::string out(value_.get());

  for (size_t f = out.find("\"", 0); f != std::string::npos;
       f = out.find("\"", f + 2))
    out.replace(f, 1, "\\\"");

  stream << '\t' << idx_ << " [label=\"" << out << "\"];\n";

  for (auto &it : children_)
    stream << '\t' << idx_ << " -> " << it->idx_ << ";\n";

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
Tree* Tree::FindIsomorphicChild(Tree *t) {
  for (auto &other : t->children_) {
    if (IsIsomorphic(other.get())) {
      return other.get();
    }
  }

  return nullptr;
}

bool Tree::IsIsomorphic(Tree *t) {
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
    iso->iso_cache_.insert(me.get());
  }

  iso_cache_.insert(t);
  t->iso_cache_.insert(this);
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

void DumpMapping(std::ostream &stream, Tree *t1, Tree *t2, Mappings &v) {
  stream << "digraph G {\n\tsubgraph AST1 {\n";
  t1->dumpDot(stream);
  stream << "}\n\tsubgraph AST2 {\n";
  t2->dumpDot(stream);
  stream << "}\n";

  for (auto p : v)
    stream << p.first->idx_ << " -> " << p.second->idx_
           << " [fillcolor = blue] [color = blue] [style = dashed] "
              "[constraint = false];\n";

  stream << "}\n";
}

bool Tree::IsDescendantOf(Tree *t) {
  if (!parent_)
    return false;

  if (t == this)
    return true;

  return parent_->IsDescendantOf(t);
}

double Tree::Dice(Tree *t2, Mappings &M) {
  std::cout << "Computing dice between " << value_ << " " << height_ << " and "
            << t2->value_ << " " << t2->height_;
  auto desc1 = GetDescendants(this);
  auto desc2 = GetDescendants(t2);

  double d = desc1.size() + desc2.size();

  int u = 0;
  for (auto &desc : desc1) {
    if (M.ContainsSourceMapping(desc))
      u++;
  }

  double ret = (double)(2 * u) / d;

  std::cout << " = " << ret << "\n";

  return ret;
}

Tree *Tree::Candidate(Tree *t1, Mappings &M) {
  Tree::vecptr candidates = FindAll(this, [&](Tree *c) {
    if (t1->value_ != c->value_)
      return false;
    if (M.ContainsDestinationMapping(c))
      return false;

    if (!std::any_of(M.begin(), M.end(), [&](Mappings::treepair &pair) {
          std::cout << pair.first->GetValue() << " child of " << t1->GetValue()
                    << "?\n";
          return pair.first->IsDescendantOf(t1) &&
                 pair.second->IsDescendantOf(c);
        }))
      return false;

    return true;
  });

  std::cout << "Candidates size " << candidates.size() << '\n';

  if (candidates.size() == 0)
    return nullptr;

  auto it = std::max_element(
      candidates.begin(), candidates.end(),
      [&](Tree *c1, Tree *c2) { return t1->Dice(c1, M) < t1->Dice(c2, M); });

  return *it;
}
