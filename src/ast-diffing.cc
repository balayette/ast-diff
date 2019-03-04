#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "heap.hh"
#include "lexer.hh"
#include "parser.hh"

int main(void)
{
	std::string s("(Nicolas\n\t(a \"c\" d) (e (f (g (h) i))))");
	std::string s2("(Nicolas\n\t(e (f (g (i) h))) (a d \"c\"))");

	std::istringstream f(s);
	std::istringstream f2(s2);

	Lexer l(f);
	Lexer l2(f2);

	Parser p(l);
	Parser p2(l2);

	auto ret = p.Parse();
	ret->ComputeHeightDepth();
	auto ret2 = p2.Parse();
	ret2->ComputeHeightDepth();

	ret->PrettyPrint(std::cout) << '\n';
	ret2->PrettyPrint(std::cout) << '\n';

	std::ofstream output("out.dot");
	ret->DumpDot(output);

	std::cout << ret->IsIsomorphic(ret2) << '\n';

	return 0;
}
