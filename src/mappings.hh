#pragma once

#include <memory>
#include <vector>

class Tree;

class Mappings {
public:
  using treeptr = Tree *;
  using treepair = std::pair<treeptr, treeptr>;
  using mapping_store = std::vector<treepair>;
  using mapping_iterator = mapping_store::iterator;

  void AddMapping(treeptr t1, treeptr t2);

  bool ContainsMapping(treeptr t1, treeptr t2);

  bool ContainsSourceMapping(treeptr t);
  bool ContainsDestinationMapping(treeptr t);

  treeptr GetDestination(treeptr t);

  int size();

  mapping_iterator begin();
  mapping_iterator end();

  treepair &operator[](const int idx);

  mapping_iterator erase(mapping_iterator position);

private:
  mapping_store mappings_;
};
