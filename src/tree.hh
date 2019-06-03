#pragma once

#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "symbol.hh"

class Mappings;

class Tree {
public:
  using sptr = std::shared_ptr<Tree>;
  using vecsptr = std::vector<sptr>;
  using vecptr = std::vector<Tree *>;
  using pair = std::pair<Tree *, Tree *>;
  using vecpair = std::vector<pair>;

  Tree();
  Tree(const std::string &value);

  void AddChild(Tree::sptr &tree);
  std::ostream &Print(std::ostream &stream) const;

  size_t ChildrenSize() const;
  const vecsptr &GetChildren() const;

  void SetValue(const std::string &value);
  const Symbol &GetValue() const;

  void SetHeight(size_t height);
  size_t GetHeight() const;
  size_t InitTree();

  void LoadLocation(std::istream &stream);
  void LoadLocation(const std::vector<std::string> &loc);
  const std::string &GetLocationInfo();

  const Tree *GetParent() const;
  void SetParent(Tree *p);

  int GetIdx() const;

  int GetLeftMostDesc() const;
  int GetRightMostDesc() const;

  std::ostream &PrettyPrint(std::ostream &stream);
  std::ostream &DumpDot(std::ostream &stream) const;

  template <typename Func> void PreorderTraversal(Func f);

  template <typename Func> void PostorderTraversal(Func f);

  Tree *FindIsomorphicChild(const Tree *t) const;
  bool IsIsomorphic(const Tree *t) const;

  bool IsDescendantOf(const Tree *t) const;

  friend void DumpMapping(std::ostream &stream, const Tree *t1, const Tree *t2,
                          Mappings &v);

  friend void DumpMapping2(std::ostream &stream, const Tree *t1, const Tree *t2,
                           Mappings &v);

  friend std::ostream &operator<<(std::ostream &os, const Tree &tree);

private:
  Symbol value_;
  const Tree *parent_;
  std::string location_info_;
  int tree_id_;
  int idx_;
  size_t height_;
  size_t depth_;
  int left_desc_;
  int right_desc_;

  std::recursive_mutex iso_lock_;

  static inline std::atomic<int> tree_count_;

  vecsptr children_;
  std::unordered_set<Tree *> iso_cache_;

  size_t initTree(size_t depth, const Tree *parent);
  void dumpDot(std::ostream &stream) const;
};

Tree::vecptr GetDescendants(Tree *t);

template <typename Pred> Tree *FindIf(Tree &t, Pred f);

template <typename Pred> Tree::vecptr FindAll(Tree &t, Pred f);

#include "tree.hxx"
