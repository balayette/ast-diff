#pragma once

#include <iostream>
#include <set>
#include <string>

class Symbol {
public:
  Symbol(const std::string &str);
  Symbol(const char *str = "");

  Symbol &operator=(const Symbol &rhs);

  bool operator==(const Symbol &rhs) const;
  bool operator!=(const Symbol &rhs) const;

  size_t size() const;

  friend std::ostream &operator<<(std::ostream &ostr, const Symbol &the);

  const std::string &get() const;

private:
  std::set<std::string> &getSet();
  const std::string *instance_;
};
