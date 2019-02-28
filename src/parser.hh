#pragma once

#include "token.hh"
#include "lexer.hh"
#include "tree.hh"
#include <memory>

class Parser
{
public:
	Parser(Lexer& lexer);

	std::shared_ptr<Tree> Parse();
private:
	Lexer& lexer_;
};
