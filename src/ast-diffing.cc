#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "heap.hh"
#include "lexer.hh"
#include "parser.hh"

int main(void) {
	std::string s("(Nicolas\n\t(a \"c\" d) (e (f (g (h) i))))");

	std::istringstream f(s);

	Lexer l(f);

	Parser p(l);

	auto ret = p.Parse();
	ret->ComputeHeightDepth();

	ret->PrettyPrint(std::cout) << '\n';

	std::ofstream output("out.dot");
	ret->DumpDot(output);

	Heap h;
	ret->PreorderTraversal(
	    [&](Tree* t) { h.Push(std::make_shared<Tree>(*t)); });

	while (h.size() > 0)
	{
		std::cout << "max: " << h.PeekMax() << '\n';
		auto vec = h.Pop();
		for (auto& it : vec)
			std::cout << "  " << it->GetValue() << '\n';
	}

	return 0;
}
