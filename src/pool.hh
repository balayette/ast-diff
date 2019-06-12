#pragma once

#include "tree.hh"
#include <utility>

namespace pool {
using iterator = std::vector<Tree *>::iterator;
void AddTree(Tree *t);
std::pair<iterator, iterator> GetDescendants(Tree *t);
} // namespace pool
