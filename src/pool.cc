#include "pool.hh"
#include <mutex>
#include <vector>

namespace pool {

static std::mutex mutex_;
static std::vector<Tree *> trees_;

void AddTree(Tree *t) {
  std::lock_guard<std::mutex> lock(mutex_);
  t->PostorderTraversal([](auto *t) {
    t->SetIdx(trees_.size());
    trees_.push_back(t);
  });
}

std::pair<iterator, iterator> GetDescendants(Tree *t) {
  return std::make_pair(trees_.begin() + t->GetLeftMostDesc(),
                        trees_.begin() + t->GetRightMostDesc() + 1);
}

} // namespace pool
