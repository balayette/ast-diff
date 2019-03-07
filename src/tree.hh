#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Tree
{
    public:
	Tree();
	Tree(std::string& value, std::shared_ptr<Tree>& parent);

	void AddChild(std::shared_ptr<Tree>& tree);
	std::ostream& Print(std::ostream& stream);

	size_t ChildrenSize();
	std::vector<std::shared_ptr<Tree>>& GetChildren();

	void SetValue(std::string& value);
	std::string& GetValue();

	void SetHeight(int height);
	int GetHeight();
	int ComputeHeightDepth();

	std::shared_ptr<Tree>& GetParent();
	void SetParent(std::shared_ptr<Tree>& p);

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
	std::shared_ptr<Tree> parent_;

	inline static int node_count_ = 0;

	int height_;
	int depth_;
	int idx_;

	int computeHeightDepth(int depth);
	void dumpDot(std::ostream& stream);
};

std::vector<std::shared_ptr<Tree>> GetDescendants(std::shared_ptr<Tree>& t);

template <typename Pred>
std::shared_ptr<Tree> FindIf(std::shared_ptr<Tree> t, Pred f);

template <typename Pred>
std::vector<std::shared_ptr<Tree>> FindAll(std::shared_ptr<Tree>& t, Pred f);

#include "tree.hxx"
