#pragma once

#include <memory>
#include <vector>

#include "tree.hh"
#include <unordered_map>

class Mappings {
public:
  using treeptr = Tree *;
  using mapping_store = std::vector<treeptr>;
  using mapping_iterator = mapping_store::iterator;

  Mappings(size_t nodeCountFirst, size_t nodeCountSecond);

  void AddMapping(treeptr t1, treeptr t2);

  bool ContainsMapping(treeptr t1, treeptr t2);

  bool ContainsSourceMapping(treeptr t);
  bool ContainsDestinationMapping(treeptr t);

  treeptr GetDestination(treeptr t);
  treeptr GetSource(treeptr t);

  int size();

  mapping_iterator begin();
  mapping_iterator end();

  const mapping_store &GetMappingsStore();
  const mapping_store &GetDestinationStore();

private:
  mapping_store mappings_;
  mapping_store destinations_;
  int mapping_nbr_;
};

void DumpMapping2(std::ostream &stream, Tree *t1, Mappings &v);

Tree::vecpair MappingsVec2(Tree *t1, Mappings &v);
