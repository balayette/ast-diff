#include "cache.hh"

#include <fstream>
#include <iostream>

#include "lexer.hh"
#include "parser.hh"

std::tuple<bool, Tree::sptr> Cache::OpenAst(const std::string &path) {
  auto it = trees.find(path);
  if (it != trees.end())
    return std::make_tuple(true, (*it).second);

  std::ifstream f(path);
  if (!f) {
    std::cerr << "Couldn't open file " << path << '\n';
    std::exit(2);
  }

  Lexer l(f);
  Parser p(l);

  auto ret = p.Parse();

  auto [r, ins] = trees.emplace(path, ret);
  return std::make_tuple(false, (*r).second);
}

const std::vector<std::string> *Cache::OpenLocation(const std::string &path) {
  auto it = locs.find(path);
  if (it != locs.end())
    return &((*it).second);

  std::ifstream f(path);
  if (!f)
    return nullptr;

  std::vector<std::string> vec;
  std::string tmp;
  while (std::getline(f, tmp))
    vec.push_back(tmp);

  auto [r, ins] = locs.emplace(path, vec);
  return &((*r).second);
}
