#include "symbol.hh"

Symbol::Symbol(const std::string &str) {
  getSet().insert(str);
  instance_ = &(*(getSet().find(str)));
}

Symbol::Symbol(const char *str) : Symbol(std::string(str)) {}

std::set<std::string> &Symbol::getSet() {
  static std::set<std::string> set{};

  return set;
}

Symbol &Symbol::operator=(const Symbol &rhs) {
  if (this == &rhs)
    return *this;

  instance_ = rhs.instance_;
  return *this;
}

bool Symbol::operator==(const Symbol &rhs) {
  return rhs.instance_ == instance_;
}

bool Symbol::operator!=(const Symbol &rhs) {
  return !(rhs.instance_ == instance_);
}

std::ostream &operator<<(std::ostream &ostr, const Symbol &the) {
  return ostr << *the.instance_;
}

int Symbol::size() { return instance_->size(); }

const std::string &Symbol::get() { return *instance_; }
