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
  std::tuple<bool, Tree::sptr> OpenAst(const std::string &path);

  const std::vector<std::string> *OpenLocation(const std::string &path);

private:
  std::unordered_map<std::string, Tree::sptr> trees;
  std::unordered_map<std::string, std::vector<std::string>> locs;
};
