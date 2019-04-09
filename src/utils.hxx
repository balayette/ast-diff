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
