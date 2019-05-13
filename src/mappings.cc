#include "mappings.hh"

#include <algorithm>

Mappings::Mappings(size_t nodeCountFirst, size_t nodeCountSecond)
    : mappings_(nodeCountFirst, nullptr),
      destinations_(nodeCountSecond, nullptr), mapping_nbr_(0) {}

void Mappings::AddMapping(treeptr t1, treeptr t2) {
  if (mappings_[t1->GetIdx()] != nullptr)
    return;
  mappings_[t1->GetIdx()] = t2;
  destinations_[t2->GetIdx()] = t1;
  mapping_nbr_++;
}

bool Mappings::ContainsMapping(treeptr t1, treeptr t2) {
  return mappings_[t1->GetIdx()] == t2;
}

bool Mappings::ContainsSourceMapping(treeptr t) {
  return mappings_[t->GetIdx()] != nullptr;
}

bool Mappings::ContainsDestinationMapping(treeptr t) {
  return destinations_[t->GetIdx()] != nullptr;
}

Mappings::treeptr Mappings::GetDestination(treeptr t) {
  return mappings_[t->GetIdx()];
}

Mappings::treeptr Mappings::GetSource(treeptr t) {
  return destinations_[t->GetIdx()];
}

int Mappings::size() { return mapping_nbr_; }

Mappings::mapping_iterator Mappings::begin() { return mappings_.begin(); }

Mappings::mapping_iterator Mappings::end() { return mappings_.end(); }

void dumpMapping2(std::ostream &stream, Tree *t1, Mappings &v) {
  auto *dest = v.GetDestination(t1);
  if (dest) {
    stream << *t1 << " -> " << *dest
           << " [fillcolor = blue] [color = blue] [style = dashed] "
              "[constraint = false];\n";
    return;
  }

  for (auto &child : t1->GetChildren())
    dumpMapping2(stream, child.get(), v);
}

void DumpMapping2(std::ostream &stream, Tree *t1, Tree *t2, Mappings &v) {
  stream << "digraph G {\n\tsubgraph AST1 {\n";
  t1->dumpDot(stream);
  stream << "}\n\tsubgraph AST2 {\n";
  t2->dumpDot(stream);
  stream << "}\n";

  dumpMapping2(stream, t1, v);

  stream << '}';
}

void mappingsVec2(Tree *t1, Mappings &v, Tree::vecpair &ret) {
  auto *dest = v.GetDestination(t1);
  if (dest) {
    ret.emplace_back(t1, dest);
    return;
  }

  for (auto &child : t1->GetChildren())
    mappingsVec2(child.get(), v, ret);
}

Tree::vecpair MappingsVec2(Tree *t1, Mappings &v) {
  std::vector<std::pair<Tree *, Tree *>> ret;

  mappingsVec2(t1, v, ret);

  return ret;
}

void mappingsVec(Tree *t1, Mappings &v, Tree::vecpair &ret) {
  auto *dest = v.GetDestination(t1);
  if (dest) {
    ret.emplace_back(t1, dest);
  }

  for (auto &child : t1->GetChildren())
    mappingsVec(child.get(), v, ret);
}

Tree::vecpair MappingsVec(Tree *t1, Mappings &v) {
  std::vector<std::pair<Tree *, Tree *>> ret;

  mappingsVec(t1, v, ret);

  return ret;
}

void DumpMapping(std::ostream &stream, Tree *t1, Tree *t2, Mappings &v) {
  stream << "digraph G {\n\tsubgraph AST1 {\n";
  t1->dumpDot(stream);
  stream << "}\n\tsubgraph AST2 {\n";
  t2->dumpDot(stream);
  stream << "}\n";

  int idx = 0;
  for (auto it = v.begin(); it != v.end(); it++, idx++) {
    if (*it == nullptr)
      continue;
    stream << *(v.GetMappingsStore()[idx]) << " -> " << *it
           << " [fillcolor = blue] [color = blue] [style = dashed] "
              "[constraint = false];\n";
  }

  stream << "}\n";
}

const Mappings::mapping_store &Mappings::GetMappingsStore() {
  return mappings_;
}

const Mappings::mapping_store &Mappings::GetDestinationStore() {
  return destinations_;
}
