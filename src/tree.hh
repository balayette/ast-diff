#pragma once

#include <functional>
#include <iostream>
#include <memory>
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
  Tree(std::string &value);

  void AddChild(Tree::sptr &tree);
  std::ostream &Print(std::ostream &stream);

  size_t ChildrenSize();
  vecsptr &GetChildren();

  void SetValue(std::string &value);
  Symbol &GetValue();

  void SetHeight(int height);
  int GetHeight();
  int InitTree();

  void LoadLocation(std::istream &stream);
  void LoadLocation(const std::vector<std::string> &loc);
  const std::string &GetLocationInfo();

  Tree *GetParent();
  void SetParent(Tree *p);

  int GetIdx();

  int GetLeftMostDesc();
  int GetRightMostDesc();

  std::ostream &PrettyPrint(std::ostream &stream);
  std::ostream &DumpDot(std::ostream &stream);

  template <typename Func> void PreorderTraversal(Func f);

  template <typename Func> void PostorderTraversal(Func f);

  Tree *FindIsomorphicChild(Tree *t);
  bool IsIsomorphic(Tree *t);

  bool IsDescendantOf(Tree *t);

  friend void DumpMapping(std::ostream &stream, Tree *t1, Tree *t2,
                          Mappings &v);

  friend void DumpMapping2(std::ostream &stream, Tree *t1, Tree *t2,
                           Mappings &v);

private:
  Symbol value_;
  vecsptr children_;
  Tree *parent_;

  std::unordered_set<Tree *> iso_cache_;

  std::string location_info_;

  inline static int node_count_ = 0;

  int height_;
  int depth_;
  int idx_;
  int left_desc_;
  int right_desc_;

  int initTree(int depth, Tree *parent);
  void dumpDot(std::ostream &stream);
};

Tree::vecptr GetDescendants(Tree *t);

template <typename Pred> Tree *FindIf(Tree &t, Pred f);

template <typename Pred> Tree::vecptr FindAll(Tree &t, Pred f);

#include "tree.hxx"
