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

  void AddMapping(Tree *t1, Tree *t2);

  bool ContainsMapping(const Tree *t1, const Tree *t2) const;

  bool ContainsSourceMapping(const Tree *t) const;
  bool ContainsDestinationMapping(const Tree *t) const;

  Tree *GetDestination(const Tree *t) const;
  Tree *GetSource(const Tree *t) const;

  size_t size() const;

  mapping_iterator begin();
  mapping_iterator end();

  const mapping_store &GetMappingsStore() const;
  const mapping_store &GetDestinationStore() const;

private:
  mapping_store mappings_;
  mapping_store destinations_;
  size_t mapping_nbr_;
};

Tree::vecpair MappingsVec2(Tree *t1, Mappings &v);
Tree::vecpair MappingsVec(Tree *t1, Mappings &v);
