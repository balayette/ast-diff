#include "utils.hh"

#include <unordered_set>

template <typename Iterator, typename Func>
void foreach_pair(Iterator b1, Iterator e1, Iterator b2, Iterator e2, Func f) {
  for (auto it1 = b1; it1 != e1; it1++) {
    for (auto it2 = b2; it2 != e2; it2++) {
      f(*it1, *it2);
    }
  }
}

template <typename Container, typename Func>
void foreach_pair(Container &i1, Container &i2, Func f) {
  foreach_pair(i1.begin(), i1.end(), i2.begin(), i2.end(), f);
}

template <typename Iterator>
int common_elements(Iterator b1, Iterator e1, Iterator b2, Iterator e2) {
  int ret = 0;

  for (auto it1 = b1; it1 != e1; it1++) {
    for (auto it2 = b2; it2 != e2; it2++) {
      if (*it1 == *it2)
        ret++;
    }
  }

  return ret;
}
