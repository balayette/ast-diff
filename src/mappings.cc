#include "mappings.hh"

#include <algorithm>

Mappings::Mappings(size_t nodeCountFirst, size_t nodeCountSecond)
    : mappings_(nodeCountFirst, nullptr),
      destinations_(nodeCountSecond, nullptr), mapping_nbr_(0) {}

void Mappings::AddMapping(treeptr t1, treeptr t2) {
  if (mappings_[t1->GetIdx()] != nullptr)
    return;
  mappings_[t1->GetIdx()] = t2;
  destinations_[t2->GetIdx() - mappings_.size()] = t1;
  mapping_nbr_++;
}

bool Mappings::ContainsMapping(treeptr t1, treeptr t2) {
  return mappings_[t1->GetIdx()] == t2;
}

bool Mappings::ContainsSourceMapping(treeptr t) {
  return mappings_[t->GetIdx()] != nullptr;
}

bool Mappings::ContainsDestinationMapping(treeptr t) {
  return destinations_[t->GetIdx() - mappings_.size()] != nullptr;
}

Mappings::treeptr Mappings::GetDestination(treeptr t) {
  return mappings_[t->GetIdx()];
}

int Mappings::size() { return mapping_nbr_; }

Mappings::mapping_iterator Mappings::begin() { return mappings_.begin(); }

Mappings::mapping_iterator Mappings::end() { return mappings_.end(); }

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
    stream << idx << " -> " << (*it)->GetIdx()
           << " [fillcolor = blue] [color = blue] [style = dashed] "
              "[constraint = false];\n";
  }

  stream << "}\n";
}
