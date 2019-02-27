#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "lexer.hh"

int main(void)
{
	std::string s("(ok) (ok) (((ok \"ok\\\"ok\" aa) b) c)");

	std::istringstream f(s);

	Lexer l(f);

	while (std::shared_ptr<Token> t = l.Peek())
	{
		std::cout << *t << '\n';
		l.Eat();
	}

	return 0;
}
