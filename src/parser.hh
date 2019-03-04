#pragma once

#include <memory>

#include "lexer.hh"
#include "token.hh"
#include "tree.hh"

class Parser
{
    public:
	Parser(Lexer& lexer);

	std::shared_ptr<Tree> Parse();

    private:
	Lexer& lexer_;
};
