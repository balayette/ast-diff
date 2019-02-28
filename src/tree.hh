#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>

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
private:
	std::string value_;
	std::vector<std::shared_ptr<Tree>> children_;
	Tree* parent_;
};
