#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "lexer.hh"
#include "parser.hh"

int main(void)
{
	std::string s("(Nicolas\n\t(a \"aa\" a) (a (a (lo (a) l))))");

	std::istringstream f(s);

	Lexer l(f);

	Parser p(l);

	auto ret = p.Parse();

	ret->Print(std::cout);

	return 0;
}
