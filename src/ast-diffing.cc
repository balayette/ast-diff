#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "algo.hh"
#include "heap.hh"
#include "lexer.hh"
#include "parser.hh"

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "usage: ast-diffing FILENAME1 FILENAME2\n";
		std::exit(1);
	}

	std::ifstream f1(argv[1]);
	if (!f1)
	{
		std::cerr << "Couldn't open file " << argv[1] << '\n';
		std::exit(2);
	}

	std::ifstream f2(argv[2]);
	if (!f2)
	{
		std::cerr << "Couldn't open file " << argv[2] << '\n';
		std::exit(2);
	}

	Lexer l(f1);
	Lexer l2(f2);

	Parser p(l);
	Parser p2(l2);

	auto ret = p.Parse();
	ret->ComputeHeightDepth();
	auto ret2 = p2.Parse();
	ret2->ComputeHeightDepth();

	ret->PrettyPrint(std::cout) << '\n';
	ret2->PrettyPrint(std::cout) << '\n';

	std::ofstream out1("out1.dot");
	std::ofstream out2("out2.dot");

	ret->DumpDot(out1);
	ret2->DumpDot(out2);

	auto mapping = Gumtree(ret, ret2);

	for (auto& it : mapping)
	{
		std::cout << it.first->GetValue() << " mapped to "
			  << it.second->GetValue() << '\n';
	}

	std::ofstream map("map.dot");
	DumpMapping(map, ret, ret2, mapping);
}
