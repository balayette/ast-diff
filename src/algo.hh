#pragma once

#include <map>
#include <memory>

#include "tree.hh"

std::vector<std::pair<std::shared_ptr<Tree>, std::shared_ptr<Tree>>>
Gumtree(std::shared_ptr<Tree> t1, std::shared_ptr<Tree> t2);
