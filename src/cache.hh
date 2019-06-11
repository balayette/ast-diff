#pragma once

#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "tree.hh"

class Cache {
public:
  Tree *OpenAst(const std::string &path, bool location, const std::string &loc);

private:
  std::unordered_map<std::string, Tree *> trees;

  std::mutex cache_lock_;
};
