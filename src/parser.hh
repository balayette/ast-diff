#pragma once

#include <memory>

#include "lexer.hh"
#include "token.hh"
#include "tree.hh"

class Parser {
public:
  Parser(Lexer &lexer);

  Tree::sptr Parse();

private:
  Lexer &lexer_;
};
