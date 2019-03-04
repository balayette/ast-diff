#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Tree
{
    public:
	Tree();
	Tree(std::string& value, Tree* parent);

	void AddChild(std::shared_ptr<Tree>& tree);
	std::ostream& Print(std::ostream& stream);

	size_t ChildrenSize();
	std::vector<std::shared_ptr<Tree>>& GetChildren();

	void SetValue(std::string& value);
	std::string& GetValue();

	void SetHeight(int height);
	int GetHeight();
	int ComputeHeightDepth();

	std::ostream& PrettyPrint(std::ostream& stream);
	std::ostream& DumpDot(std::ostream& stream);

	template <typename Func>
	void PreorderTraversal(Func f);

	template <typename Func>
	void PostorderTraversal(Func f);

	bool IsIsomorphic(std::shared_ptr<Tree>& t);

    private:
	std::string value_;
	std::vector<std::shared_ptr<Tree>> children_;
	Tree* parent_;

	inline static int node_count_ = 0;

	int height_;
	int depth_;
	int idx_;

	int computeHeightDepth(int depth);
	void dumpDot(std::ostream& stream);
};

#include "tree.hxx"
