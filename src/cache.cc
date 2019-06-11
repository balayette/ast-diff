#include "cache.hh"

#include <fstream>
#include <iostream>
#include <mutex>

#include "lexer.hh"
#include "parser.hh"

Tree *Cache::OpenAst(const std::string &path, bool location,
                     const std::string &loc) {
  std::lock_guard<std::mutex> lock(cache_lock_);

  auto it = trees.find(path);
  if (it != trees.end())
    return (*it).second;

  std::ifstream f(path);
  if (!f) {
    std::cerr << "Couldn't open file " << path << '\n';
    std::exit(2);
  }

  Lexer l(f, path);
  Parser p(l);

  auto ret = p.Parse();
  ret->InitTree();

  if (location) {
    std::ifstream locpath(std::string(path).append(loc));
    if (!locpath)
      std::cerr << "Couldn't open location file for " << path << '\n';
    else
      ret->LoadLocation(locpath);
  }

  auto [r, ins] = trees.emplace(path, ret);
  return (*r).second;
}
