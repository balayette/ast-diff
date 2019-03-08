#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "symbol.hh"
#include "mappings.hh"

class Tree
{
    public:
	using ptr = std::shared_ptr<Tree>;
	using vecptr = std::vector<std::shared_ptr<Tree>>;
	using pair = std::pair<ptr, ptr>;
	using vecpair = std::vector<pair>;

	Tree();
	Tree(std::string& value, ptr& parent);

	void AddChild(ptr& tree);
	std::ostream& Print(std::ostream& stream);

	size_t ChildrenSize();
	std::vector<ptr>& GetChildren();

	void SetValue(std::string& value);
	Symbol& GetValue();

	void SetHeight(int height);
	int GetHeight();
	int ComputeHeightDepth();

	ptr& GetParent();
	void SetParent(ptr& p);

	std::ostream& PrettyPrint(std::ostream& stream);
	std::ostream& DumpDot(std::ostream& stream);

	template <typename Func>
	void PreorderTraversal(Func f);

	template <typename Func>
	void PostorderTraversal(Func f);

	bool IsIsomorphic(ptr& t);

	friend void DumpMapping(std::ostream& stream, ptr& t1, ptr& t2,
				Mappings& v);

    private:
	Symbol value_;
	vecptr children_;
	ptr parent_;

	inline static int node_count_ = 0;

	int height_;
	int depth_;
	int idx_;

	int computeHeightDepth(int depth);
	void dumpDot(std::ostream& stream);
};

Tree::vecptr GetDescendants(Tree::ptr& t);

template <typename Pred>
Tree::ptr FindIf(Tree::ptr t, Pred f);

template <typename Pred>
Tree::vecptr FindAll(Tree::ptr& t, Pred f);

#include "tree.hxx"
