#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "mappings.hh"
#include "symbol.hh"

class Tree
{
    public:
	using ptr = std::shared_ptr<Tree>;
	using vecptr = std::vector<std::shared_ptr<Tree>>;
	using opt = std::optional<Tree>;
	using refwrap = std::reference_wrapper<Tree>;
	using optref = std::optional<refwrap>;
	using vecref = std::vector<refwrap>;
	using pair = std::pair<ptr, ptr>;
	using vecpair = std::vector<pair>;

	Tree();
	Tree(std::string& value);

	void AddChild(Tree::ptr& tree);
	std::ostream& Print(std::ostream& stream);

	size_t ChildrenSize();
	vecptr& GetChildren();

	void SetValue(std::string& value);
	Symbol& GetValue();

	void SetHeight(int height);
	int GetHeight();
	int ComputeHeightDepth();

	optref GetParent();
	void SetParent(Tree& p);

	std::ostream& PrettyPrint(std::ostream& stream);
	std::ostream& DumpDot(std::ostream& stream);

	template <typename Func>
	void PreorderTraversal(Func f);

	template <typename Func>
	void PostorderTraversal(Func f);

	bool IsIsomorphic(Tree& t);

	Tree::optref Candidate(Tree& t1, Mappings& M);

	bool IsDescendantOf(Tree& t);

	double Dice(Tree& t2, Mappings& M);

	friend void DumpMapping(std::ostream& stream, Tree& t1, Tree& t2,
				Mappings& v);

	bool operator==(const Tree& t);
	bool operator!=(const Tree& t);

    private:
	Symbol value_;
	vecptr children_;
	Tree* parent_;

	inline static int node_count_ = 0;

	int height_;
	int depth_;
	int idx_;

	int computeHeightDepth(int depth, Tree* parent);
	void dumpDot(std::ostream& stream);
};

Tree::vecref GetDescendants(Tree& t);

template <typename Pred>
Tree::optref FindIf(Tree& t, Pred f);

template <typename Pred>
Tree::vecref FindAll(Tree& t, Pred f);

bool operator==(std::reference_wrapper<Tree> a, std::reference_wrapper<Tree> b);

#include "tree.hxx"
